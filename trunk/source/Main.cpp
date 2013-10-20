/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Main.h"

#include <time.h>

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"
#include "IwAnim.h"

static Game *game;

void gameModeChanged();

int main() {

	IwGxInit();
	IwGraphicsInit();
	IwAnimInit();

#ifdef IW_BUILD_RESOURCES
	// Build resources in camera view
	CameraViewInit();
	CameraViewTerm();
#endif

	game = new Game();
	game->setGameMode(MAP_MODE);

    while (1) {
        s3eDeviceYield(0);

        int start = clock();

		if (game->getLastGameMode() != game->getGameMode()) {
			gameModeChanged();
			game->updateLastGameMode();
		}

		if (game->getGameMode() == CAMERA_MODE) {
			CameraControllerUpdate();
			CameraModelUpdate();
			CameraViewUpdate();
		} else if (game->getGameMode() == MAP_MODE) {
			MapControllerUpdate();
			MapModelUpdate();
			MapViewUpdate();
		}
		
		// Cap FPS
		int yield;
        do {
            yield = FRAME_DURATION - (clock() - start);
            if (yield > 0) {
				s3eDeviceYield(yield);
			}
        } while (yield > 0);
    }

	CameraControllerTerm();
	CameraModelTerm();
    CameraViewTerm();

	MapControllerTerm();
	MapModelTerm();
	MapViewTerm();

	delete game;

	IwAnimTerminate();
	IwGraphicsTerminate();
    IwGxTerminate();

    return 0;
}

void gameModeChanged() {
	if (game->getLastGameMode() == CAMERA_MODE) {
		CameraControllerTerm();
		CameraModelTerm();
		CameraViewTerm();
	} else if (game->getLastGameMode() == MAP_MODE) {
		MapControllerTerm();
		MapModelTerm();
		MapViewTerm();
	}

	if (game->getGameMode() == CAMERA_MODE) {
		CameraControllerInit();
		CameraModelInit();
		CameraViewInit();
	} else if (game->getGameMode() == MAP_MODE) {
		MapControllerInit();
		MapModelInit();
		MapViewInit();
	}
}

Game::Game() {
	Game::gameMode = NO_GAMEMODE;
}

void Game::setGameMode(GameMode gameMode) {
	Game::lastGameMode = Game::gameMode;
	Game::gameMode = gameMode;
}

void Game::updateLastGameMode() {
	Game::lastGameMode = Game::gameMode;
}

GameMode Game::getGameMode() {
	return gameMode;
}

GameMode Game::getLastGameMode() {
	return lastGameMode;
}
