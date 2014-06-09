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

#include <math.h>
#include <time.h>
#include "IwDebug.h"

#define MIN_STRIKE_TIME 100
#define MAX_STRIKE_TIME 600
#define REQUIRED_BLOW_STR -900
#define ACCEL_TO_START_STRKE 200.f

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
