/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Ghost.h"

#include "IwRandom.h"
#include "s3eTimer.h"
#include "IwDebug.h"

Ghost::Ghost() {
	positionX = 0;
	positionY = 0;
	width = 150;
	height = 300;

	hit = false;
	hitTime = 0;

	staging = true;
	IwRandSeed((int32)s3eTimerGetMs());
	compassPoint = IwRandMinMax(0, 360);
};

void Ghost::ghostGotHit() {
	IwTrace(GHOST_HUNTER, ("Ghost got hit"));
	hit = true;
	hitTime = clock();
}

bool Ghost::isGhostHit() {
	return hit;
}

bool Ghost::ghostUpdate() {
	if (clock() - hitTime > 100) {
		hit = false;
	}
}

void Ghost::compassUpdate(double heading, bool error)
{
	
}
