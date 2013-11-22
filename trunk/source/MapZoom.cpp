/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "MapZoom.h"

#include "math.h"
#include "IwGraphics.h"

MapZoom::MapZoom() {
	for (int i = 0; i < MAX_TOUCHES; i++) {
		touch[i].active = false;
	}

	currentZoom = 1.5f;
	lastDistance = 0;

	resetTaps();
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
	currentZoom += (newDistance - lastDistance)*zoomStrength;

	lastDistance = newDistance;

	if (currentZoom > 3) currentZoom = 3;
	if (currentZoom < 1) currentZoom = 1;
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
			resetTaps();
			currentZoom = 1.5f;
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
