/* ========================================================================= *
 * Ghost Hunter Project                                                      *
 * https://ghosthuntermobile.wordpress.com/                                  *
 * ========================================================================= *
 * Copyright (C) 2013 Ghost Hunter Project                                   *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 * ========================================================================= */

#include "GhostAttack.h"
#include "CameraModel.h"
#include "GameState.h"

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

	float strength;
	if (getGameState()->getIntroState() == INTRO_ATTACK) {
		strength = 0.2f;
	} else {
		strength = ghostType.getStrength();
	}

	hit = hit*strength;
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

clock_t GhostAttack::getInterval() {
	return interval;
}
