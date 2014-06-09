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

#ifndef _MAP_ZOOM_H
#define _MAP_ZOOM_H

#include "Iw2D.h"
#include "time.h"

class MapZoomTouch {
public:
	bool active;
	CIwFVec2 location;
};

class MapZoom {
private:
	static const int MAX_TOUCHES = 2;

	float currentZoom;
	float destinationZoom;

	int tapCount;
	clock_t pressedTime;
	clock_t lastTapTime;

	int lastDistance;
	MapZoomTouch touch[MAX_TOUCHES];
	double getDistance(int id1, int id2);

	void setZoom(float zoom);

public:
	MapZoom();

	void Update();

	void Touch(int32 x, int32 y, bool press, uint32 id);
	void Motion(int32 x, int32 y, uint32 id);

	void Touch(int32 x, int32 y, bool pressed, bool touchReserved);
	void resetTaps();

	float getZoom();
};

#endif
