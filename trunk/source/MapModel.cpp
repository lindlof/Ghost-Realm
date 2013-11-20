/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GameState.h"
#include "Ghost.h"
#include "MapZoom.h"

#include "IwRandom.h"
#include "s3eTimer.h"

void respawnGhost();

MapZoom* mapZoom;
GhostType lastGhostType;

void MapModelInit() {
	IwRandSeed((int32)s3eTimerGetMs());

	mapZoom = new MapZoom();
}

void MapModelTerm() {
	if (mapZoom)
		delete mapZoom;
}

bool MapModelUpdate() {

	respawnGhost();

	Player* player = getGameState()->getPlayer();
	if (player->isDead()) {
		player->resurrect();
	}
	player->headingUpdate();

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

MapZoom* getMapZoom() {
	return mapZoom;
}
