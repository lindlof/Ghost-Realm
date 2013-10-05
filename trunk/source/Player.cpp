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

Player::Player() {
	ghost = NULL;
	Player::strike = new Strike();
	vitality = MAX_PLAYER_VITALITY;
};

void Player::playerGotHit(int vitality) {
	Player::vitality -= vitality;
	IwTrace(GHOST_HUNTER, ("Player vitality %d", getVitality()));
}

int Player::getVitality() {
	return Player::vitality;
}

void Player::compassUpdate(double heading, bool error)
{
	Player::heading = heading;
}

double Player::getHeading() {
	return heading;
}

void Player::accelometerUpdate(int32 x, int32 y, int32 z) {
	int strikeRes = strike->strikeUpdate(x, y, z);
	if (strikeRes > 0) {
		IwTrace(GHOST_HUNTER, ("Player is hitting"));
		if (ghost != NULL) ghost->ghostGotHit();
	}
}

void Player::setGhost(Ghost *ghost) {
	Player::ghost = ghost;
}
