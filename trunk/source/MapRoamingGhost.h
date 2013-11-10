/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _MAP_ROAMING_GHOST_H
#define _MAP_ROAMING_GHOST_H

#include "Iw2D.h"
#include "s3eTimer.h"
#include "time.h"

class MapRoamingGhost {
	private:
	CIwFVec2 centre;
	CIwFVec2 size;

	CIwFVec2 tapAreaTopLeft;
	CIwFVec2 tapAreaSize;
	clock_t pressedTime;

	CIw2DImage* ghostTexture;
	CIwFMat2D *matrix;

	bool notice;
	CIw2DImage* noticeTexture;

	float randomModifier;
	clock_t roamingModifierUpdate;
	clock_t roamingUpdate;

	int ghostRoamingAngle;
	int ghostRoamingRadius;

	public:
	MapRoamingGhost(char *texture, CIwFVec2 centre);
	~MapRoamingGhost();

	void Render();
	void Update();
	void Touch(int x, int y, bool pressed);

	void modifyCentreWithTexture(float x, float y);
	void setNotice(bool notice);
};

#endif
