/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GhostAttack.h"
#include "CameraModel.h"

#include "Audio.h"
#include "IwRandom.h"

GhostAttack::GhostAttack(Player* player, GhostType ghostType) {
	if (IwRandMinMax(0, 11) == 11) {
		// Critical hit
		hit = 50;
	} else {
		hit = ((double)IwRandMinMax(PLAYER_MAX_MANA*27, PLAYER_MAX_MANA*40))/PLAYER_MAX_MANA;
	}

	this->player = player;

	hit = hit*ghostType.getStrength();
	lastTime = clock();
	interval = 3000;

	over = false;
}

void GhostAttack::Update() {
	if (!gameIsHalt()) {
		clock_t currentTime = clock();
		interval -= currentTime - lastTime;
		lastTime = currentTime;
	} else {
		lastTime = clock();
	}

	if (!over && interval <= 0) {
		player->playerGotHit((int)hit);
		IwTrace(GHOST_HUNTER, ("Player got hit for %f", hit));
		over = true;
	}
}

void GhostAttack::setDefended() {
	Audio::Play("sounds/camera_ghost_attack_miss.mp3");
	over = true;
}

bool GhostAttack::isOver() {
	return over;
}
