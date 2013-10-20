/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"
#include "CameraController.h"
#include "CameraView.h"

#include "MapModel.h"
#include "MapController.h"
#include "MapView.h"

// 24 FPS limit
#define FRAME_DURATION 1000/24

enum GameMode { NO_GAMEMODE, CAMERA_MODE, MAP_MODE };
class Game {
	private:
	GameMode gameMode;
	GameMode lastGameMode;

	public:
	Game();
	void setGameMode(GameMode gameMode);
	void updateLastGameMode();
	GameMode getGameMode();
	GameMode getLastGameMode();
};

Game* getGame();
