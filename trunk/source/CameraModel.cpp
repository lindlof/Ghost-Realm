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

	getGameState()->getGhost()->floatingAngleUpdate(gravityX, gravityY, gravityZ);

	if (iteration < ACCELOMETER_ITERATIONS) {
		// Accelometer values are useless before the gravity values
		// have some time to adjust.
		iteration++;
		if (iteration == ACCELOMETER_ITERATIONS)
			IwTrace(GHOST_HUNTER, ("CameraModel accelometer initialized"));
	} else {
		getGameState()->getPlayer()->accelometerUpdate(linearAccelerationX, linearAccelerationY, 
			linearAccelerationZ);
	}
}

void CameraModelInit() 
{
	fightTutorial = new FightTutorial();
}

void CameraModelTerm() 
{
	if (fightTutorial)
		delete fightTutorial;
}

void initFight() {
	fightTutorial->resetShown();
}

bool CameraModelUpdate() 
{
	GameState* gameState = getGameState();
	Player* player = gameState->getPlayer();
	Ghost* ghost = gameState->getGhost();

	int ectoplasm = gameState->getGhost()->getEctoplasm();
	if (ectoplasm <= 0) {
		player->wonBattle();
		gameState->deleteGhost();
		gameState->setGameMode(MAP_MODE);
	} else if (player->getMana() <= 0 && !player->isDead()) {
		player->lostBattle();
		gameState->deleteGhost();
		gameState->setGameMode(MAP_MODE);
	}

	if (!gameIsHalt()) {
		if (ghost != NULL)
			ghost->ghostUpdate();
	}

	player->headingUpdate();

	fightTutorial->triggerTutorial(TUTORIAL_SEARCH);

	return true;
}

FightTutorial* getFightTutorial() {
	return fightTutorial;
}

bool gameIsHalt() {
	return fightTutorial->isTutorialOn();
}
