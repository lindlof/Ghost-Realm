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

#include "Strike.h"
#include <math.h>

Strike::Strike() {
}

// Returs the hit
//
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

int Strike::strikeUpdate(int32 x, int32 y, int32 z) {
	int i;

	float phoneAccel = (float)sqrt(z*z + y*y);
	phoneAccel = (z+y) < 0 ? phoneAccel*-1 : phoneAccel*1;

	if (phoneAccel < -200 || phoneAccel > 200) 
		IwTrace(GHOST_HUNTER, ("Strike x: %d y: %d z: %d accel: %f", x, y, z, phoneAccel));

	// 1) Should the strike init?
	if (!striking && phoneAccel > ACCEL_TO_START_STRKE) {
		striking = true;
			
		IwTrace(GHOST_HUNTER, ("Strike started with accel %f", phoneAccel));

		// Initialize variables
		strikeInitTime = clock();
		peakAccel = 0;
		accelDecreasing = false;
	}

	int timeUsed = clock() - strikeInitTime;

	if (striking) {
		IwTrace(GOHU, ("accel %f", phoneAccel));

		if (timeUsed < MIN_STRIKE_TIME && z + y < ACCEL_TO_START_STRKE/2 && (x < 100 && y < 100)) {
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
			striking = false;
			return 1;
		} else {
			IwTrace(GHOST_HUNTER, ("The blow was too weak with accel %f", 
				phoneAccel));
		}
	}

	return 0;
}
