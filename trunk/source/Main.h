/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

// 24 FPS limit
#define FRAME_DURATION 1000/24

enum GameMode { CAMERA_MODE };
class Game {
	private:
	GameMode gameMode;

	public:
	void setGameMode(GameMode gameMode);
	GameMode getGameMode();
};

void CameraViewInit();
void CameraViewTerm();
bool CameraViewUpdate();

void CameraControllerInit();
void CameraControllerTerm();
bool CameraControllerUpdate();

void CameraModelInit();
void CameraModelTerm();
bool CameraModelUpdate();
