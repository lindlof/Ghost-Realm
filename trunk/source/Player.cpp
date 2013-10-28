/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Player.h"
#include "CameraModel.h"

#include "GameState.h"
#include "IwDebug.h"
#include "IwRandom.h"
#include "s3eTimer.h"
#include "s3eVibra.h"

Player::Player() {
	IwRandSeed((int32)s3eTimerGetMs());
	ready = 0;
	heading = 0;
	headingFilter = 0;
	strike = new Strike();
	hitTime = 0;
	init();
};

void Player::init() {
	ghost = NULL;
	vitality = PLAYER_MAX_VITALITY;
	mana = PLAYER_MAX_MANA;
}

bool Player::headingUpdate() {

	// Fix going over north point of compass
	if (headingFilter < 90 && heading > 360-90) {
		headingFilter += 360;
	} else if (headingFilter > 360-90 && heading < 90) {
		headingFilter -= 360;
	}

	float filterStrength = getGameState()->getGameMode() == CAMERA_MODE ? 
		HEADING_FILTER_CAMERA : HEADING_FILTER_MAP;

	headingFilter = heading * filterStrength + 
		headingFilter * (1.f - filterStrength);

	return true;
}

bool Player::isReady() {
	return ready > 1;
}

void Player::playerGotHit(int hit) {
	Player::mana -= hit;
	if (hit > 0) hitTime = clock();
	s3eVibraVibrate(100, 230);
	IwTrace(GHOST_HUNTER, ("Player vitality %d", getVitality()));
}

int Player::getVitality() {
	return Player::vitality;
}

int Player::getMana() {
	return Player::mana;
}

void Player::lostBattle() {
	setGhost(NULL);
	vitality -= 2;
	if (!isDead()) mana = PLAYER_MAX_MANA;
}

void Player::wonBattle() {
	setGhost(NULL);
	vitality += 1;
	if (vitality > PLAYER_MAX_VITALITY) vitality = PLAYER_MAX_VITALITY;
	mana = PLAYER_MAX_MANA;
}

void Player::compassUpdate(double heading, bool error) {
	Player::heading = heading;
	if (!isReady()) {
		headingFilter = heading;
		ready++;
	}
}

double Player::getHeading() {
	return headingFilter;
}

void Player::accelometerUpdate(int32 x, int32 y, int32 z) {
	int strikeRes = strike->strikeUpdate(x, y, z);

	int ghostAngle = abs(getHeading() - getGameState()->getGhost()->getBearing());
	bool angleOk = ghostAngle < 30;

	if (angleOk && getFightTutorial() != NULL) {
		getFightTutorial()->triggerTutorial(TUTORIAL_ATTACK);
	}

	if (strikeRes > 0 && isReady()) {

		{ // Initial hit?
			if (angleOk && ghost == NULL) {
				setGhost(getGameState()->getGhost());
				ghost->setFound();
			}
		}

		if (ghost != NULL) {
			int manaLoss = 2;
			mana -= manaLoss;
			IwTrace(GHOST_HUNTER, ("Player loses mana as she hits %d, mana: %d", manaLoss, mana));

			// 7-10 successful hits to kill a ghost
			int hit = IwRandMinMax(GHOST_MAX_ECTOPLASM/100*7, GHOST_MAX_ECTOPLASM/100*10);
			if (IwRandMinMax(0, 15) == 15) hit *= 2; // Player crit, 1/16 chance

			IwTrace(GHOST_HUNTER, ("Player is hitting %d", hit));
			if (ghost != NULL) ghost->ghostGotHit(hit);
		}
	}
}

void Player::setGhost(Ghost *ghost) {
	Player::ghost = ghost;
}

bool Player::isDead() {
	return vitality <= 0;
}

void Player::resurrect() {
	init();
}

clock_t Player::getHitTime() {
	return hitTime;
}
