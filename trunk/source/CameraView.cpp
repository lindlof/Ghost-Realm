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
#include "GhostType.h"

#include "CameraDefend.h"
#include "PlayerHit.h"
#include "FightTutorialView.h"
#include "ManaBar.h"
#include "PlayerAttackView.h"

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

void LoadViking();
void LoadSkelman();
void DeleteGhost();

void initFightView();

void setupPlayer();
void renderCamera();
void updateGhost();
void renderGhost();

void cameraStreamInit(int camDataW, int camDataH);
static CIwSVec2 cameraVert[4];
static CIwFVec2 cameraUvs[4];

static CIwFVec2 cameraUvsRotated[4];

uint16* g_pCameraTexelsRGB565 = NULL;
static CIwTexture* g_CameraTexture = NULL;

enum GhostAnimation {GHOST_ANIM_IDLE, GHOST_ANIM_AGRO, GHOST_ANIM_DODGE, 
	GHOST_ANIM_ATTACK, GHOST_ANIM_CAPTURED};

static CIwModel*       ghost_Model;
static GhostCollision* ghostCollision;
static CIwAnim*        ghost_Anims[5];
static CIwAnimSkel*    ghost_Skel;
static CIwAnimSkin*    ghost_Skin;
static CIwAnimPlayer*  ghost_Player;

static CIwModel*       ghostStore_Model[2];
static GhostCollision* ghostStoreCollision[2];
static CIwAnim*        ghostStore_Anims[2][5];
static CIwAnimSkel*    ghostStore_Skel[2];
static CIwAnimSkin*    ghostStore_Skin[2];

static s3eCameraFrameRotation g_FrameRotation = S3E_CAMERA_FRAME_ROT90;
static CIwFMat viewMatrix;
static CIwFMat* ghostMatrix;
static CameraDefend* cameraDefend;
static PlayerHit* playerHit;
static FightTutorialView* tutorialView;
static ManaBar* manaBar;
static PlayerAttackView* playerAttackView;

static int lostManaAnim = 0;

int clickX = 0, clickY = 0;

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

void initFightView() {
	GhostType ghostType = getGameState()->getGhost()->getGhostType();

	if (ghostType == GhostType::VIKING) {
		ghost_Model = ghostStore_Model[0];
		ghostCollision = ghostStoreCollision[0];
		for (int i = 0; i < 5; i++) {
			ghost_Anims[i] = ghostStore_Anims[0][i];
		}
		ghost_Skel = ghostStore_Skel[0];
		ghost_Skin = ghostStore_Skin[0];
	} else if (ghostType == GhostType::SKELMAN) {
		ghost_Model = ghostStore_Model[1];
		ghostCollision = ghostStoreCollision[1];
		for (int i = 0; i < 5; i++) {
			ghost_Anims[i] = ghostStore_Anims[1][i];
		}
		ghost_Skel = ghostStore_Skel[1];
		ghost_Skin = ghostStore_Skin[1];
	}

	if (ghostMatrix)
		delete ghostMatrix;
	if (ghost_Player)
		delete ghost_Player;
	
	ghostMatrix = new CIwFMat();
	ghostCollision->init(ghostMatrix, ghostType);
	
    ghost_Player = new CIwAnimPlayer;
    ghost_Player->SetSkel(ghost_Skel);

	// Reset animation
	ghost_Player->PlayAnim(ghost_Anims[GHOST_ANIM_IDLE], 1, CIwAnimBlendSource::LOOPING_F, 0);
	updateGhost();
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
	// For some reason loading images in early stage of init solves lagginess/buggyness
	playerHit = new PlayerHit(getGameState()->getPlayer());
	tutorialView = new FightTutorialView();
	cameraDefend = new CameraDefend();
	manaBar = new ManaBar();
	playerAttackView = new PlayerAttackView(getGameState()->getPlayer());
}
void CameraViewInit()
{
    // Camera field of view
	IwGxSetPerspMul((float) IwGxGetScreenWidth() / 1.3);
    IwGxSetFarZNearZ(0x2000,0x10);

	ghost_Model = NULL;
	ghostCollision = NULL;
	ghost_Skin = NULL;
	ghost_Skel = NULL;
	for (int i = 0; i < 5; i++) {
		ghost_Anims[i] = NULL;
	}
	ghostMatrix = NULL;
	ghost_Player = NULL;

	// Initiate collision builder before loading models
	IW_CLASS_REGISTER(GhostCollision);

#ifdef IW_BUILD_RESOURCES
	IwGetModelBuilder()->SetPostBuildFn(&BuildCollision);
#endif

	LoadSkelman();
	LoadViking();

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

	DeleteGhost();
	IW_CLASS_REMOVE(GhostCollision);

    if (s3eCameraAvailable())
    {
        s3eCameraStop();
        s3eCameraUnRegister(S3E_CAMERA_UPDATE_STREAMING, frameReceived);
    }

	if (playerHit)
		delete playerHit;

	if (tutorialView)
		delete tutorialView;

	if (cameraDefend)
		delete cameraDefend;

	if (manaBar)
		delete manaBar;

	if (playerAttackView)
		delete playerAttackView;
}

void LoadViking() {
	IwGetResManager()->LoadGroup("viking/viking.group");
	CIwResGroup* pGroup = IwGetResManager()->GetGroupNamed("viking");

	ghostStore_Model[0] = (CIwModel*)pGroup->GetResNamed("viking", IW_GRAPHICS_RESTYPE_MODEL);
	ghostStoreCollision[0] = (GhostCollision*)pGroup->GetResNamed("viking", "GhostCollision");
	ghostStore_Skin[0]  = (CIwAnimSkin*)pGroup->GetResNamed("viking", IW_ANIM_RESTYPE_SKIN);
	ghostStore_Skel[0]  = (CIwAnimSkel*)pGroup->GetResNamed("Armature", IW_ANIM_RESTYPE_SKELETON);
	ghostStore_Anims[0][GHOST_ANIM_IDLE]  = (CIwAnim*)pGroup->GetResNamed("Armature_idle", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[0][GHOST_ANIM_AGRO]  = (CIwAnim*)pGroup->GetResNamed("Armature_agro", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[0][GHOST_ANIM_DODGE]  = (CIwAnim*)pGroup->GetResNamed("Armature_dodge", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[0][GHOST_ANIM_ATTACK]  = (CIwAnim*)pGroup->GetResNamed("Armature_attack", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[0][GHOST_ANIM_CAPTURED]  = (CIwAnim*)pGroup->GetResNamed("Armature_captured", IW_ANIM_RESTYPE_ANIMATION);
}

void LoadSkelman() {
	IwGetResManager()->LoadGroup("Skelman/Skelman.group");
	CIwResGroup* pGroup = IwGetResManager()->GetGroupNamed("Skelman");

	ghostStore_Model[1] = (CIwModel*)pGroup->GetResNamed("GEO_For_Export", IW_GRAPHICS_RESTYPE_MODEL);
	ghostStoreCollision[1] = (GhostCollision*)pGroup->GetResNamed("GEO_For_Export", "GhostCollision");
	ghostStore_Skin[1]  = (CIwAnimSkin*)pGroup->GetResNamed("GEO_For_Export", IW_ANIM_RESTYPE_SKIN);
	ghostStore_Skel[1]  = (CIwAnimSkel*)pGroup->GetResNamed("HIP01", IW_ANIM_RESTYPE_SKELETON);
	ghostStore_Anims[1][GHOST_ANIM_IDLE]  = (CIwAnim*)pGroup->GetResNamed("Skelman_Idle", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[1][GHOST_ANIM_AGRO]  = (CIwAnim*)pGroup->GetResNamed("Skelman_Idle", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[1][GHOST_ANIM_DODGE]  = (CIwAnim*)pGroup->GetResNamed("Skelman_Idle", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[1][GHOST_ANIM_ATTACK]  = (CIwAnim*)pGroup->GetResNamed("Skelman_Attack", IW_ANIM_RESTYPE_ANIMATION);
	ghostStore_Anims[1][GHOST_ANIM_CAPTURED]  = (CIwAnim*)pGroup->GetResNamed("Skelman_Death", IW_ANIM_RESTYPE_ANIMATION);
}

void DeleteGhost() {
	if (ghost_Model)
		delete ghost_Model;
	if (ghostCollision)
		delete ghostCollision;
	if (ghost_Skin)
		delete ghost_Skin;
	if (ghost_Skel)
		delete ghost_Skel;
	for (int i = 0; i < 5; i++) {
		if (ghost_Anims[i])
			delete ghost_Anims[i];
	}
}

bool CameraViewUpdate()
{
	if (viewFightInitRequired())
		initFightView();

	bool ghostAvailable = getGameState()->getGhost() != NULL;

	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	if (ghostAvailable && 
		(!gameIsHalt() || getGameState()->getGhost()->isDead())) {
		updateGhost();
	}

	renderCamera();
	setupPlayer();
	if (ghostAvailable)
		renderGhost();
	playerAttackView->Render();

	playerHit->Render();

	manaBar->Render();
	cameraDefend->Update();
	cameraDefend->Render();

	tutorialView->Render();

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
	if (ghost_Player->IsCurrentAnimComplete() && !getGameState()->getGhost()->isDead()) {
		ghost_Player->PlayAnim(ghost_Anims[GHOST_ANIM_IDLE], 1, CIwAnimBlendSource::LOOPING_F, BLEND_DURATION);
	}

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
	double ghostRotation = rad(ghost->getRotation());

    // Place the markers on the edge of the compass radius
    // rotated to their correct bearing to current location
	ghostMatrix->SetRotY(rad(ghost->getBearing()));
	ghostMatrix->SetTrans(ghostMatrix->RotateVec(ghostPosition));
	ghostMatrix->PostRotateY(ghostRotation);

    IwGxSetModelMatrix(ghostMatrix);

	if (ghost->pollAnimCaptured()) {
		ghost_Player->PlayAnim(ghost_Anims[GHOST_ANIM_CAPTURED], 1, 0, BLEND_DURATION);
	} else if (ghost->pollAnimAgro()) {
		ghost_Player->PlayAnim(ghost_Anims[GHOST_ANIM_AGRO], 1, 0, BLEND_DURATION);
	} else if (ghost->pollAnimDodge()) {
		ghost_Player->PlayAnim(ghost_Anims[GHOST_ANIM_DODGE], 1, 0, BLEND_DURATION);
	} else if (ghost->pollAnimAttack()) {
		ghost_Player->PlayAnim(ghost_Anims[GHOST_ANIM_ATTACK], 1, 0, BLEND_DURATION);
	}
	
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

	if (ghost->isFound() && !ghost->isDead()) {
		ghostCollision->RenderEctoplasmaBar((float)ghost->getEctoplasm() / GHOST_MAX_ECTOPLASM, ghostRotation);
	}
}

void ghostTouch(int32 x, int32 y) {
	clickX = x;
	clickY = y;
}

CameraDefend* getCameraDefend() {
	return cameraDefend;
}

FightTutorialView* getFightTutorialView() {
	return tutorialView;
}
