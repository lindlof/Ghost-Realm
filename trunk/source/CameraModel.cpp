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
#include "CameraView.h"

class Strike {
	clock_t time;

	const static int SPEED_TO_START_STRIKING = 200;
	const static int STRINKE_TIME_TO_HIT = 100;

	// If player keeps the device in high acceleration for over
	// half second the ghost is considered to be attacked
	public : void strikeUpdate(int32 x, int32 y, int32 z) {
		float magnitude = (float)sqrt((x*x) + (y*y) + (z*z));

		float phoneAccel = sqrt(z^2 + y^2);

		// Striking starts when the phone moves 'forward' with
		// enough acceleration
		bool striking = (z > 0) && 
			phoneAccel > SPEED_TO_START_STRIKING;
		
		// TODO: Consider specific direction/higher acceleration to be rewarded differently
		if (striking) {

			// Striking is done when phone stops with enough acceleration
			if (z < 0 && phoneAccel > 200) {
				// Ghost is attacked if the strike took enough time

				if (clock() - time > STRINKE_TIME_TO_HIT) {
					setGhostAttacked(true);
				}
			}
		} else {
			time = clock();
			setGhostAttacked(false);
		}

	}
};

class Vitality {
	int vitality;


};

class Ghost {
	int positionX;
	int positionY;
	int sizeX;
	int sizeY;


};

static Strike strike;
static Vitality vitality;

float gravityX, gravityY, gravityZ = 0;

void accelometerUpdate(int32 x, int32 y, int32 z) {
	// alpha is calculated as t / (t + dT)
    // with t, the low-pass filter's time-constant
    // and dT, the event delivery rate

	float alpha = 0.8;

	// Use low-pass filter to isolate the force of gravity
	gravityX = alpha * gravityX + (1 - alpha) * x;
	gravityY = alpha * gravityY + (1 - alpha) * y;
	gravityZ = alpha * gravityZ + (1 - alpha) * z;

	float linearAccelerationX = x - gravityX;
	float linearAccelerationY = y - gravityY;
	float linearAccelerationZ = z - gravityZ;

	strike.strikeUpdate(x, y, z);
}

void CameraModelInit() {

}

void CameraModelUpdate() {

}
