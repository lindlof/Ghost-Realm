/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "IwGx.h"
#include <time.h>
#include "Main.h"

#include "ExamplesMain.h"
#include "s3e.h"
#include "s3eLocation.h"
#include "IwHTTP.h"
#include "IwImage.h"
#include "Iw2D.h"
#include "IwGxPrint.h"

static Game *game;

int main() {

    CameraViewInit();
	CameraControllerInit();
	CameraModelInit();

	game = new Game();
	game->setGameMode(CAMERA_MODE);
	
    while (1) {
        s3eDeviceYield(0);

        int start = clock();

		if (game->getGameMode() == CAMERA_MODE) {
			CameraControllerUpdate();
			CameraModelUpdate();
			CameraViewUpdate();
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

    CameraViewTerm();
	CameraControllerTerm();
	CameraModelTerm();

	game = new Game();
	game->setGameMode(MAP_MODE);

	while(1){
		s3eDeviceYield(0);
		int start = clock();
		if(game->getGameMode() == MAP_MODE){
			CameraControllerUpdate();
			CameraModelUpdate();
			CameraViewUpdate();
		}

		int yield;
		do {
			yield = FRAME_DURATION - (clock()-start);
			if(yield > 0){
				s3eDeviceYield(yield);
			}
		}while(yield >0);
	}


	delete game;

    return 0;
}
	


void Game::setGameMode(GameMode gameMode) {
	Game::gameMode = gameMode;
}

GameMode Game::getGameMode() {
	return gameMode;
}
