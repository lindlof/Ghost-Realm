/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _MAP_ZOOM_H
#define _MAP_ZOOM_H

#include "Iw2D.h"

class MapZoomTouch {
public:
	bool active;
	CIwFVec2 location;
};

class MapZoom {
private:
	static const int MAX_TOUCHES = 2;

	float currentZoom;

	int lastDistance;
	MapZoomTouch touch[MAX_TOUCHES];
	double getDistance(int id1, int id2);

public:
	MapZoom();

	void Touch(int32 x, int32 y, bool press, uint32 id);
	void Motion(int32 x, int32 y, uint32 id);

	float getZoom();
};

#endif
