/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

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
	Audio::PlayAmbientMap();
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
