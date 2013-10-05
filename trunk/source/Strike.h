/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <math.h>
#include <time.h>
#include "IwDebug.h"

#define MIN_STRIKE_TIME 150
#define MAX_STRIKE_TIME 250
#define REQUIRED_BLOW_STR -1400
#define ACCEL_TO_START_STRKE 500.f

class Strike {
	private:
	bool striking;
	clock_t strikeInitTime;

	float peakAccel;
	bool accelDecreasing;
	clock_t accelDecreasingTime;

	public:
	Strike();
	int strikeUpdate(int32 x, int32 y, int32 z);
};
