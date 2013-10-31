/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GameState.h"
#include "CameraModel.h"
#include "GhostType.h"
#include "Ghost.h"

#include <math.h>
#include <time.h>
#include "IwDebug.h"

#define ACCELOMETER_ITERATIONS 40
int iteration = 0;
float gravityX, gravityY, gravityZ;

bool fightOver;
clock_t gameOverTime;
bool fightViewInit;

FightTutorial* fightTutorial;

void accelometerUpdate(int32 x, int32 y, int32 z) {

	// alpha is calculated as t / (t + dT)
    // with t, the low-pass filter's time-constant
    // and dT, the event delivery rate

	float alpha = 0.8f;

	// Use low-pass filter to isolate the force of gravity
	gravityX = alpha * gravityX + (1 - alpha) * x;
	gravityY = alpha * gravityY + (1 - alpha) * y;
	gravityZ = alpha * gravityZ + (1 - alpha) * z;

	float linearAccelerationX = x - gravityX;
	float linearAccelerationY = y - gravityY;
	float linearAccelerationZ = z - gravityZ;

	if (getGameState()->getGhost() != NULL)
		getGameState()->getGhost()->floatingAngleUpdate(gravityX, gravityY, gravityZ);

	if (iteration < ACCELOMETER_ITERATIONS) {
		// Accelometer values are useless before the gravity values
		// have some time to adjust.
		iteration++;
		if (iteration == ACCELOMETER_ITERATIONS)
			IwTrace(GHOST_HUNTER, ("CameraModel accelometer initialized"));
	} else {
		if (!fightOver) {
			getGameState()->getPlayer()->accelometerUpdate(linearAccelerationX, linearAccelerationY, 
				linearAccelerationZ);
		}
	}
}

void CameraModelInit() 
{
	fightTutorial = new FightTutorial();
	fightViewInit = true;
}

void CameraModelTerm() 
{
	if (fightTutorial)
		delete fightTutorial;
}

void initFight() {
	fightTutorial->resetShown();
	fightOver = false;
	getGameState()->getGhost()->cameraInit();
	fightViewInit = true;
}

bool CameraModelUpdate() 
{
	GameState* gameState = getGameState();
	Player* player = gameState->getPlayer();
	Ghost* ghost = gameState->getGhost();

	if (ghost != NULL) {
		ghost->ghostUpdate();

		int ectoplasm = ghost->getEctoplasm();
		if (ectoplasm <= 0) {
			player->wonBattle();
			gameState->deleteGhost();
			fightOver = true;
			gameOverTime = clock() + 10000;
			getFightTutorial()->triggerTutorial(TUTORIAL_YOU_WON);
		} else if (player->getMana() <= 0) {
			player->lostBattle();
			gameState->deleteGhost();
			fightOver = true;
			gameOverTime = clock() + 10000;

			if (player->isDead()) {
				fightTutorial->triggerTutorial(TUTORIAL_YOU_DIED);
				getGameState()->getPlayer()->resurrect();
			} else {
				getFightTutorial()->triggerTutorial(TUTORIAL_GHOST_WON);
			}
		}
	}

	if (fightOver && clock() > gameOverTime) {
		gameState->setGameMode(MAP_MODE);
	}

	fightTutorial->triggerTutorial(TUTORIAL_SEARCH);

	player->headingUpdate();

	return true;
}

FightTutorial* getFightTutorial() {
	return fightTutorial;
}

bool gameIsHalt() {
	return fightTutorial->isTutorialOn();
}

bool viewFightInitRequired() {
	bool init = fightViewInit;
	fightViewInit = false;
	return init;
}
