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
#define ORIGINAL_SCALE 0.5f

float getGhostTypeDistance(GhostType ghostType);

Ghost::Ghost(GhostType ghostType, Player *player) {
	Ghost::ghostType = ghostType;
	int tries = 0;

	Ghost::player = player;
	found = false;

	middleMagnetTime = 0;
	middleMagnet = 0;

	playerHitTime = 0;
	hitTime = 0;

	scale = ORIGINAL_SCALE;
	foundAnimTime = 0;
	foundAnimProgress = 0;

	ectoplasm = MAX_GHOST_ECTOPLASM;

	staging = true;
	IwRandSeed((int32)s3eTimerGetMs());
	do {
		bearing = IwRandMinMax(0, 360);
		tries++;
	} while (abs(bearing - player->getHeading()) < 90 && 
		tries < 50);
};

void Ghost::ghostGotHit() {
	ectoplasm -= 40;
	IwTrace(GHOST_HUNTER, ("Ghost got hit, ectoplasm: %d", ectoplasm));
	hitTime = clock();
	s3eVibraVibrate(100, 100);
}

clock_t Ghost::getHitTime() {
	return hitTime;
}

bool Ghost::ghostUpdate() {

	double ghostDistance = abs(bearing - player->getHeading());

	if (!found && ghostDistance < 20) {
		// Ghost is middle enough of the screen to aggro
		found = true;
		player->setGhost(this);
	}

	if (found) {

		if (foundAnimProgress < FOUND_ANIM_STEPS) {

			if (clock() - foundAnimTime > 7) {
				foundAnimProgress++;
				foundAnimTime = clock();
			}

			// Increase scale each anim step so that it ends being 1
			scale = ORIGINAL_SCALE + (1-ORIGINAL_SCALE)/FOUND_ANIM_STEPS * foundAnimProgress;
		}

		float ghostMoveSpeed;
		float moveSmooth;
			
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

			if (bearing < 90 && player->getHeading() > 360-90) {
				// Going clockwise over north point of compass
				bearing += 360;
			} else if (bearing > 360-90 && player->getHeading() < 90) {
				// Going counterclockwise over north point of compass
				bearing -= 360;
			}

			bearing = bearing * ((100-ghostMoveSpeed)/100) + 
				player->getHeading() * (ghostMoveSpeed/100);
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

int Ghost::getStrength() {
	switch(Ghost::ghostType) {
		case GHOST_NORMAL: return 5;
	}
	return -1;
}

float Ghost::getDistance() {
	return getGhostTypeDistance(Ghost::ghostType) * 1.f/scale;
}

bool Ghost::isFound() {
	return found;
}

double Ghost::getBearing() {
	return bearing;
}

float getGhostTypeDistance(GhostType ghostType) {
	switch(ghostType) {
		case GHOST_NORMAL: return 400.f;
	}
	return -1;
}

float getGhostTypeHeight(GhostType ghostType) {
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
