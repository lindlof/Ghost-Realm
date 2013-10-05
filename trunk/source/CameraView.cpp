/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"
#include "s3e.h"
#include "s3eCamera.h"

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"
#include "IwAnim.h"

#include <sys/param.h>

#define GHOST_HIT_LENGTH 500
#define BLEND_DURATION 0.25f

void setupPlayer();
void renderCamera(CIwMaterial* pMat);
void renderGhost();
void renderVitality(CIwMaterial* pMat);

void cameraStreamInit(int camDataW, int camDataH);
static CIwSVec2 cameraVert[4];
static CIwFVec2 cameraUvs[4];
static CIwFVec2 cameraUvsRotated[4];

static CIwTexture* g_CameraTexture = NULL;

static CIwModel*       ghost_Model;
static CIwAnim*        ghost_Anims[1];
static CIwAnimSkel*    ghost_Skel;
static CIwAnimSkin*    ghost_Skin;
static CIwAnimPlayer*  ghost_Player;

static s3eCameraFrameRotation g_FrameRotation = S3E_CAMERA_FRAME_ROT90;
static CIwFMat viewMatrix;

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

// Camera callback. Copy the capture frame buffer into a texture ready for rendering.
static int32 frameReceived(void* systemData, void* userData)
{
    s3eCameraFrameData *data = (s3eCameraFrameData*)systemData;

    // If there is no texture, create one.
    // This is a slow operation compared to memcpy so we don't want to do it every frame.
    if (g_CameraTexture == NULL)
    {
		cameraStreamInit(data->m_Width, data->m_Height);

        g_CameraTexture = new CIwTexture();
        g_CameraTexture->SetMipMapping(false);
        g_CameraTexture->SetModifiable(true);

        g_CameraTexture->CopyFromBuffer(data->m_Width, data->m_Height, CIwImage::RGB_565, data->m_Pitch, (uint8*)data->m_Data, NULL);
        g_CameraTexture->Upload();
    }

    // Copy the camera image data into the texture. Note that it does not get copied to VRAM at this point.
    memcpy(g_CameraTexture->GetTexels(), data->m_Data, data->m_Height * data->m_Pitch);
    g_FrameRotation = data->m_Rotation;

    return 0;
}

void cameraStreamInit(int camDataW, int camDataH) {
	double w = IwGxGetScreenWidth();
	double h = IwGxGetScreenHeight();
	{
		double whRatio = ((double)camDataW) / camDataH;

		if (whRatio > w/h) {
			w = w * whRatio;
		} else if (whRatio < w/h) {
			h = h * 1.f/whRatio;
		}
	}

	// Full screen screenspace vertex coords
	int16 x1 = (int16)(-abs(w - IwGxGetScreenWidth())/2); // Negative or 0
	int16 x2 = (int16)(w - x1);
	int16 y1 = (int16)(-abs(h - IwGxGetScreenHeight())/2); // Negative or 0
	int16 y2 = (int16)(h - y1);

	cameraVert[0].x = x1, cameraVert[0].y = y1;
	cameraVert[1].x = x1, cameraVert[1].y = y2;
	cameraVert[2].x = x2, cameraVert[2].y = y2;
	cameraVert[3].x = x2, cameraVert[3].y = y1;

	{
		// Camera UV counter-mirrors the image.
		// Camera UV presents the image counterclockwise. Image is rotated in live.
		// Camera UV does cut parts of the image that bleed over the screen.
		// (so that waste doesn't get rendered)

		float uvX = ((float)y1) / h;
		float uvY = ((float)x1) / w;
		float uvW = 1.f - uvX;
		float uvH = 1.f - uvY;

		cameraUvs[0].x = uvX;
		cameraUvs[0].y = uvY;
		cameraUvs[1].x = uvX;
		cameraUvs[1].y = uvY + uvH;
		cameraUvs[2].x = uvX + uvW;
		cameraUvs[2].y = uvY + uvH;
		cameraUvs[3].x = uvX + uvW;
		cameraUvs[3].y = uvY;
	}
}

void CameraViewInit()
{
    IwGxInit();
	IwGraphicsInit();
	IwAnimInit();

    // Camera field of view
	IwGxSetPerspMul((float) IwGxGetScreenWidth() / 1.3);
    IwGxSetFarZNearZ(0x2000,0x10);

	// Set screen clear colour
    IwGxSetColClear(0xff, 0xff, 0xff, 0xff);
    IwGxPrintSetColour(128, 128, 128);

	// Load viking
	IwGetResManager()->LoadGroup("viking/viking.group");
	CIwResGroup* pGroup = IwGetResManager()->GetGroupNamed("viking");

    ghost_Model = (CIwModel*)pGroup->GetResNamed("body", IW_GRAPHICS_RESTYPE_MODEL);
    ghost_Skin  = (CIwAnimSkin*)pGroup->GetResNamed("body", IW_ANIM_RESTYPE_SKIN);
    ghost_Skel  = (CIwAnimSkel*)pGroup->GetResNamed("Armature", IW_ANIM_RESTYPE_SKELETON);
    ghost_Anims[0]  = (CIwAnim*)pGroup->GetResNamed("Armature", IW_ANIM_RESTYPE_ANIMATION);

    // Create animation player
    ghost_Player = new CIwAnimPlayer;
    ghost_Player->SetSkel(ghost_Skel);
    ghost_Player->PlayAnim(ghost_Anims[0], 1, CIwAnimBlendSource::LOOPING_F, BLEND_DURATION);

    // Set up camera capture
    if (s3eCameraAvailable())
    {
        // Register callback to receive camera frames
        s3eCameraRegister(S3E_CAMERA_UPDATE_STREAMING, frameReceived, NULL);

        // Request medium sized image (for decent frame rate) with a platform independent format
		s3eCameraStart(S3E_CAMERA_STREAMING_SIZE_HINT_MEDIUM, S3E_CAMERA_PIXEL_TYPE_RGB565_CONVERTED, 
			S3E_CAMERA_STREAMING_QUALITY_HINT_MED);
    }
    else
    {
        s3eDebugTraceLine("Camera Extension not available!");
    }
}

//-----------------------------------------------------------------------------
// The following function deletes textures if they exist and terminates
// IwGx and s3e API functionality.
//-----------------------------------------------------------------------------
void CameraViewTerm()
{
    if (g_CameraTexture)
        delete g_CameraTexture;

	delete ghost_Player;

    if (s3eCameraAvailable())
    {
        s3eCameraStop();
        s3eCameraUnRegister(S3E_CAMERA_UPDATE_STREAMING, frameReceived);
    }

	IwAnimTerminate();
	IwGraphicsTerminate();
    IwGxTerminate();
}

bool CameraViewUpdate()
{
	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	CIwMaterial* pMat = NULL;

	renderCamera(pMat);
	setupPlayer();
	renderGhost();
	renderVitality(pMat);

    IwGxFlush();
    IwGxSwapBuffers();

	return true;
}

void renderCamera(CIwMaterial* pMat) {
    // Refresh dynamic texture
    if (g_CameraTexture != NULL)
        g_CameraTexture->ChangeTexels(g_CameraTexture->GetTexels(), CIwImage::RGB_565);

    pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    pMat->SetTexture(g_CameraTexture);

    IwGxSetMaterial(pMat);

    // rotate UV coordinates based on rotation
    // (image must rotate against rotation of surface,
    // assuming that the camera image is NOT auto rotated)
    for (int j=0; j<4; j++)
    {
        cameraUvsRotated[j] = cameraUvs[(4 - (int)g_FrameRotation + j ) % 4];
    }

	IwGxSetScreenSpaceSlot(-1);

    IwGxSetUVStream(cameraUvsRotated);
    IwGxSetVertStreamScreenSpace(cameraVert, 4);

    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void setupPlayer() {
	Player *player = getPlayer();
	
    // Set viewing position with the Y axis as upright.
    viewMatrix.SetIdentity();
    viewMatrix.LookAt(CIwFVec3(0,0,0), CIwFVec3(0,0,100), CIwFVec3(0,-1,0));

    // Rotate according to compass heading
	viewMatrix.PostRotateY((float)rad(player->getHeading()));
    IwGxSetViewMatrix(&viewMatrix);
}

void renderGhost() {

	Ghost *ghost = getGhost();

	CIwFVec3 ghostPosition(0, 0, ghost->getDistance());
	static CIwFMat modelMatrix;

    // Place the markers on the edge of the compass radius
    // rotated to their correct bearing to current location
	modelMatrix.SetRotY((float)rad(ghost->getBearing()));
	modelMatrix.SetTrans(modelMatrix.RotateVec(ghostPosition));
	modelMatrix.PostRotateY(PI);

    IwGxSetModelMatrix(&modelMatrix);

	
	IwGxLightingAmbient(true);
	IwGxSetLightType(0, IW_GX_LIGHT_AMBIENT);
    CIwColour colA = {0xff, 0x00, 0x00, 0xff};
	
	int sinceHit = clock() - ghost->getHitTime();
	CIwColour colAmbient = {0xff, 0x00, 0x00, 0x00};

	if (sinceHit < GHOST_HIT_LENGTH) {
		int halfAnimation = GHOST_HIT_LENGTH/2;

		// Hit starts from 0, maxes out at 0xff and
		// then decreases back to 0. This all happens
		// in the animation length creating fade 
		// in fade out effect.
		int hit;

		// Divide the sinceHit time so that it maxes
		// out in 0xff
		hit = sinceHit / ((float)halfAnimation/0xff);

		if (sinceHit > halfAnimation) {
			// If we are over the half way let's start
			// decreasing the number from 0xff
			hit = 0xff + (0xff - hit);
		}

		colAmbient = 0xff, 0xff-hit, 0xff-hit, 0xff;
	} else {
		// The default state that displays the image as it is
		colAmbient = 0xffffffff;
	}
	
	IwGxSetLightCol(0, &colAmbient);


	// Update animation player
    ghost_Player->Update(1.0f / 30.0f);

    // Update IwGx state time stamp
    IwGxTickUpdate();

	IwGxSetScreenSpaceSlot(1);
    IwAnimSetSkelContext(ghost_Player->GetSkel());
    IwAnimSetSkinContext(ghost_Skin);
    ghost_Model->Render();

    // Tidier to reset these
    IwAnimSetSkelContext(NULL);
    IwAnimSetSkinContext(NULL);
}

void renderVitality(CIwMaterial* pMat) {

	Player *player = getPlayer();

	pMat = IW_GX_ALLOC_MATERIAL();
	pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	IwGxSetMaterial(pMat);

	// Vertex coords for full vitality
	int16 x1 = (int16)IwGxGetScreenWidth()/100 * 5;
    int16 x2 = (int16)IwGxGetScreenWidth()/100 * 95;
    int16 y1 = (int16)IwGxGetScreenHeight()/100 * 1;
    int16 y2 = (int16)IwGxGetScreenHeight()/100 * 2;

	// Full length of the bar
	int16 barLength = x2 - x1;
	// Multiply the full bar length with current vitality status
	barLength =  barLength * (float)player->getVitality() / MAX_PLAYER_VITALITY;

	x2 = x1 + barLength;

	CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
	cols[0].Set(0, 0, 0xff, 0x50);
	cols[1] = cols[2] = cols[3] = cols[0];

	CIwSVec2 XY(x1, y1), dXY(x2, y2);
	IwGxDrawRectScreenSpace(&XY, &dXY, cols);
}
