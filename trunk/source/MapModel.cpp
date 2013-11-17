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

MapZoom* mapZoom;

void MapModelInit() {
	IwRandSeed((int32)s3eTimerGetMs());

	mapZoom = new MapZoom();
}

void MapModelTerm() {
	if (mapZoom)
		delete mapZoom;
}

bool MapModelUpdate() {

	if (getGameState()->getGhost() == NULL) {
		IntroState introState = getGameState()->getIntroState();
		
		GhostType type;
		if (introState == INTRO_ATTACK) {
			type = GhostType::VIKING;
		} else {
			type = GhostType::SKELMAN;
		}

		Ghost* ghost = new Ghost(type, getGameState()->getPlayer());
		getGameState()->setGhost(ghost);
	}

	Player* player = getGameState()->getPlayer();
	if (player->isDead()) {
		player->resurrect();
	}
	player->headingUpdate();

	return true;
}

MapZoom* getMapZoom() {
	return mapZoom;
}
