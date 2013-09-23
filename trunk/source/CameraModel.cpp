/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"
#include "Main.h"

#include <math.h>
#include <time.h>
#include "IwDebug.h"


static Player *player;
static Ghost *ghost;

Ghost getGhost() {
	return *ghost;
}

Player getPlayer() {
	return *player;
}

class Strike {
	bool striking;
	clock_t strikeInitTime;

	float peakAccel;
	bool accelDecreasing;
	clock_t accelDecreasingTime;

	float accelToStartStrike;
	const static int MIN_STRIKE_TIME = 150;
	const static int MAX_STRIKE_TIME = 250;
	const static int REQUIRED_BLOW_STR = -1400;

	public : Strike() {
		accelToStartStrike = 700;
	};

	// 1) When the phone is going forward and there is certain
	//    amount of acceleration the strike mode is initiated.
	//
	// 2) The strike must end in given minimum and maximum time.
	//    Normal strike don't take very long but neither should
	//    shaking of the phone be considered as a strike.
	//
	//    The strike is immediately discarded if phone starts
	//    going backward before the minimum time is reached.
	//
	// 3) The end phase begins when the phone acceleration
	//    starts decreasing. (Please note the effect of the
	//    low-pass filter.) If acceleration decreases rapidly
	//    enough the strike is considered successful.
	//
	// 4) When a ghost is hit tell it to the ghost.

	public : void strikeUpdate(int32 x, int32 y, int32 z) {
		int i;

		float phoneAccel = (float)sqrt(z*z + y*y);
		phoneAccel = z < 0 ? phoneAccel*-1 : phoneAccel*1;

		//if (phoneAccel < -200 || phoneAccel > 200) 
		//	IwTrace(GHOST_HUNTER, ("Strike x: %d y: %d z: %d accel: %f", x, y, z, phoneAccel));

		// 1) Should the strike init?
		if (!striking && phoneAccel > accelToStartStrike) {
			striking = true;
			
			IwTrace(GHOST_HUNTER, ("Strike started with accel %f", phoneAccel));

			// Initialize variables
			strikeInitTime = clock();
			peakAccel = 0;
			accelDecreasing = false;
		}

		int timeUsed = clock() - strikeInitTime;

		if (striking) {
			if (timeUsed < MIN_STRIKE_TIME && z < 0) {
				// 2) Is the phone already slowing or going backward?
				striking = false;
				IwTrace(GHOST_HUNTER, ("Strike ended too early"));
			} else if (timeUsed > MAX_STRIKE_TIME) {
				// 2) Is the time up?
				IwTrace(GHOST_HUNTER, ("Strike timed out"));
				striking = false;
			}
		}

		
		if (striking && timeUsed >= MIN_STRIKE_TIME) {
			peakAccel = phoneAccel > peakAccel ? phoneAccel : peakAccel;
			if (phoneAccel < peakAccel - 20) {
				// 3) Acceleration is decreasing
				if (!accelDecreasing) {
					// 3) Initialize the end phase
					accelDecreasing = true;
					accelDecreasingTime = clock();
				}
			}
			IwTrace(GOHU, ("Peak accel %f", peakAccel));

			if (phoneAccel < REQUIRED_BLOW_STR) {
				// If the final stop/blow of the strike is strong enough
				// accept the hit
				IwTrace(GHOST_HUNTER, ("Final phone accel %f", phoneAccel));
				ghost->ghostGotHit();
				striking = false;
			} else {
				IwTrace(GHOST_HUNTER, ("The blow was too weak with accel %f", 
					phoneAccel));
			}
		}
	}
};

static Strike strike;

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
		strike.strikeUpdate(linearAccelerationX, linearAccelerationY, 
		linearAccelerationZ);
	}
}

void compassUpdate(double heading, bool error) {
	ghost->compassUpdate(heading, error);
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
	ghost->ghostUpdate();
	return true;
}
