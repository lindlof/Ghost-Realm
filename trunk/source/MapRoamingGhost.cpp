/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "MapRoamingGhost.h"

#include "GameState.h"
#include "CameraModel.h"

#include "IwGraphics.h"
#include "IwRandom.h"
#include "cmath"

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

MapRoamingGhost::MapRoamingGhost(char *texture, CIwFVec2 centre) {
	IwRandSeed((int32)s3eTimerGetMs());

	ghostTexture = Iw2DCreateImage(texture);
	matrix = new CIwFMat2D();

	{
		this->centre = centre;

		int16 w = IwGxGetScreenWidth()*0.29f;

		float whScale = (float)((double)ghostTexture->GetWidth() / ghostTexture->GetHeight());
		int16 h = w * 1/whScale;

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);

		this->size = CIwFVec2(w, h);
	}

	ghostRoamingAngle = IwRandMinMax(0, 359);
	ghostRoamingRadius = 0;

	roamingUpdate = 0;
	roamingModifierUpdate = 0;
	randomModifier = 0;

	noticeTexture = NULL;
	notice = false;

	tapAreaTopLeft = CIwFVec2(0, 0);
	tapAreaSize = CIwFVec2(0, 0);
}

MapRoamingGhost::~MapRoamingGhost() {
	if (ghostTexture)
		delete ghostTexture;

	if (matrix)
		delete matrix;

	if (noticeTexture)
		delete noticeTexture;
}

void MapRoamingGhost::Render() {
	CIwFVec2 topLeft = CIwFVec2(centre.x-size.x/2+ghostRoamingRadius, centre.y-size.y/2);

	matrix->SetRot(rad(ghostRoamingAngle), centre);
	Iw2DSetTransformMatrix(*matrix);

	Iw2DDrawImage(ghostTexture, topLeft, size);

	if (notice) {
		Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);
		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);

		int16 w = (double)IwGxGetScreenWidth()*0.45;

		float whScale = (float)((double)noticeTexture->GetWidth() / noticeTexture->GetHeight());
		int16 h = w * 1/whScale;
	
		double roamingX = cos(rad(ghostRoamingAngle)) * ghostRoamingRadius;
		double roamingY = sin(rad(ghostRoamingAngle)) * ghostRoamingRadius;

		float ghostEmptyPadding = 0.17f;
		CIwFVec2 noticeTopLeft = CIwFVec2(
			topLeft.x - (w - size.x)/2 + roamingX, 
			topLeft.y - h + size.y*ghostEmptyPadding + roamingY - 1);
		CIwFVec2 noticeSize = CIwFVec2(w, h);
		Iw2DDrawImage(noticeTexture, noticeTopLeft, noticeSize);

		tapAreaTopLeft = CIwFVec2(topLeft.x < noticeTopLeft.x ? topLeft.x : noticeTopLeft.x, noticeTopLeft.y);
		tapAreaSize = CIwFVec2(size.x > noticeSize.x ? size.x : noticeSize.x, topLeft.y + size.y - noticeTopLeft.y);
	} else {
		tapAreaTopLeft = CIwFVec2(0, 0);
		tapAreaSize = CIwFVec2(0, 0);
	}
}

void MapRoamingGhost::Update() {

	if (clock() > roamingModifierUpdate) {
		// Random modifier to keep the speed of the ghost random over period of time
		randomModifier = (float)IwRandMinMax(10, 130)/100;
		roamingModifierUpdate = clock() + IwRandMinMax(1200, 2200);
	}
	if (clock() > roamingUpdate) {
		ghostRoamingAngle++;
		if (ghostRoamingAngle > 359)
			ghostRoamingAngle = 0;
		roamingUpdate = clock() + IwRandMinMax(50, 80) * randomModifier;
	}
	ghostRoamingRadius = IwRandMinMax(0, 1) ? ghostRoamingRadius + 1 : ghostRoamingRadius - 1;
	if (ghostRoamingRadius < 20) ghostRoamingRadius = 20;
	if (ghostRoamingRadius > 100) ghostRoamingRadius = 100;
}

void MapRoamingGhost::modifyCentreWithTexture(float x, float y) {
	centre.x += size.x*x;
	centre.y += size.y*y;
}

void MapRoamingGhost::setNotice(bool notice) {
	if (notice && !noticeTexture)
		noticeTexture = Iw2DCreateImage("textures/map_fight_notice.png");
	this->notice = notice;
}

void MapRoamingGhost::Touch(int x, int y, bool pressed) {
	if (getGameState()->getGhost() == NULL) return;

	if (x > tapAreaTopLeft.x &&
		x < tapAreaTopLeft.x + tapAreaSize.x &&
		y > tapAreaTopLeft.y && 
		y < tapAreaTopLeft.y + tapAreaSize.y) {

		if (pressed) {
			pressedTime = clock();
		}

		if (clock() - pressedTime < 1500 && !pressed) {
			IwTrace(GHOST_HUNTER, ("Player wants to fight"));
			initFight();
			getGameState()->setGameMode(CAMERA_MODE);
		}
	} else {
		pressedTime = 0;
	}
}
