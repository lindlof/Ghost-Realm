/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraDefend.h"
#include "CameraModel.h"
#include "GameState.h"

#include "IwRandom.h"

bool dotCollides(CIwFVec2* rect, CIwFVec2 point);

static int cells = 4;
static int rows = 4;
static CIwFVec2 defend_uvs[4] =
{
    CIwFVec2(0,         0),
    CIwFVec2(0,         1.f/rows),
    CIwFVec2(1.f/cells, 1.f/rows),
    CIwFVec2(1.f/cells, 0),
};

CameraDefend::CameraDefend() {
	defendingDotTexture = new CIwTexture;
	defendingDotTexture->LoadFromFile ("textures/defending_dot.png");
	defendingDotTexture->Upload();

	pMat = new CIwMaterial;
	pMat->SetTexture(defendingDotTexture);

	pMat->CreateAnim();
	pMat->SetAnimCelW(defendingDotTexture->GetWidth()/cells);
	pMat->SetAnimCelH(defendingDotTexture->GetHeight()/rows);
    pMat->SetAnimCelPeriod(1);

	IwRandSeed((int32)s3eTimerGetMs());
	float width = (float)IwGxGetScreenWidth()*0.13f;
	width = width > 120 ? 120 : width;
	float height = width;
	float padding = (float)IwGxGetScreenWidth()*0.05f;

	int mid = IwGxGetScreenHeight()/2;
	int heightSqrt = sqrt(IwGxGetScreenHeight()/2);
	bool leftAboveMid;

	float locPadding = (float)IwGxGetScreenHeight()*0.10f;
	{
		// The dot prefers corners
		int positionFromMid = IwRandMinMax(0, heightSqrt) * IwRandMinMax(0, heightSqrt);
		positionFromMid = IwGxGetScreenHeight()/2 - positionFromMid;
		leftAboveMid = IwRandMinMax(0, 9) < 5;
		int leftSpotY = leftAboveMid ? 
			mid - positionFromMid + locPadding :
			mid + positionFromMid - locPadding - height;

		defendVertsLeft[0] = CIwFVec2(padding, leftSpotY);
		defendVertsLeft[1] = CIwFVec2(padding, leftSpotY+height);
		defendVertsLeft[2] = CIwFVec2(padding+width, leftSpotY+height);
		defendVertsLeft[3] = CIwFVec2(padding+width, leftSpotY);
	}

	{
		int positionFromMid = IwRandMinMax(0, heightSqrt) * IwRandMinMax(0, heightSqrt);
		positionFromMid = IwGxGetScreenHeight()/2 - positionFromMid;
		// The dot prefers the opposite corner
		bool rightAboveMid = leftAboveMid ? IwRandMinMax(0, 9) < 2 : IwRandMinMax(0, 9) >= 2;
		int rightSpotY = rightAboveMid ? 
			mid - positionFromMid + locPadding :
			mid + positionFromMid - locPadding - height;

		int screenW = IwGxGetScreenWidth();
		defendVertsRight[0] = CIwFVec2(screenW-padding-width, rightSpotY);
		defendVertsRight[1] = CIwFVec2(screenW-padding-width, rightSpotY+height);
		defendVertsRight[2] = CIwFVec2(screenW-padding, rightSpotY+height);
		defendVertsRight[3] = CIwFVec2(screenW-padding, rightSpotY);
	}

	drawing = false;
	active = false;
}

CameraDefend::~CameraDefend() {
	if (pMat)
		delete pMat;

	if (defendingDotTexture)
		delete defendingDotTexture;
}

void CameraDefend::Render() {
	if (!isActive()) return;

	pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	if (!drawing) pMat->SetAnimCelID(0);

	IwGxSetMaterial(pMat);
	IwGxSetUVStream(defend_uvs);

	IwGxSetVertStreamScreenSpace(defendVertsLeft, 4);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

	IwGxSetVertStreamScreenSpace(defendVertsRight, 4);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void CameraDefend::Update() {
	Ghost* ghost = getGameState()->getGhost();

	if (ghost != NULL && ghost->isAttackDefendable() && !(ghost->getAttack() == NULL || ghost->getAttack()->isOver())) {
		active = true;
		getFightTutorial()->triggerTutorial(TUTORIAL_DEFEND);
	} else {
		active = false;
	}
}

void CameraDefend::Touch(int32 x, int32 y, bool press) {
	if (!isActive()) return;

	if (press) {
		drawStart = CIwFVec2(x, y);
		drawing = true;
	} else {
		Motion(x, y);
		drawing = false;
	}
}

void CameraDefend::Motion(int32 x, int32 y) {
	if (!isActive()) return;
	Ghost* ghost = getGameState()->getGhost();

	CIwFVec2 drawEnd = CIwFVec2(x, y);

	bool dotsCollideWithDrawing = 
		(dotCollides(defendVertsLeft,  drawStart) && dotCollides(defendVertsRight, drawEnd)) ||
		(dotCollides(defendVertsRight, drawStart) && dotCollides(defendVertsLeft,  drawEnd));

	if (dotsCollideWithDrawing) {
		active = false;
		if (ghost->getAttack() != NULL)
			ghost->getAttack()->setDefended();

		IwTrace(GHOST_HUNTER, ("Player defend drawing %s; coords %.0f.%.0f to %.0f.%.0f - dot left coord %.0f.%.0f right coord %.0f.%.0f", 
			dotsCollideWithDrawing ? "accpeted" : "rejected",
			drawStart.x, drawStart.y, drawEnd.x, drawEnd.y, defendVertsLeft[0].x, defendVertsLeft[0].y,
			defendVertsRight[0].x, defendVertsRight[0].y));
	}
}

bool dotCollides(CIwFVec2* rect, CIwFVec2 point) {
	float collisionModifier = 0.2f;
	float leftPoint  = rect[0].x*(1-collisionModifier);
	float rightPoint = rect[3].x*(1+collisionModifier);
	float upPoint    = rect[0].y*(1-collisionModifier);
	float downPoint  = rect[1].y*(1+collisionModifier);

	if (point.x > leftPoint && point.x < rightPoint &&
		point.y > upPoint   && point.y < downPoint)
		return true;
	return false;
}

bool CameraDefend::isActive() {
	return active;
}
