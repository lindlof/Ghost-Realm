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

#include "MapZoom.h"

#include "math.h"
#include "IwGraphics.h"

inline int getSign(float a) { return a == 0 ? 0 : a > 0 ? +1 : -1; }

MapZoom::MapZoom() {
	for (int i = 0; i < MAX_TOUCHES; i++) {
		touch[i].active = false;
	}

	setZoom(1.5f);
	lastDistance = 0;

	resetTaps();
}

void MapZoom::Update() {
	if (currentZoom != destinationZoom) {
		int pol = getSign(destinationZoom - currentZoom);
		currentZoom += 0.10f*pol;
		if ((pol > 0 && currentZoom > destinationZoom) ||
			(pol < 0 && currentZoom < destinationZoom)) {
				setZoom(destinationZoom);
		}
	}
}

void MapZoom::setZoom(float zoom) {
	currentZoom = zoom;
	destinationZoom = zoom;
}

void MapZoom::Touch(int32 x, int32 y, bool press, uint32 id) {
	// Multiple touches decline double tap zooming
	if (id > 0) {
		resetTaps();
	}

	if (id >= MAX_TOUCHES) return;
	touch[id].location = CIwFVec2(x, y);
	touch[id].active = press;
	lastDistance = getDistance(0, 1);
}

void MapZoom::Motion(int32 x, int32 y, uint32 id) {
	if (id >= MAX_TOUCHES) return;
	touch[id].location = CIwFVec2(x, y);

	if (!touch[0].active || !touch[1].active) return;

	float zoomStrength = 3.0/IwGxGetScreenWidth();
	double newDistance = getDistance(0, 1);
	setZoom(currentZoom + (newDistance - lastDistance)*zoomStrength);

	lastDistance = newDistance;

	if (currentZoom > 3) setZoom(3);
	if (currentZoom < 1) setZoom(1);
}

void MapZoom::Touch(int32 x, int32 y, bool pressed, bool touchReserved) {
	if (touchReserved) {
		resetTaps();
		return;
	}

	if (pressed) {
		pressedTime = clock();
	}

	if (clock() - pressedTime < 1500 && !pressed) {
		tapCount++;

		if (tapCount > 1 && clock() - lastTapTime < 2200) {
			// Double tap registered
			resetTaps();

			if (currentZoom >= 1.5 && currentZoom < 2) {
				destinationZoom = 2.5f;
			} else {
				destinationZoom = 1.5f;
			}
		} else {
			lastTapTime = clock();
			pressedTime = 0;
		}
	}
}

void MapZoom::resetTaps() {
	tapCount = 0;
	pressedTime = 0;
	lastTapTime = 0;
}

double MapZoom::getDistance(int id1, int id2) {
	CIwFVec2 loc1 = touch[id1].location;
	CIwFVec2 loc2 = touch[id2].location;
	return sqrt(pow(loc1.x - loc2.x, 2) + pow(loc1.y - loc2.y, 2));
}

float MapZoom::getZoom() {
	return currentZoom;
}
