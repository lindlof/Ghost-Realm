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
	vitality = MAX_PLAYER_VITALITY;
};

void Player::playerGotHit(int vitality) {
	Player::vitality -= vitality;
	IwTrace(GHOST_HUNTER, ("Player vitality %d", getVitality()));
}

int Player::getVitality() {
	return Player::vitality;
}
