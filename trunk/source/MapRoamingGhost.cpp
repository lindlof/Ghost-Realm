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
#include "MapModel.h"

#include "IwGraphics.h"
#include "IwRandom.h"
#include "cmath"

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

double inline dabs(double d) { return d < 0 ? d*-1 : d; }

MapRoamingGhost::MapRoamingGhost(char *texture, CIwFVec2 centre) {
	IwRandSeed((int32)s3eTimerGetMs());

	ghostTexture = Iw2DCreateImage(texture);
	matrix = new CIwFMat2D();

	{
		setCentre(centre);

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
	float zoom = getMapZoom()->getZoom();

	double hScrW = (double)IwGxGetScreenWidth()/2;
	double hScrH = (double)IwGxGetScreenHeight()/2;

	CIwFVec2 centre = CIwFVec2(hScrW+zoom*(this->centreX-hScrW), hScrH+zoom*(this->centreY-hScrH));
	CIwFVec2 topLeft = CIwFVec2(
		centre.x - zoom*(size.x/2) + zoom*ghostRoamingRadius, 
		centre.y - zoom*(size.y/2));
	CIwFVec2 size = CIwFVec2(this->size.x*zoom, this->size.y*zoom);

	matrix->SetRot(rad(ghostRoamingAngle), centre);
	Iw2DSetTransformMatrix(*matrix);

	Iw2DDrawImage(ghostTexture, topLeft, size);

	if (notice) {
		Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);
		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);

		int16 w = (double)IwGxGetScreenWidth()*0.63;

		float whScale = (float)((double)noticeTexture->GetWidth() / noticeTexture->GetHeight());
		int16 h = w * 1/whScale;
	
		double roamingX = cos(rad(ghostRoamingAngle)) * zoom*ghostRoamingRadius;
		double roamingY = sin(rad(ghostRoamingAngle)) * zoom*ghostRoamingRadius;

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

	if (!arrived) {
		double travelX = cos(destinationAngle)*6.f;
		double travelY = sin(destinationAngle)*6.f;
		if (abs(centreX-destination.x) < abs((centreX+travelX)-destination.x))
			travelX = 0;
		if (abs(centreY-destination.y) < abs((centreY+travelY)-destination.y))
			travelY = 0;
		centreX += travelX;
		centreY += travelY;
		if (travelX == 0 && travelY == 0) {
			centreX = destination.x;
			centreY = destination.y;
			arrived = true;
			arrivalCallback(this);
			arrivalCallback = NULL;
		}
	}
}

void MapRoamingGhost::moveGhost(CIwFVec2 destination, void arrivalCallback(MapRoamingGhost*)) {
	this->arrived = false;
	this->arrivalCallback = arrivalCallback;
	this->destination = destination;
	this->destinationAngle = atan(dabs(destination.y-centreY) / dabs(destination.x-centreX));
	if (destination.x < centreX) {
		destinationAngle *= -1;
		destinationAngle += PI;
	}
	if (destination.y < centreY) {
		destinationAngle *= -1;
	}
}

void MapRoamingGhost::setCentre(CIwFVec2 centre) {
	this->centreX = centre.x;
	this->centreY = centre.y;
	this->destination = centre;
	this->arrived = true;
	this->arrivalCallback = NULL;
}

void MapRoamingGhost::modifyCentreWithTexture(float x, float y) {
	destination.x = centreX += size.x*x;
	destination.y = centreY += size.y*y;
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

void MapRoamingGhost::Touch(uint32 id) {
	// Multiple touches decline ghost pressing
	if (id > 0) pressedTime = 0;
}
