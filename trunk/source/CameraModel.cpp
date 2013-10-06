/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"

#include <math.h>
#include <time.h>
#include "IwDebug.h"


static Player *player;
static Ghost *ghost;

Ghost* getGhost() {
	return ghost;
}

Player* getPlayer() {
	return player;
}

#define ACCELOMETER_ITERATIONS 40
int iteration = 0;
float gravityX, gravityY, gravityZ;

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

	if (iteration < ACCELOMETER_ITERATIONS) {
		// Accelometer values are useless before the gravity values
		// have some time to adjust.
		iteration++;
		if (iteration == ACCELOMETER_ITERATIONS)
			IwTrace(GHOST_HUNTER, ("CameraModel accelometer initialized"));
	} else {
		player->accelometerUpdate(linearAccelerationX, linearAccelerationY, 
			linearAccelerationZ);
		ghost->floatingUpdate(linearAccelerationX, linearAccelerationY, 
			linearAccelerationZ);
	}
}

void compassUpdate(double heading, bool error) {
	player->compassUpdate(heading, error);
}

void CameraModelInit() 
{
	player = new Player();
	ghost = new Ghost(GHOST_NORMAL, player);
}

void CameraModelTerm() 
{
	delete player;
	delete ghost;
}

bool CameraModelUpdate() 
{
	int ectoplasm = ghost->getEctoplasm();
	if (ectoplasm < 0) {
		delete ghost;
		player->setGhost(NULL);
		ghost = new Ghost(GHOST_NORMAL, player);
	}

	ghost->ghostUpdate();
	player->playerUpdate();
	return true;
}
