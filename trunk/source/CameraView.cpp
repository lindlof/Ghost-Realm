/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GameState.h"

#include "CameraView.h"
#include "CameraModel.h"

#include "CameraUI.h"
#include "CameraDefend.h"
#include "PlayerHit.h"
#include "FightTutorialView.h"

#include "s3e.h"
#include "s3eCamera.h"

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"
#include "IwAnim.h"
#include "IwGxTransformSW.h"
#include "IwTextParserITX.h"

#include <sys/param.h>

#define BLEND_DURATION 0.25f

void setupPlayer();
void renderCamera();
void updateGhost();
void renderGhost();
void renderMana();
void renderGameOver();

void cameraStreamInit(int camDataW, int camDataH);
static CIwSVec2 cameraVert[4];
static CIwFVec2 cameraUvs[4];

static CIwFVec2 cameraUvsRotated[4];

uint16* g_pCameraTexelsRGB565 = NULL;
static CIwTexture* g_CameraTexture = NULL;

static CIwModel*       ghost_Model;
static CIwAnim*        ghost_Anims[1];
static CIwAnimSkel*    ghost_Skel;
static CIwAnimSkin*    ghost_Skin;
static CIwAnimPlayer*  ghost_Player;

static s3eCameraFrameRotation g_FrameRotation = S3E_CAMERA_FRAME_ROT90;
static CIwFMat viewMatrix;
static CIwFMat* ghostMatrix;
static GhostCollision* ghostCollision;
static CameraDefend* cameraDefend;
static PlayerHit* playerHit;
static FightTutorialView* tutorialView;

static int lostManaAnim = 0;

int clickX = 0, clickY = 0;

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

// In-place matrix transposition of 90 degrees to transform camera to portrait
void transpose(uint16 *tomatrix, uint16 *matrix, int row, int col) {
	for (int i = 0; i<row; i++) {
		for (int j = 0; j<col; j++) {
			tomatrix[j*row + i] = matrix[i*col + j];
		}
	}
}

// Camera callback. Copy the capture frame buffer into a texture ready for rendering.
static int32 frameReceived(void* systemData, void* userData)
{
    s3eCameraFrameData *data = (s3eCameraFrameData*)systemData;

    // If there is no texture, create one.
    // This is a slow operation compared to memcpy so we don't want to do it every frame.
    if (g_CameraTexture == NULL)
    {
		cameraStreamInit(data->m_Height, data->m_Width);

        g_CameraTexture = new CIwTexture();
        g_CameraTexture->SetMipMapping(false);
        g_CameraTexture->SetModifiable(true);

		uint cameraRGB565BufferSize = data->m_Width * data->m_Height * 2; //Size in bytes
        g_pCameraTexelsRGB565 = (uint16*) s3eMalloc(cameraRGB565BufferSize);

        g_CameraTexture->CopyFromBuffer(data->m_Height, data->m_Width, CIwImage::RGB_565, data->m_Height*2, (uint8*)g_pCameraTexelsRGB565, NULL);
        g_CameraTexture->Upload();
    }

	transpose(g_pCameraTexelsRGB565, (uint16*)data->m_Data, data->m_Height, data->m_Width);
    g_FrameRotation = data->m_Rotation;

    return 0;
}

void cameraStreamInit(int camDataW, int camDataH) {
	double w = IwGxGetScreenWidth();
	double h = IwGxGetScreenHeight();

	IwTrace(GHOST_HUNTER, ("camDataW %d camDataH %d w %f h %f", camDataW, camDataH, w, h));
	{
		double whRatio = ((double)camDataW) / camDataH;

		if (whRatio > w/h) {
			w = h * whRatio;
		} else if (whRatio < w/h) {
			h = w * 1.f/whRatio;
		}
	}

	IwTrace(GHOST_HUNTER, ("w %f h %f", w, h));

	int16 x1 = (int16)(-abs(w - IwGxGetScreenWidth())/2); // Negative or 0
	int16 x2 = (int16)(IwGxGetScreenWidth()  - x1);
	int16 y1 = (int16)(-abs(h - IwGxGetScreenHeight())/2); // Negative or 0
	int16 y2 = (int16)(IwGxGetScreenHeight() - y1);

	cameraVert[0].x = x1, cameraVert[0].y = y1;
	cameraVert[1].x = x1, cameraVert[1].y = y2;
	cameraVert[2].x = x2, cameraVert[2].y = y2;
	cameraVert[3].x = x2, cameraVert[3].y = y1;

	cameraUvs[0] = CIwFVec2(1, 0);
    cameraUvs[1] = CIwFVec2(1, 1);
    cameraUvs[2] = CIwFVec2(0, 1);
    cameraUvs[3] = CIwFVec2(0, 0);
}
void CameraViewPreInit()
{
	playerHit = new PlayerHit(getGameState()->getPlayer());
	tutorialView = new FightTutorialView();
}
void CameraViewInit()
{
    // Camera field of view
	IwGxSetPerspMul((float) IwGxGetScreenWidth() / 1.3);
    IwGxSetFarZNearZ(0x2000,0x10);

	// Initiate collision builder before loading models
	IW_CLASS_REGISTER(GhostCollision);

#ifdef IW_BUILD_RESOURCES
	IwGetModelBuilder()->SetPostBuildFn(&BuildCollision);
#endif

	IwGetResManager()->LoadGroup("viking/viking_v4.group");
	CIwResGroup* pGroup = IwGetResManager()->GetGroupNamed("viking_v4");

    ghost_Model = (CIwModel*)pGroup->GetResNamed("viking", IW_GRAPHICS_RESTYPE_MODEL);
	ghostCollision = (GhostCollision*)pGroup->GetResNamed("viking", "GhostCollision");
    ghost_Skin  = (CIwAnimSkin*)pGroup->GetResNamed("viking", IW_ANIM_RESTYPE_SKIN);
    ghost_Skel  = (CIwAnimSkel*)pGroup->GetResNamed("Armature", IW_ANIM_RESTYPE_SKELETON);
    ghost_Anims[0]  = (CIwAnim*)pGroup->GetResNamed("Armature", IW_ANIM_RESTYPE_ANIMATION);
	
	ghostMatrix = new CIwFMat();
	ghostCollision->init(ghostMatrix);
	
    // Create animation player
    ghost_Player = new CIwAnimPlayer;
    ghost_Player->SetSkel(ghost_Skel);
    ghost_Player->PlayAnim(ghost_Anims[0], 1, CIwAnimBlendSource::LOOPING_F, BLEND_DURATION);
	updateGhost();

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

	CameraUIInit();

	cameraDefend = NULL;
	tutorialView->setTutorial(getFightTutorial());
}

//-----------------------------------------------------------------------------
// The following function deletes textures if they exist and terminates
// IwGx and s3e API functionality.
//-----------------------------------------------------------------------------
void CameraViewTerm()
{
	if (g_pCameraTexelsRGB565)
		delete g_pCameraTexelsRGB565;

    if (g_CameraTexture)
        delete g_CameraTexture;

	delete ghost_Player;

	delete ghostMatrix;
	delete ghostCollision;

    if (s3eCameraAvailable())
    {
        s3eCameraStop();
        s3eCameraUnRegister(S3E_CAMERA_UPDATE_STREAMING, frameReceived);
    }

	if (playerHit)
		delete playerHit;

	if (tutorialView)
		delete tutorialView;

	CameraUITerm();
}

bool CameraViewUpdate()
{
	bool ghostAvailable = getGameState()->getGhost() != NULL;

	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	if (!gameIsHalt() && ghostAvailable) {
		updateGhost();
	}

	renderCamera();
	setupPlayer();
	if (ghostAvailable)
		renderGhost();
	playerHit->Render();
	renderMana();

	if (ghostAvailable) {
		GhostAttack* ghostAttack = getGameState()->getGhost()->getAttack();
		if (cameraDefend != NULL) {
			if (cameraDefend->isOver()) {
				delete cameraDefend;
				cameraDefend = NULL;
			} else {
				cameraDefend->Update();
				cameraDefend->Render();
			}
		} else if (ghostAttack != NULL && 
				getGameState()->getGhost()->isAttackDefendable()) {
			cameraDefend = new CameraDefend(getGameState()->getGhost());
		}
	}

	if (getGameState()->getPlayer()->isDead()) {
		showGameOverButton(true);
		renderGameOver();
	} else {
		showGameOverButton(false);
	}

	tutorialView->Render();

	// Draw background, then clear depth buffer so we can render UI over the top.
    IwGxFlush();
	IwGxClear(IW_GX_DEPTH_BUFFER_F);

	CameraUIUpdate();
	CameraUIRender();

	IwGxFlush();
    IwGxSwapBuffers();

	return true;
}

void renderCamera() {

	IwGxLightingOff();

    // Refresh dynamic texture
    if (g_CameraTexture != NULL)
        g_CameraTexture->ChangeTexels((uint8*)g_pCameraTexelsRGB565, CIwImage::RGB_565);

    CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    pMat->SetTexture(g_CameraTexture);

    IwGxSetMaterial(pMat);

	IwGxSetScreenSpaceSlot(-1);

    IwGxSetUVStream(cameraUvs);
    IwGxSetVertStreamScreenSpace(cameraVert, 4);

    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void setupPlayer() {
	Player *player = getGameState()->getPlayer();

	if (!player->isReady()) {
		return;
	}

    // Set viewing position with the Y axis as upright.
    viewMatrix.SetIdentity();
    viewMatrix.LookAt(CIwFVec3(0,0,0), CIwFVec3(0,0,100), CIwFVec3(0,-1,0));

    // Rotate according to compass heading
	viewMatrix.PostRotateY(rad(player->getHeading()));
    IwGxSetViewMatrix(&viewMatrix);
}

void updateGhost() {
	// Update animation player
    ghost_Player->Update(1.0f / 30.0f);

    // Update IwGx state time stamp
    IwGxTickUpdate();
}

void renderGhost() {

	if (!getGameState()->getPlayer()->isReady()) {
		return;
	}

	IwGxLightingOff();

	Ghost *ghost = getGameState()->getGhost();

	CIwFVec3 ghostPosition(0, 0, ghost->getDistance());

    // Place the markers on the edge of the compass radius
    // rotated to their correct bearing to current location
	ghostMatrix->SetRotY(rad(ghost->getBearing()));
	ghostMatrix->SetTrans(ghostMatrix->RotateVec(ghostPosition));
	ghostMatrix->PostRotateY(rad(ghost->getRotation()));

    IwGxSetModelMatrix(ghostMatrix);

	
	IwGxLightingAmbient(true);
	IwGxSetLightType(0, IW_GX_LIGHT_AMBIENT);
	
	int sinceHit = clock() - ghost->getHitTime();
	CIwColour colAmbient;

	if (sinceHit < GHOST_HIT_LENGTH) {
		int halfAnimation = GHOST_HIT_LENGTH/2;

		float animState = (float) sinceHit / halfAnimation;
		if (animState > 1) {
			animState = 2 - animState;
		}

		colAmbient.Set(0xff, 0xff*(1-animState), 0xff*(1-animState), 0xff);
	} else {
		// The default state that displays the image as it is
		colAmbient.Set(0xff, 0xff, 0xff, 0xff);
	}
	
	IwGxSetLightCol(0, &colAmbient);

	IwGxSetScreenSpaceSlot(1);
    IwAnimSetSkelContext(ghost_Player->GetSkel());
    IwAnimSetSkinContext(ghost_Skin);
    ghost_Model->Render();

	//outline the face under the cursor
	if (clickX > 0 && clickY > 0) {
		int32 faceID = ghostCollision->GetFaceUnderCursor(clickX, clickY);
		if (faceID != -1)
		{
			getGameState()->getGhost()->tapped();
			clickX = clickY = -1;
		} else {
			clickX = clickY = -1;
		}
	}

    // Tidier to reset these
    IwAnimSetSkelContext(NULL);
    IwAnimSetSkinContext(NULL);

	if (getGameState()->getGhost()->isFound()) {
		ghostCollision->RenderEctoplasmaBar((float)ghost->getEctoplasm() / GHOST_MAX_ECTOPLASM);
	}
}

void renderMana() {
	
	IwGxLightingOff();

	Player *player = getGameState()->getPlayer();

	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
	pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	IwGxSetMaterial(pMat);

	// Vertex coords for full mana
	int16 x1 = ((double)IwGxGetScreenWidth()/100) * 5;
    int16 x2 = ((double)IwGxGetScreenWidth()/100) * 95;
    int16 y1 = ((double)IwGxGetScreenHeight()/100) * 1;
    int16 y2 = ((double)IwGxGetScreenHeight()/100) * 3;

	// Full length of the bar
	int16 barLength = x2 - x1;
	// Multiply the full bar length with current mana status
	barLength =  barLength * (float)player->getMana() / PLAYER_MAX_MANA;

	x2 = x1 + barLength;

	CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);

	int sinceHit = clock() - player->getHitTime();

	if (sinceHit < PLAYER_HIT_LENGTH) {
		int animation = PLAYER_HIT_LENGTH / PLAYER_HIT_FLASHES;
		int halfAnimation = animation / 2;

		float animState = ((float)(sinceHit % animation) / halfAnimation);

		if (animState > 1) {
			animState = 2 - animState;
		}

		uint8 r = 0xFF*animState + 0x00*(1-animState);
		uint8 g = 0xFF*animState + 0x80*(1-animState);
		uint8 b = 0xFF*animState + 0xFF*(1-animState);
		uint8 a = 0xFF*animState + 0x70*(1-animState);
		cols->Set(   r,    g,    b,    a);
	} else {
		// The default state that displays the image as it is
		cols->Set(0x00, 0x80, 0xFF, 0x70);
	}

	cols[1] = cols[2] = cols[3] = cols[0];

	CIwSVec2 XY(x1, y1), dXY(x2-x1, y2-y1);
	IwGxDrawRectScreenSpace(&XY, &dXY, cols);
}

void renderGameOver() {
	IwGxLightingOff();


	CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
	cols[0].Set(0xff, 0x66, 0x66, 0x40);
	cols[1] = cols[2] = cols[3] = cols[0];

	CIwSVec2 XY(0, 0), dXY(IwGxGetScreenWidth(), IwGxGetScreenHeight());
	IwGxDrawRectScreenSpace(&XY, &dXY, cols);
}

void ghostTouch(int32 x, int32 y) {
	clickX = x;
	clickY = y;
}

void getCameraDefend(CameraDefend** cameraDefendRet) {
	*cameraDefendRet = cameraDefend;
}
