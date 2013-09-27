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

#define DEGREE_IN_PIXELS 55
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

	middleMagnetTime = 0;
	middleMagnet = 0;

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

	{ // What is ghosts x coord in relation to the screen
		int ghostScreenDistance = abs(bearing - player->getHeading()) * DEGREE_IN_PIXELS;
		int ghostScreenArea = (int16)IwGxGetScreenWidth()/2 + 
				Ghost::getWidth() - ghostScreenDistance;

		if (ghostScreenArea > 0) {
			// Ghost is in the screen
			found = true;
		}

		if (bearing < player->getHeading()) {
			compassX = -Ghost::getWidth() + ghostScreenArea;
		} else {
			compassX = (int16)IwGxGetScreenWidth() - ghostScreenArea;
		}
	}

	if (found) {

		if (foundAnimProgress < FOUND_ANIM_STEPS) {

			if (clock() - foundAnimTime > 7) {
				foundAnimProgress++;
				foundAnimTime = clock();
			}

			// Increase scale each anim step so that it ends being 1
			scale = ORIGINAL_SCALE + (1-ORIGINAL_SCALE)/FOUND_ANIM_STEPS * foundAnimProgress;

			int16 midPositionX = getMidPositionX();

			// Ghost is dragged more heavily to middle as animation goes on
			Ghost::positionX = compassX * ((float)(FOUND_ANIM_STEPS-foundAnimProgress)/FOUND_ANIM_STEPS) + 
				midPositionX * ((float)(foundAnimProgress)/FOUND_ANIM_STEPS);
		
		}

		if (foundAnimProgress == FOUND_ANIM_STEPS) {
			float ghostMoveSpeed;
			float moveSmooth;

			int ghostDistance = abs(bearing - player->getHeading());
			
			{
				// Ghost moves torwards heading

				bool headingToMiddle = bearing < player->getHeading();

				if (ghostDistance < 1.3) {
					ghostMoveSpeed = (headingToMiddle) ? 2.f  : 1.3f;
				} else if (ghostDistance < 3) {
					ghostMoveSpeed = (headingToMiddle) ? 4.f  : 2.f;
				} else if (ghostDistance < 6) {
					ghostMoveSpeed = (headingToMiddle) ? 7.f  : 3.f;
				} else if (ghostDistance < 12) {
					ghostMoveSpeed = (headingToMiddle) ? 12.f : 9.f;
				} else if (ghostDistance < 20) {
					ghostMoveSpeed = (headingToMiddle) ? 16.f : 13.f;
				} else {
					ghostMoveSpeed = 20.f;
				}

				bearing = bearing * ((100-ghostMoveSpeed)/100) + 
					player->getHeading() * (ghostMoveSpeed/100);
			}

			{
				// Smoothen the ghost movement but try to keep up with
				// the bearing, except in very middle of the screen

				if (ghostDistance < 1.3) {
					moveSmooth = 98.f;
				} else if (ghostDistance < 3) {
					moveSmooth = 95.f;
				} else if (ghostDistance < 6) {
					moveSmooth = 93.f;
				} else if (ghostDistance < 12) {
					moveSmooth = 85.f;
				} else if (ghostDistance < 20) {
					moveSmooth = 75.f;
				} else {
					moveSmooth = 50.f;
				}

				Ghost::positionX = compassX * ((100.f-moveSmooth)/100) + 
					positionX * (moveSmooth/100);
			}

			{
				// Compas rarely sets the ghost to the very middle of the
				// screen so we installed a magnet in the very middle

				if (clock() - middleMagnetTime > 10) {

					if (ghostDistance < 1) {
						if (middleMagnet < 100) middleMagnet++;
					} else {
						if (middleMagnet > 0) middleMagnet--;
					}
					middleMagnetTime = clock();
				}

				Ghost::positionX = positionX * ((100.f-middleMagnet)/100) + 
					getMidPositionX() * (((float)middleMagnet)/100);
			}
		}
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

int16 Ghost::getMidPositionX() {
	return (int16)IwGxGetScreenWidth()/2-getWidth()/2;
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

void Ghost::floatingUpdate(int32 x, int32 y, int32 z) {
	
	/*if (found && foundAnimProgress == FOUND_ANIM_STEPS && 
			clock() - floatingTime > 50) {
		float floatingPower = 50.f;

		int16 midPositionX = getMidPositionX();
		Ghost::positionX = (positionX + x) * (floatingPower/100) + 
			(midPositionX * (100-floatingPower)/100);

		floatingTime = clock();
	}*/
}
