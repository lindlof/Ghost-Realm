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
	int i;
	vitality = MAX_PLAYER_VITALITY;
	for (i = 0; i < HEADING_SIZE; i++) {
		headingArr[i] = 0;
	}
	headingFilled = 0;
};

void Player::playerGotHit(int vitality) {
	Player::vitality -= vitality;
	IwTrace(GHOST_HUNTER, ("Player vitality %d", getVitality()));
}

int Player::getVitality() {
	return Player::vitality;
}

void Player::compassUpdate(double newHeading, bool error)
{
	int i;
	double heading = 0;
	for (i = HEADING_SIZE-1; i > 0; i--) {
		Player::headingArr[i] = Player::headingArr[i-1];
		heading += Player::headingArr[i];
	}
	Player::headingArr[0] = newHeading;
	heading += newHeading;

	if (headingFilled < HEADING_SIZE) headingFilled++;

	if (headingFilled != 0) {
		heading = heading / headingFilled;
	}

	Player::heading = heading;
}

double Player::getHeading() {
	return heading;
}
