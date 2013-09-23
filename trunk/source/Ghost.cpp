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
#include "s3eVibra.h"

Ghost::Ghost(GhostType ghostType, Player *player) {
	Ghost::ghostType = ghostType;

	Ghost::player = player;
	found = true;

	positionX = 0;
	positionY = 0;
	width = 150;
	height = 300;

	playerHitTime = 0;
	hitTime = 0;

	staging = true;
	IwRandSeed((int32)s3eTimerGetMs());
	compassPoint = IwRandMinMax(0, 360);
};

void Ghost::ghostGotHit() {
	IwTrace(GHOST_HUNTER, ("Ghost got hit"));
	hitTime = clock();
	s3eVibraVibrate(100, 100); 	
}

clock_t Ghost::getHitTime() {
	return hitTime;
}

bool Ghost::ghostUpdate() {
	// If the ghost is found it may hit the player
	if (found && clock() - playerHitTime > 5000) {
		int hit = getStrength() * IwRandMinMax(1, 10);
		hit = hit*hit;
		player->playerGotHit(hit);
		IwTrace(GHOST_HUNTER, ("Player got hit for %d", hit));
		playerHitTime = clock();
	}
	return true;
}

int Ghost::getStrength() {
	switch(Ghost::ghostType) {
		case GHOST_NORMAL: return 5;
	}
	return -1;
}

void Ghost::compassUpdate(double heading, bool error)
{
	
}
