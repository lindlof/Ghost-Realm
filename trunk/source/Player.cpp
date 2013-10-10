/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Player.h"

#include "IwDebug.h"
#include "IwRandom.h"
#include "s3eTimer.h"

Player::Player() {
	IwRandSeed((int32)s3eTimerGetMs());
	ready = 0;
	Player::heading = 0;
	Player::headingFilter = 0;
	Player::strike = new Strike();
	init();
};

void Player::init() {
	ghost = NULL;
	vitality = PLAYER_MAX_VITALITY;
	mana = PLAYER_MAX_MANA;
}

bool Player::playerUpdate() {

	// Fix going over north point of compass
	if (headingFilter < 90 && heading > 360-90) {
		headingFilter += 360;
	} else if (headingFilter > 360-90 && heading < 90) {
		headingFilter -= 360;
	}

	headingFilter = heading * HEADING_FILTER + 
		headingFilter * (1.f - HEADING_FILTER);

	return true;
}

bool Player::isReady() {
	return ready > 10;
}

void Player::playerGotHit(int hit) {
	Player::mana -= hit;
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
	vitality -= PLAYER_MAX_VITALITY/2;
	if (!isDead()) mana = PLAYER_MAX_MANA;
}

void Player::wonBattle() {
	setGhost(NULL);
	vitality += PLAYER_MAX_VITALITY/6;
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
	if (ghost != NULL && strikeRes > 0) {
		int manaLoss = 2;
		mana -= manaLoss;
		IwTrace(GHOST_HUNTER, ("Player loses mana as she hits %d, mana: %d", manaLoss, mana));

		// 7-10 successful hits to kill a ghost
		int hit = IwRandMinMax(GHOST_MAX_ECTOPLASM/100*7, GHOST_MAX_ECTOPLASM/100*10);
		IwTrace(GHOST_HUNTER, ("Player is hitting %d", hit));
		if (ghost != NULL) ghost->ghostGotHit(hit);
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
