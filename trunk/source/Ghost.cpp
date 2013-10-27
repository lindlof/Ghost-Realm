/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Ghost.h"
#include "CameraModel.h"

#include "IwRandom.h"
#include "s3eTimer.h"
#include "IwDebug.h"
#include "s3eVibra.h"
#include "IwGx.h"

#define FOUND_ANIM_STEPS 200
#define ANIM_TURN_DEGS_PER_STEP 1.8f

Ghost::Ghost(GhostType ghostType, Player *player) {
	IwRandSeed((int32)s3eTimerGetMs());

	Ghost::ghostType = ghostType;
	int tries = 0;

	Ghost::player = player;

	foundAnimTime = 0;
	foundAnimProgress = 0;

	ectoplasm = GHOST_MAX_ECTOPLASM;

	staging = true;

#ifdef GHOST_DEBUG
	found = true;
	bearing = 0;
#else
	found = false;

	do {
		bearing = IwRandMinMax(0, 360);
		tries++;
	} while (abs(bearing - player->getHeading()) < 90 && 
		tries < 50);
#endif

	initRotation = IwRandMinMax(0, 360);
	rotation = initRotation;

	tappedTime = 0;
	tappedCount = 0;
	ghostAttack = NULL;

	hitTime = 0;
	playerAttackLast = 0;
	nextAttackInterval = 0;
	
	floatingAngle = 0;
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

	if (found && !gameIsHalt()) {

		if (foundAnimProgress < FOUND_ANIM_STEPS) {

			if (foundAnimProgress == 0) {
				// Ghost waits for 2 seconds before attacking
				playerAttackLast = clock();
				nextAttackInterval = 2000;
			}

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

	// Initiate new attacks
	if (found && getAttack() == NULL && nextAttackInterval <= 0) {
		attackDefendable = IwRandMinMax(0, 9) > 2; // 30 % of attacks not defendable
		ghostAttack = new GhostAttack(player, ghostType);
		playerAttackLast = clock();
		nextAttackInterval = IwRandMinMax(4000, 6000);
	}

	// Update attacks
	if (getAttack() != NULL) {
		if (getAttack()->isOver()) {
			delete ghostAttack;
			ghostAttack = NULL;
		} else {
			getAttack()->Update();
		}
	}

	// Decrease next attack interval
	if (!gameIsHalt()) {
		clock_t currentTime = clock();
		nextAttackInterval -= currentTime - playerAttackLast;
		playerAttackLast = currentTime;
	} else {
		playerAttackLast = clock();
	}

	return true;
}

int Ghost::getDistance() {
	int distanceDiff = ghostType.getDistance() - ghostType.getInitialDistance();

	distanceDiff *= ((float)foundAnimProgress)/FOUND_ANIM_STEPS;

	return ghostType.getInitialDistance() + distanceDiff;
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

int Ghost::getEctoplasm() {
	return ectoplasm;
}

// Ghost must be tapped for aggro
void Ghost::tapped() {
	if (player->isDead()) return;

	if (tappedCount == 0) {
		tappedTime = clock();
	}
	if (clock() - tappedTime < 1500) {
		tappedCount++;
	} else {
		tappedTime = 0;
		tappedCount = 0;
	}
	// Press and release both increase the count
	if (tappedCount > 1) {
		IwTrace(GHOST_HUNTER, ("Player touched a ghost"));
	}
}

GhostAttack* Ghost::getAttack() {
	return ghostAttack;
}

bool Ghost::isAttackDefendable() {
	return attackDefendable;
}

void Ghost::floatingAngleUpdate(float x, float y, float z) {

	double currentAngle;
	float ratio = y/960;
	if (ratio < 0) ratio *= -1;
	
	if (y < 0 && x > 0) { // 0 - 90 deg
		currentAngle = 0*ratio + 90*(1-ratio);
	} else if (y > 0 && x > 0) { // 90 - 180 deg
		currentAngle = 180*ratio + 90*(1-ratio);
	} else if (y > 0 && x < 0) { // 180 - 270 deg
		currentAngle = 180*ratio + 270*(1-ratio);
	} else { // 270 - 360 deg
		currentAngle = 360*ratio + 270*(1-ratio);
	}

	if (floatingAngle < 90 && currentAngle > 360-90) {
		floatingAngle += 360;
	} else if (floatingAngle > 360-90 && currentAngle < 90) {
		floatingAngle -= 360;
	}
	floatingAngle = floatingAngle*0.1f + currentAngle*0.9f;
}

double Ghost::getFloatingAngle() {
	return floatingAngle;
}
