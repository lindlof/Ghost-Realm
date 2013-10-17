/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GhostAttack.h"

#include "IwRandom.h"

GhostAttack::GhostAttack(Player* player, GhostType ghostType) {
	if (IwRandMinMax(0, 11) == 11) {
		// Critical hit
		hit = 25;
	} else {
		hit = ((double)IwRandMinMax(PLAYER_MAX_MANA*11, PLAYER_MAX_MANA*19))/PLAYER_MAX_MANA;
	}

	this->player = player;

	hit = hit*ghostType.getStrength();
	hitTime = clock() + IwRandMinMax(2000, 2400);

	over = false;
}

void GhostAttack::Update() {
	if (!over && clock() > hitTime) {
		player->playerGotHit((int)hit);
		IwTrace(GHOST_HUNTER, ("Player got hit for %f", hit));
		over = true;
	}
}

void GhostAttack::setDefended() {
	over = true;
}

bool GhostAttack::isOver() {
	return over;
}
