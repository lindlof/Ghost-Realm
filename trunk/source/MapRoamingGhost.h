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

#ifndef _MAP_ROAMING_GHOST_H
#define _MAP_ROAMING_GHOST_H

#include "Iw2D.h"
#include "s3eTimer.h"
#include "time.h"

class MapRoamingGhost {
	private:
	double centreX;
	double centreY;
	CIwFVec2 size;

	bool arrived;
	double moveHypotenuse;
	CIwFVec2 destination;
	double destinationAngle;
	void (*arrivalCallback)(MapRoamingGhost*);
	time_t moveTime;

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
	bool Touch(int x, int y, bool pressed);
	void Touch(uint32 id);

	void modifyCentreWithTexture(float x, float y);
	void setNotice(bool notice);
	void moveGhost(CIwFVec2 destination, void arrivalCallback(MapRoamingGhost*));
	void setCentre(CIwFVec2 centre);
};

#endif
