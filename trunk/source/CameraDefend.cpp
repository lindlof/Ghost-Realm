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

bool dotCollides(CIwFVec2* rectTopLeft, CIwFVec2* rectSize, CIwFVec2 point);

static CIwFVec2 dot_uvs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
    CIwFVec2(1, 1),
    CIwFVec2(1, 0),
};

static int cells = 3;
static int rows = 3;
static CIwFVec2 anim_uvs[4] =
{
    CIwFVec2(0,         0),
    CIwFVec2(0,         1.f/rows),
    CIwFVec2(1.f/cells, 1.f/rows),
    CIwFVec2(1.f/cells, 0),
};

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

CameraDefend::CameraDefend() {
	dotTexture = Iw2DCreateImage("textures/defending_dot.png");
	dotAngle = 0;

	animTexture = new CIwTexture;
	animTexture->LoadFromFile("textures/defending_anim.png");
	animTexture->Upload();

	animMat = new CIwMaterial;
	animMat->SetTexture(animTexture);

	animMat->CreateAnim();
	animMat->SetAnimCelW((double)animTexture->GetWidth()/cells);
	animMat->SetAnimCelH((double)animTexture->GetHeight()/rows);
    animMat->SetAnimCelPeriod(4);

	IwRandSeed((int32)s3eTimerGetMs());
	float width = (float)IwGxGetScreenWidth()*0.15f;
	//width = width > 120 ? 120 : width;
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

		dotVertsTopLeftLeft = CIwFVec2(padding, leftSpotY);
		dotVertsSizeLeft = CIwFVec2(width, height);

		animVertsLeft[0] = CIwFVec2(dotVertsTopLeftLeft.x - width, dotVertsTopLeftLeft.y - height);
		animVertsLeft[1] = CIwFVec2(dotVertsTopLeftLeft.x - width, dotVertsTopLeftLeft.y + height*2);
		animVertsLeft[2] = CIwFVec2(dotVertsTopLeftLeft.x + width*2, dotVertsTopLeftLeft.y + height*2);
		animVertsLeft[3] = CIwFVec2(dotVertsTopLeftLeft.x + width*2, dotVertsTopLeftLeft.y - height);
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
		dotVertsTopLeftRight = CIwFVec2(screenW-padding-width, rightSpotY);
		dotVertsSizeRight = CIwFVec2(width, height);

		animVertsRight[0] = CIwFVec2(dotVertsTopLeftRight.x - width, dotVertsTopLeftRight.y - height);
		animVertsRight[1] = CIwFVec2(dotVertsTopLeftRight.x - width, dotVertsTopLeftRight.y + height*2);
		animVertsRight[2] = CIwFVec2(dotVertsTopLeftRight.x + width*2, dotVertsTopLeftRight.y + height*2);
		animVertsRight[3] = CIwFVec2(dotVertsTopLeftRight.x + width*2, dotVertsTopLeftRight.y - height);
	}

	drawing = false;
	active = false;
}

CameraDefend::~CameraDefend() {
	if (animMat)
		delete animMat;

	if (dotTexture)
		delete dotTexture;

	if (animTexture)
		delete animTexture;
}

void CameraDefend::Render() {
	if (!isActive()) return;

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	CIwFMat2D rot;
	rot.SetRot(rad(dotAngle), CIwFVec2(
		dotVertsTopLeftLeft.x + dotVertsSizeLeft.x/2, 
		dotVertsTopLeftLeft.y + dotVertsSizeLeft.y/2));
	Iw2DSetTransformMatrix(rot);
	Iw2DDrawImage(dotTexture, dotVertsTopLeftLeft, dotVertsSizeLeft);

	rot.SetRot(rad(dotAngle), CIwFVec2(
		dotVertsTopLeftRight.x + dotVertsSizeRight.x/2, 
		dotVertsTopLeftRight.y + dotVertsSizeRight.y/2));
	Iw2DSetTransformMatrix(rot);
	Iw2DDrawImage(dotTexture, dotVertsTopLeftRight, dotVertsSizeRight);

	if (!drawing) {
		animMat->SetAnimCelID(0);
		return;
	}

	IwGxLightingOff();

	animMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	animMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	IwGxSetMaterial(animMat);
	IwGxSetUVStream(anim_uvs);

	IwGxSetVertStreamScreenSpace(animVertsLeft, 4);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

	IwGxSetVertStreamScreenSpace(animVertsRight, 4);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void CameraDefend::Update() {
	dotAngle += 5;
	if (dotAngle > 359)
		dotAngle = 0;

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
		(dotCollides(&dotVertsTopLeftLeft, &dotVertsSizeLeft,  drawStart) && dotCollides(&dotVertsTopLeftRight, &dotVertsSizeRight, drawEnd)) ||
		(dotCollides(&dotVertsTopLeftRight, &dotVertsSizeRight, drawStart) && dotCollides(&dotVertsTopLeftLeft, &dotVertsSizeLeft,  drawEnd));

	if (dotsCollideWithDrawing) {
		active = false;
		if (ghost->getAttack() != NULL)
			ghost->getAttack()->setDefended();

		/*IwTrace(GHOST_HUNTER, ("Player defend drawing %s; coords %.0f.%.0f to %.0f.%.0f - dot left coord %.0f.%.0f right coord %.0f.%.0f", 
			dotsCollideWithDrawing ? "accpeted" : "rejected",
			drawStart.x, drawStart.y, drawEnd.x, drawEnd.y, defendVertsLeft[0].x, defendVertsLeft[0].y,
			defendVertsRight[0].x, defendVertsRight[0].y));*/
	}
}

bool dotCollides(CIwFVec2* rectTopLeft, CIwFVec2* rectSize, CIwFVec2 point) {
	float collisionModifier = 0.3f;
	float leftPoint  = rectTopLeft->x*(1-collisionModifier);
	float rightPoint = (rectTopLeft->x + rectSize->x)*(1+collisionModifier);
	float upPoint    = rectTopLeft->y*(1-collisionModifier);
	float downPoint  = (rectTopLeft->y + rectSize->y)*(1+collisionModifier);

	if (point.x > leftPoint && point.x < rightPoint &&
		point.y > upPoint   && point.y < downPoint)
		return true;
	return false;
}

bool CameraDefend::isActive() {
	return active;
}
