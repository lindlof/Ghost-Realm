/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraDefend.h"

#include "IwRandom.h"

bool dotCollides(CIwFVec2* rect, CIwFVec2 point);

static CIwFVec2 defend_uvs[4] =
{
    CIwFVec2(1, 1),
    CIwFVec2(1, 0),
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
};

CameraDefend::CameraDefend(Ghost* ghost) {
	this->ghost = ghost;

	defendingDotTexture = new CIwTexture;
	defendingDotTexture->LoadFromFile ("textures/defending_dot.png");
	defendingDotTexture->Upload();

	IwRandSeed((int32)s3eTimerGetMs());
	float width = (float)IwGxGetScreenWidth()*0.13f;
	width = width > 120 ? 120 : width;
	float height = width;
	float padding = (float)IwGxGetScreenWidth()*0.05f;

	float locPadding = (float)IwGxGetScreenHeight()*0.20f;
	{
		float leftSpotY = IwRandMinMax(locPadding, IwGxGetScreenHeight()-locPadding-height);
		defendVertsLeft[0] = CIwFVec2(padding, leftSpotY);
		defendVertsLeft[1] = CIwFVec2(padding, leftSpotY+height);
		defendVertsLeft[2] = CIwFVec2(padding+width, leftSpotY+height);
		defendVertsLeft[3] = CIwFVec2(padding+width, leftSpotY);
	}

	{
		float rightSpotY = IwRandMinMax(locPadding, IwGxGetScreenHeight()-locPadding-height);
		int screenW = IwGxGetScreenWidth();
		defendVertsRight[0] = CIwFVec2(screenW-padding-width, rightSpotY);
		defendVertsRight[1] = CIwFVec2(screenW-padding-width, rightSpotY+height);
		defendVertsRight[2] = CIwFVec2(screenW-padding, rightSpotY+height);
		defendVertsRight[3] = CIwFVec2(screenW-padding, rightSpotY);
	}

	drawing = false;

	over = false;
}

CameraDefend::~CameraDefend() {
	if (defendingDotTexture)
		delete defendingDotTexture;
}

void CameraDefend::Render() {
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();

    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
    pMat->SetTexture(defendingDotTexture);
    IwGxSetMaterial(pMat);
	IwGxSetUVStream(defend_uvs);

	IwGxSetVertStreamScreenSpace(defendVertsLeft, 4);
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

	IwGxSetVertStreamScreenSpace(defendVertsRight, 4);
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void CameraDefend::Update() {
	if (ghost->getAttack() == NULL || ghost->getAttack()->isOver()) {
		over = true;
	}
}

void CameraDefend::Touch(int32 x, int32 y) {
	if (isOver()) return;
	drawing = false;
}

void CameraDefend::Motion(int32 x, int32 y) {
	if (isOver()) return;

	if (!drawing) {
		drawStart = CIwFVec2(x, y);
		drawing = true;
	} else {
		CIwFVec2 drawEnd = CIwFVec2(x, y);

		bool dotsCollideWithDrawing = 
			(dotCollides(defendVertsLeft,  drawStart) && dotCollides(defendVertsRight, drawEnd)) ||
			(dotCollides(defendVertsRight, drawStart) && dotCollides(defendVertsLeft,  drawEnd));

		if (dotsCollideWithDrawing) {
			over = true;
			if (ghost->getAttack() != NULL)
				ghost->getAttack()->setDefended();

			IwTrace(GHOST_HUNTER, ("Player defend drawing %s; coords %.0f.%.0f to %.0f.%.0f - dot left coord %.0f.%.0f right coord %.0f.%.0f", 
				dotsCollideWithDrawing ? "accpeted" : "rejected",
				drawStart.x, drawStart.y, drawEnd.x, drawEnd.y, defendVertsLeft[0].x, defendVertsLeft[0].y,
				defendVertsRight[0].x, defendVertsRight[0].y));
		}
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

bool CameraDefend::isOver() {
	return over;
}
