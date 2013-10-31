/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "MapRoamingGhost.h"

#include "IwGraphics.h"
#include "IwRandom.h"

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

MapRoamingGhost::MapRoamingGhost() {
	IwRandSeed((int32)s3eTimerGetMs());

	ghostTexture = Iw2DCreateImage("textures/map_ghost.png");
	matrix = new CIwFMat2D();

	ghostRoamingAngle = 0;
	ghostRoamingRadius = 0;
	roamingUpdate = clock();
}

MapRoamingGhost::~MapRoamingGhost() {
	if (ghostTexture)
		delete ghostTexture;

	if (matrix)
		delete matrix;
}

void MapRoamingGhost::Render() {

	int16 w = IwGxGetScreenWidth()*0.30f;

	float whScale = (float)((double)ghostTexture->GetWidth() / ghostTexture->GetHeight());
	int16 h = w * 1/whScale;

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);

	CIwFVec2 size = CIwFVec2(w, h);
	CIwFVec2 centre =
			CIwFVec2((int16)IwGxGetScreenWidth()/2 - size.x*0.18f,
					 (int16)IwGxGetScreenHeight()/2 - size.y*0.13f);
	
	CIwFVec2 topLeft = CIwFVec2(centre.x-size.x/2-ghostRoamingRadius, centre.y-size.y/2);

	matrix->SetRot(rad(ghostRoamingAngle), centre);
	Iw2DSetTransformMatrix(*matrix);

	Iw2DDrawImage(ghostTexture, topLeft, size);
}

void MapRoamingGhost::Update() {

	if (clock() > roamingUpdate) {
		ghostRoamingAngle++;
		roamingUpdate = clock() + IwRandMinMax(50, 80);
	}
	ghostRoamingRadius = IwRandMinMax(0, 1) ? ghostRoamingRadius + 1 : ghostRoamingRadius - 1;
	if (ghostRoamingRadius < 20) ghostRoamingRadius = 20;
	if (ghostRoamingRadius > 100) ghostRoamingRadius = 100;
}
