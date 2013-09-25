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
#include "IwGx.h"

#define FOUND_ANIM_STEPS 100
#define ORIGINAL_SCALE 0.5

float getGhostScale(GhostType ghostType);
float getWidthSpace(GhostType ghostType);
float getHeightSpace(GhostType ghostType);

Ghost::Ghost(GhostType ghostType, Player *player) {
	Ghost::ghostType = ghostType;

	Ghost::player = player;
	found = false;

	positionX = 0;
	positionY = 0;

	playerHitTime = 0;
	hitTime = 0;

	scale = 0.5;
	foundAnimTime = 0;
	foundAnimProgress = 0;

	ectoplasm = MAX_GHOST_ECTOPLASM;

	staging = true;
	IwRandSeed((int32)s3eTimerGetMs());
	bearing = IwRandMinMax(0, 360);
};

void Ghost::ghostGotHit() {
	if (found) {
		IwTrace(GHOST_HUNTER, ("Ghost got hit"));
		ectoplasm -= 40;
		hitTime = clock();
		s3eVibraVibrate(100, 100); 	
	}
}

clock_t Ghost::getHitTime() {
	return hitTime;
}

bool Ghost::ghostUpdate() {
	if (found && foundAnimProgress < FOUND_ANIM_STEPS) {

		if (clock() - foundAnimTime > 7) {
			foundAnimProgress++;
			foundAnimTime = clock();
		}

		// Increase scale each anim step so that it ends being 1
		scale = ORIGINAL_SCALE + (1-ORIGINAL_SCALE)/FOUND_ANIM_STEPS * foundAnimProgress;

		int midPositionX = (int16)IwGxGetScreenWidth()/2-getWidth()/2;

		// Ghost is dragged more heavily to middle as animation goes on
		positionX = compassX * ((float)(FOUND_ANIM_STEPS-foundAnimProgress)/FOUND_ANIM_STEPS) + 
			midPositionX * ((float)(foundAnimProgress)/FOUND_ANIM_STEPS);

		//positionX = (int16)IwGxGetScreenWidth()/2-getWidth()/2;

		//positionX = compassX;
		
	} else if (foundAnimProgress == FOUND_ANIM_STEPS) {
		positionX = (int16)IwGxGetScreenWidth()/2-getWidth()/2;
	}

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

int Ghost::getWidth() {
	float ghostWhScale = getGhostScale(Ghost::ghostType);
	float width = (int16)IwGxGetScreenWidth() / getWidthSpace(Ghost::ghostType);
	float height = (int16)IwGxGetScreenHeight() / getHeightSpace(Ghost::ghostType);

	if (ghostWhScale < width/height) {
		width = height * ghostWhScale;
	}

	width *= scale;
	return (int) width;
}

int Ghost::getHeight() {
	float ghostWhScale = getGhostScale(Ghost::ghostType);
	float width = (int16)IwGxGetScreenWidth() / getWidthSpace(Ghost::ghostType);
	float height = (int16)IwGxGetScreenHeight() / getHeightSpace(Ghost::ghostType);

	if (ghostWhScale >= width/height) {
		height = width * 1/ghostWhScale;
	}

	height *= scale;
	return (int) height;
}

int Ghost::getPositionX() {
	return positionX;
}

bool Ghost::isFound() {
	return found;
}

void Ghost::compassUpdate(double heading, bool error)
{
	if (foundAnimProgress >= FOUND_ANIM_STEPS) {
		return;
	}

	int ghostScreenDistance = abs(bearing - heading) * 55;
	int ghostScreenArea = (int16)IwGxGetScreenWidth()/2 + 
		Ghost::getWidth() - ghostScreenDistance;

	if (ghostScreenArea > 0) {
		// Ghost is in the screen
		found = true;

		if (bearing < heading) {
			compassX = -Ghost::getWidth() + ghostScreenArea;
		} else {
			compassX = (int16)IwGxGetScreenWidth() - ghostScreenArea;
		}
	}
}

float getGhostScale(GhostType ghostType) {
	switch(ghostType) {
		case GHOST_NORMAL: return 124.f/273;
	}
	return -1;
}

float getWidthSpace(GhostType ghostType) {
	switch(ghostType) {
		case GHOST_NORMAL: return 1.5;
	}
	return -1;
}

float getHeightSpace(GhostType ghostType) {
	switch(ghostType) {
		case GHOST_NORMAL: return 2;
	}
	return -1;
}

int Ghost::getEctoplasm() {
	return ectoplasm;
}
