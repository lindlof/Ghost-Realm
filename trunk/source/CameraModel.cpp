/* ========================================================================= *
 * Ghost Hunter Project                                                      *
 * https://ghosthuntermobile.wordpress.com/                                  *
 * ========================================================================= *
 * Copyright (C) 2013 Ghost Hunter Project                                   *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 * ========================================================================= */

#include "GameState.h"
#include "CameraModel.h"
#include "GhostType.h"
#include "Ghost.h"
#include "Audio.h"

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
	Audio::PlayAmbientCamera();

	getGameState()->getPlayer()->initFight();

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

		if (!fightOver && ghost->isDead()) {
			Audio::StopAmbient();
			player->wonBattle();
			fightOver = true;
			gameOverTime = clock() + 10000;
			getFightTutorial()->triggerTutorial(TUTORIAL_YOU_WON);
			gameState->introProceed();
			player->increaseWinCount();
		} else if (!fightOver && player->getMana() <= 0) {
			Audio::StopAmbient();
			player->lostBattle();
			fightOver = true;
			gameOverTime = clock() + 10000;

			player->increaseLoseCount();
			if (player->isDead()) {
				player->increaseDeadCount();
				fightTutorial->triggerTutorial(TUTORIAL_YOU_DIED);
			} else {
				getFightTutorial()->triggerTutorial(TUTORIAL_GHOST_WON);
			}
		}
	}

	fightTutorial->triggerTutorial(TUTORIAL_SEARCH);

	player->headingUpdate();

	if (fightOver && clock() > gameOverTime) {
		fightTutorial->tutorialAcknowledged();
	}

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
