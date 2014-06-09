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
#include "CameraView.h"

#include "s3e.h"
#include "s3eCompass.h"

static bool accelometerEnabled = false;
void SingleTouch(s3ePointerEvent* event);
void SingleTouchMotion(s3ePointerMotionEvent* event);
void MultiTouch(s3ePointerTouchEvent* event);
void MultiTouchMotion(s3ePointerTouchMotionEvent* event);


void CameraControllerInit()
{
	if (s3eCompassAvailable())
    {
        s3eCompassStart();
    }
    if (s3eAccelerometerStart() == S3E_RESULT_SUCCESS)
    {
        accelometerEnabled = true;
    }

	s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)SingleTouch, NULL);
	s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)SingleTouchMotion, NULL);

	s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouch, NULL);
    s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MultiTouchMotion, NULL);
}

void CameraControllerTerm()
{
	if (s3eCompassAvailable()) 
	{
        s3eCompassStop();
	}
    s3eAccelerometerStop();
}

bool CameraControllerUpdate()
{
	if (accelometerEnabled)
	{
		int32 x = s3eAccelerometerGetX();
		int32 y = s3eAccelerometerGetY();
		int32 z = s3eAccelerometerGetZ();

		accelometerUpdate(x, y, z);
	}

	s3eCompassHeading h;
	bool compassError = S3E_RESULT_SUCCESS != s3eCompassGetHeading(&h);
	double compassHeading = h.m_Heading;

	getGameState()->getPlayer()->compassUpdate(compassHeading, compassError);

    return true;
}

void SingleTouch(s3ePointerEvent* event) {
	if (getGameState()->getGameMode() == CAMERA_MODE) {
		if (!gameIsHalt()) {
			ghostTouch(event->m_x, event->m_y);
		}

		getFightTutorialView()->Touch(event->m_x, event->m_y, event->m_Pressed);
	}
}

void SingleTouchMotion(s3ePointerMotionEvent* event) {
	
}

void MultiTouch(s3ePointerTouchEvent* event) {
	if (getGameState()->getGameMode() == CAMERA_MODE) {
		CameraDefend* cameraDefend = getCameraDefend();

		if (cameraDefend != NULL)
				cameraDefend->Touch(event->m_x, event->m_y, event->m_Pressed, event->m_TouchID);
	}
}

void MultiTouchMotion(s3ePointerTouchMotionEvent* event)
{
	if (getGameState()->getGameMode() == CAMERA_MODE) {
		if (!gameIsHalt()) {
			CameraDefend* cameraDefend = getCameraDefend();

			if (cameraDefend != NULL)
				cameraDefend->Motion(event->m_x, event->m_y, event->m_TouchID);
		}
	}
}
