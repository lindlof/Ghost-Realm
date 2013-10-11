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

#define FOUND_ANIM_STEPS 200
#define ANIM_TURN_DEGS_PER_STEP 1.8f

int getGhostTypeDistance(GhostType ghostType);
int getGhostTypeInitialDistance(GhostType ghostType);

Ghost::Ghost(GhostType ghostType, Player *player) {
	IwRandSeed((int32)s3eTimerGetMs());

	Ghost::ghostType = ghostType;
	int tries = 0;

	Ghost::player = player;
	found = false;

	middleMagnetTime = 0;
	middleMagnet = 0;

	playerHitTime = 0;
	hitTime = 0;
	nextHitInterval = 0;

	foundAnimTime = 0;
	foundAnimProgress = 0;

	ectoplasm = GHOST_MAX_ECTOPLASM;

	staging = true;
	do {
		bearing = IwRandMinMax(0, 360);
		tries++;
	} while (abs(bearing - player->getHeading()) < 90 && 
		tries < 50);

	initRotation = IwRandMinMax(0, 360);
	rotation = initRotation;

	tappedTime = 0;
	tappedCount = 0;
};

void Ghost::ghostGotHit(int hit) {
	// 75% of hits success
	if (IwRandMinMax(0, 100) <= 75) {
		ectoplasm -= hit;
		IwTrace(GHOST_HUNTER, ("Ghost got hit, ectoplasm: %d", ectoplasm));
		hitTime = clock();
		s3eVibraVibrate(100, 100);
	} else {
		IwTrace(GHOST_HUNTER, ("Ghost dodged a hit, ectoplasm: %d", ectoplasm));
		s3eVibraVibrate(300, 20);
	}
}

clock_t Ghost::getHitTime() {
	return hitTime;
}

bool Ghost::ghostUpdate() {

	double ghostDistance = abs(bearing - player->getHeading());

	if (found) {

		if (foundAnimProgress < FOUND_ANIM_STEPS) {

			if (clock() - foundAnimTime > 3) {
				foundAnimProgress++;
				foundAnimTime = clock();
			}

			if (rotation != 0) {
				// Ghost rotates torwards player
				if (initRotation < 180) {
					rotation = initRotation - (float)foundAnimProgress*ANIM_TURN_DEGS_PER_STEP;
					if (rotation < 0) rotation = 0;
				} else {
					rotation = initRotation + (float)foundAnimProgress*ANIM_TURN_DEGS_PER_STEP;
					if (rotation > 360) rotation = 0;
				}
			}
		}

		float ghostMoveSpeed;
		float moveSmooth;
			
		{
			// Ghost moves torwards heading
			if (ghostDistance < 1.3) {
				ghostMoveSpeed = 2.f;
			} else if (ghostDistance < 3) {
				ghostMoveSpeed = 4.f;
			} else if (ghostDistance < 6) {
				ghostMoveSpeed = 7.f;
			} else if (ghostDistance < 12) {
				ghostMoveSpeed = 12.f;
			} else if (ghostDistance < 20) {
				ghostMoveSpeed = 16.f;
			} else {
				ghostMoveSpeed = 20.f;
			}

			// Fix going over north point of compass
			if (bearing < 90 && player->getHeading() > 360-90) {
				bearing += 360;
			} else if (bearing > 360-90 && player->getHeading() < 90) {
				bearing -= 360;
			}

			bearing = 
				bearing * ((100.f-ghostMoveSpeed)/100) + 
				player->getHeading() * (ghostMoveSpeed/100);
		}
	}

	// If the ghost is found it may hit the player
	if (found && clock() - playerHitTime > nextHitInterval) {
		double hit;
		if (IwRandMinMax(0, 11) == 11) {
			// Critical hit
			hit = 25;
		} else {
			hit = ((double)IwRandMinMax(PLAYER_MAX_MANA*11, PLAYER_MAX_MANA*19))/PLAYER_MAX_MANA;
		}

		hit = hit*getStrength();
		player->playerGotHit((int)hit);
		IwTrace(GHOST_HUNTER, ("Player got hit for %f", hit));

		playerHitTime = clock();
		nextHitInterval = IwRandMinMax(4000, 6000);
	}

	return true;
}

float Ghost::getStrength() {
	switch(Ghost::ghostType) {
		case GHOST_NORMAL: return 1;
	}
	return -1;
}

int Ghost::getDistance() {
	int distanceDiff = getGhostTypeDistance(Ghost::ghostType) - 
			getGhostTypeInitialDistance(Ghost::ghostType);

	distanceDiff *= ((float)foundAnimProgress)/FOUND_ANIM_STEPS;

	return getGhostTypeInitialDistance(Ghost::ghostType) + distanceDiff;
}

void Ghost::setFound() {
	found = true;
}

bool Ghost::isFound() {
	return found;
}

double Ghost::getBearing() {
	return bearing;
}

float Ghost::getRotation() {
	float facingPlayer = 180;

	return facingPlayer + rotation;
}

int getGhostTypeDistance(GhostType ghostType) {
	switch(ghostType) {
		case GHOST_NORMAL: return 400;
	}
	return -1;
}

int getGhostTypeInitialDistance(GhostType ghostType) {
	switch(ghostType) {
		case GHOST_NORMAL: return 1000;
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

// Ghost must be tapped for aggro
void Ghost::tapped() {
	if (player->isDead()) return;

	if (tappedCount == 0) {
		tappedTime = clock();
	}
	if (clock() - tappedTime < 500) {
		tappedCount++;
	} else {
		tappedTime = 0;
		tappedCount = 0;
	}
	// Press and release both increase the count
	if (tappedCount > 1) {
		IwTrace(GHOST_HUNTER, ("Player touched a ghost"));
		if (player->isReady()) {
			this->setFound();
			player->setGhost(this);
		}
	}
}
