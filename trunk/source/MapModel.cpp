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

void MapModelInit() {

}

void MapModelTerm() {

}

bool MapModelUpdate() {

	if (getGameState()->getGhost() == NULL) {
		Ghost* ghost = new Ghost(GhostType::VIKING, getGameState()->getPlayer());
		getGameState()->setGhost(ghost);
	}

	getGameState()->getPlayer()->playerUpdate();

	return true;
}
