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

void setupPlayer();
void renderCamera(CIwMaterial* pMat);
void renderGhost(CIwMaterial* pMat);
void renderVitality(CIwMaterial* pMat);

void cameraStreamInit(int camDataW, int camDataH);
static CIwSVec2 cameraVert[4];
static CIwFVec2 cameraUvs[4];
static CIwFVec2 cameraUvsRotated[4];

static CIwTexture* g_CameraTexture = NULL;
static CIwTexture* g_GhostTexture = NULL;

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
	IwGxSetPerspMul((float) IwGxGetScreenWidth() / 2); // 90 deg FOV
    IwGxSetFarZNearZ(0x2000,0x10);

	// Set screen clear colour
    IwGxSetColClear(0xff, 0xff, 0xff, 0xff);
    IwGxPrintSetColour(128, 128, 128);

    g_GhostTexture = new CIwTexture;
    g_GhostTexture->LoadFromFile("textures/hammersmith_ghost.png");
    g_GhostTexture->Upload();

	IwGetResManager()->LoadGroup("viking/viking.group");
	CIwResGroup* pGroup = IwGetResManager()->GetGroupNamed("viking");

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
    // Release all textures
    if (g_GhostTexture)
        delete g_GhostTexture;

    if (g_CameraTexture)
        delete g_CameraTexture;

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
	renderGhost(pMat);
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

// Vertex data
const float s = 0x80;
CIwFVec3    s_Verts[8];

CIwColour   s_Cols[8] =
{
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
};

CIwFVec2 s_UVs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(1, 0),
    CIwFVec2(1, 1),
    CIwFVec2(0, 1),
};

// Index stream for textured material
uint16      s_QuadStrip[4] =
{
	0, 3, 1, 2,
};

void renderGhost(CIwMaterial* pMat) {

	Ghost *ghost = getGhost();

	CIwFVec3 ghostPosition(0, 0, ghost->getDistance());
	static CIwFMat modelMatrix;

    // Place the markers on the edge of the compass radius
    // rotated to their correct bearing to current location
	modelMatrix.SetRotY((float)rad(ghost->getBearing()));
	modelMatrix.SetTrans(modelMatrix.RotateVec(ghostPosition));
    IwGxSetModelMatrix(&modelMatrix);
	

    pMat = IW_GX_ALLOC_MATERIAL();
	pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
	pMat->SetTexture(g_GhostTexture);
	
	IwGxLightingEmissive(true);

	int sinceHit = clock() - ghost->getHitTime();

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

		pMat->SetColEmissive(0xff, 0xff-hit, 0xff-hit, 0xff);
	} else {
		// The default state that displays the image as it is
		pMat->SetColEmissive(0xffffffff);
	}
	
    IwGxSetMaterial(pMat);


	IwGxSetScreenSpaceSlot(1);

	{
		// Model space 3d vertex coords
		float w = 62;
		float h = 136;
		s_Verts[0] = CIwFVec3( w,  h, -s);
		s_Verts[1] = CIwFVec3(-w,  h, -s);
		s_Verts[2] = CIwFVec3(-w, -h, -s);
		s_Verts[3] = CIwFVec3( w, -h, -s);
		s_Verts[4] = CIwFVec3( w,  h,  s);
		s_Verts[5] = CIwFVec3(-w,  h,  s);
		s_Verts[6] = CIwFVec3(-w, -h,  s);
		s_Verts[7] = CIwFVec3( w, -h,  s);
	}

	IwGxSetVertStreamModelSpace(s_Verts, 8);

	// Set base color for lighting
	IwGxSetColStream(s_Cols, 8);
	
    IwGxSetUVStream(s_UVs);
	IwGxSetScreenSpaceSlot(1);
	IwGxDrawPrims(IW_GX_QUAD_STRIP, s_QuadStrip, 4);
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
