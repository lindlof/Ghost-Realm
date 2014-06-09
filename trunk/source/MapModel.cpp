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

#include "MapModel.h"

#include "GameState.h"
#include "Ghost.h"
#include "MapZoom.h"
#include "Audio.h"

#include "IwRandom.h"
#include "s3eTimer.h"

void restartGame();
void respawnGhost();

static MapZoom* mapZoom;
static GhostType lastGhostType;

static int lastLoses, lastWins;

void MapModelInit() {
	IwRandSeed((int32)s3eTimerGetMs());

	mapZoom = new MapZoom();

	Player* player = getGameState()->getPlayer();
	lastLoses = player->getLoseCount();
	lastWins = player->getWinCount();
}

void MapModelTerm() {
	if (mapZoom)
		delete mapZoom;
}

bool MapModelUpdate() {

	Player* player = getGameState()->getPlayer();
	if (player->isDead()) {
		restartGame();
	}
	player->headingUpdate();

	if (lastLoses < player->getLoseCount() ||
		lastWins < player->getWinCount()) {
		lastLoses = player->getLoseCount();
		lastWins = player->getWinCount();

		Audio::PlayAmbientMap();
	}

	respawnGhost();

	MapZoom* mapZoom = getMapZoom();
	if (mapZoom != NULL)
		mapZoom->Update();

	return true;
}

void respawnGhost() {
	if (getGameState()->getGhost() == NULL) {
		IntroState introState = getGameState()->getIntroState();
		
		GhostType type;
		if (introState == INTRO_ATTACK) {
			type = GhostType::VIKING;
		} else {
			if (lastGhostType == GhostType::VIKING) {
				type = GhostType::SKELMAN;
			} else {
				type = GhostType::VIKING;
			}
		}

		lastGhostType = type;
		Ghost* ghost = new Ghost(type, getGameState()->getPlayer());
		getGameState()->setGhost(ghost);
	}
}

void restartGame() {
	getGameState()->getPlayer()->resurrect();
	getGameState()->introReset();
}

MapZoom* getMapZoom() {
	return mapZoom;
}
