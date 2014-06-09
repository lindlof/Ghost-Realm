/* ========================================================================= *
 * Ghost Hunter Project                                                      *
 * https://ghosthuntermobile.wordpress.com/                                  *
 * ========================================================================= *
 * Copyright (C) 2013 Ghost Hunter Project                                   *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 * ========================================================================= */

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

static int cells = 4;
static int rows = 2;
static CIwFVec2 anim_uvs[4] =
{
    CIwFVec2(0,         0),
    CIwFVec2(0,         1.f/rows),
    CIwFVec2(1.f/cells, 1.f/rows),
    CIwFVec2(1.f/cells, 0),
};

static int cellsSuccess = 4;
static int rowsSuccess = 2;
static CIwFVec2 anim_uvs_success[4] =
{
    CIwFVec2(0,                0),
    CIwFVec2(0,                1.f/rowsSuccess),
    CIwFVec2(1.f/cellsSuccess, 1.f/rowsSuccess),
    CIwFVec2(1.f/cellsSuccess, 0),
};

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

CameraDefend::CameraDefend() {
	IwRandSeed((int32)s3eTimerGetMs());

	for (int i = 0; i < DEFEND_TOUCHES_MAX; i++) {
		touch[i] = new DefendTouch;
		touch[i]->drawing = false;
	}

	dotTextureGreen = Iw2DCreateImage("textures/defending_dot/defending_dot.png");
	dotTextureRed   = Iw2DCreateImage("textures/defending_dot/defending_dot_red.png");
	dotTexture = dotTextureGreen;
	dotAngle = 0;

	animTexturePress = new CIwTexture;
	animTexturePress->LoadFromFile("textures/defending_dot/defending_anim.png");
	animTexturePress->Upload();

	animMat = new CIwMaterial;
	animMat->SetTexture(animTexturePress);

	animMat->CreateAnim();
	animMat->SetAnimCelW((double)animTexturePress->GetWidth()/cells);
	animMat->SetAnimCelH((double)animTexturePress->GetHeight()/rows);
    animMat->SetAnimCelPeriod(2);

	animTextureSuccess = new CIwTexture;
	animTextureSuccess->LoadFromFile("textures/defending_dot/defending_success.png");
	animTextureSuccess->Upload();

	animMatSuccess = new CIwMaterial;
	animMatSuccess->SetTexture(animTextureSuccess);

	animMatSuccess->CreateAnim();
	animMatSuccess->SetAnimCelW((double)animTexturePress->GetWidth()/cellsSuccess);
	animMatSuccess->SetAnimCelH((double)animTexturePress->GetHeight()/rowsSuccess);
    animMatSuccess->SetAnimCelPeriod(2);
	
	reinit();
	active = false;
	defended = false;
}

void CameraDefend::reinit() {
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

		animVertsLeftSuccess[0] = animVertsLeft[0];
		animVertsLeftSuccess[1] = animVertsLeft[1];
		animVertsLeftSuccess[2] = animVertsLeft[2];
		animVertsLeftSuccess[3] = animVertsLeft[3];
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

		animVertsRightSuccess[0] = animVertsRight[0];
		animVertsRightSuccess[1] = animVertsRight[1];
		animVertsRightSuccess[2] = animVertsRight[2];
		animVertsRightSuccess[3] = animVertsRight[3];
	}

	for (int i = 0; i < DEFEND_TOUCHES_MAX; i++) {
		touch[i]->drawing = false;
	}
}

CameraDefend::~CameraDefend() {
	if (animMat)
		delete animMat;

	if (animMatSuccess)
		delete animMatSuccess;

	if (dotTextureGreen)
		delete dotTextureGreen;

	if (dotTextureRed)
		delete dotTextureRed;

	if (animTexturePress)
		delete animTexturePress;

	if (animTextureSuccess)
		delete animTextureSuccess;

	for (int i = 0; i < DEFEND_TOUCHES_MAX; i++) {
		if (touch[i])
			delete touch[i];
	}
}

void CameraDefend::Render() {
	if (defended) {
		IwGxLightingOff();

		animMatSuccess->SetModulateMode(CIwMaterial::MODULATE_NONE);
		animMatSuccess->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

		IwGxSetMaterial(animMatSuccess);
		IwGxSetUVStream(anim_uvs_success);

		IwGxSetVertStreamScreenSpace(animVertsLeftSuccess, 4);
		IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

		IwGxSetVertStreamScreenSpace(animVertsRightSuccess, 4);
		IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

		if (animMatSuccess->GetAnimCelID() == animMatSuccess->GetAnimCelNum() - 1) {
			defended = false;
		}
	} else {
		animMatSuccess->SetAnimCelID(0);
	}

	if (!isActive()) return;

	{
		Ghost* ghost = getGameState()->getGhost();
		GhostAttack* attack = (ghost != NULL) ? ghost->getAttack() : NULL;

		if (attack != NULL && 
			((attack->getInterval() <  750 && attack->getInterval() > 600) ||
			 (attack->getInterval() <  400 && attack->getInterval() > 250))) {
			dotTexture = dotTextureRed;
		} else {
			dotTexture = dotTextureGreen;
		}
	}

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

	bool isDrwaing = true;
	for (int i = 0; i < DEFEND_TOUCHES_MAX; i++) {
		// Is any touch drawing?
		if (touch[i]->drawing) break;

		if (i == DEFEND_TOUCHES_MAX - 1) {
			isDrwaing = false;
			animMat->SetAnimCelID(0);
		}
	}

	if (isDrwaing) {
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
}

void CameraDefend::Update() {
	dotAngle += 5;
	if (dotAngle > 359)
		dotAngle = 0;

	Ghost* ghost = getGameState()->getGhost();

	if (ghost != NULL && ghost->isAttackDefendable() && !(ghost->getAttack() == NULL || ghost->getAttack()->isOver())) {
		if (!active) {
			reinit();
		}
		active = true;
		getFightTutorial()->triggerTutorial(TUTORIAL_DEFEND);
	} else {
		active = false;
	}
}

void CameraDefend::Touch(int32 x, int32 y, bool press, uint32 id) {
	if (id > DEFEND_TOUCHES_MAX) return;

	if (isActive() && press) {
		touch[id]->drawStart = CIwFVec2(x, y);
		if (dotCollides(&dotVertsTopLeftLeft, &dotVertsSizeLeft,  touch[id]->drawStart) || 
				dotCollides(&dotVertsTopLeftRight, &dotVertsSizeRight, touch[id]->drawStart)) {

			if (getFightTutorial() != NULL)
				getFightTutorial()->counterTrigger(TUTORIAL_DEFEND);
			touch[id]->drawing = true;
		}
	} else {
		Motion(x, y, id);
		touch[id]->drawing = false;
	}
}

void CameraDefend::Motion(int32 x, int32 y, uint32 id) {
	if (!isActive()) return;
	Ghost* ghost = getGameState()->getGhost();

	CIwFVec2 drawEnd = CIwFVec2(x, y);

	bool dotsCollideWithDrawing = 
		(dotCollides(&dotVertsTopLeftLeft, &dotVertsSizeLeft,  touch[id]->drawStart) && dotCollides(&dotVertsTopLeftRight, &dotVertsSizeRight, drawEnd)) ||
		(dotCollides(&dotVertsTopLeftRight, &dotVertsSizeRight, touch[id]->drawStart) && dotCollides(&dotVertsTopLeftLeft, &dotVertsSizeLeft,  drawEnd));

	if (dotsCollideWithDrawing) {
		active = false;
		if (ghost->getAttack() != NULL)
			defended = true;
			ghost->getAttack()->setDefended();

		/*IwTrace(GHOST_HUNTER, ("Player defend drawing %s; coords %.0f.%.0f to %.0f.%.0f - dot left coord %.0f.%.0f right coord %.0f.%.0f", 
			dotsCollideWithDrawing ? "accpeted" : "rejected",
			drawStart.x, drawStart.y, drawEnd.x, drawEnd.y, defendVertsLeft[0].x, defendVertsLeft[0].y,
			defendVertsRight[0].x, defendVertsRight[0].y));*/
	}
}

bool dotCollides(CIwFVec2* rectTopLeft, CIwFVec2* rectSize, CIwFVec2 point) {
	float collisionModifier = 0.0f;
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
