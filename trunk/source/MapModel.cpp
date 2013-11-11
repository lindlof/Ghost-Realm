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

#include "IwRandom.h"
#include "s3eTimer.h"

void MapModelInit() {
	IwRandSeed((int32)s3eTimerGetMs());
}

void MapModelTerm() {

}

bool MapModelUpdate() {

	if (getGameState()->getGhost() == NULL) {
		int rand = IwRandMinMax(0, 2);
		GhostType type;

		if (rand == 0) type = GhostType::VIKING;
		else if (rand == 1) type = GhostType::SKELMAN;

		Ghost* ghost = new Ghost(type, getGameState()->getPlayer());
		getGameState()->setGhost(ghost);
	}

	getGameState()->getPlayer()->headingUpdate();

	return true;
}
