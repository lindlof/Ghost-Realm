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

	// If player keeps the device in high acceleration for over
	// half second the ghost is considered to be attacked
	public : void strikeUpdate(int32 x, int32 y, int32 z) {
		// Magnitude of the acceleration without gravity
		float mag = (float)sqrt((x*x) + (y*y) + (z*z)) - 950;

		bool striking = mag > 100;

		// TODO: Consider specific direction/higher acceleration to be rewarded differently
		if (striking) {
			if (clock() - time > 500) {
				setGhostAttacked(true);
			}
		} else {
			time = clock();
			setGhostAttacked(false);
		}

	}
};

static Strike strike;

void accelometerUpdate(int32 x, int32 y, int32 z) {
	strike.strikeUpdate(x, y, z);
}
