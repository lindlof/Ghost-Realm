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
#include "CameraView.h"

#include "s3e.h"
#include "s3eCompass.h"

struct Touch {
    int32 x; // position x
    int32 y; // position y
};

#define MAX_TOUCHES 1
Touch g_Touches[MAX_TOUCHES];

static bool accelometerEnabled = false;
void SingleTouch(s3ePointerEvent* event);
void SingleTouchMotion(s3ePointerMotionEvent* event);

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
	if (!gameIsHalt()) {
		if (accelometerEnabled)
		{
			int32 x = s3eAccelerometerGetX();
			int32 y = s3eAccelerometerGetY();
			int32 z = s3eAccelerometerGetZ();

			accelometerUpdate(x, y, z);
		}
	}

	s3eCompassHeading h;
	bool compassError = S3E_RESULT_SUCCESS != s3eCompassGetHeading(&h);
	double compassHeading = h.m_Heading;

	getGameState()->getPlayer()->compassUpdate(compassHeading, compassError);

    return true;
}

void SingleTouch(s3ePointerEvent* event) {
	if (!gameIsHalt()) {
		CameraDefend* cameraDefend;
	
		ghostTouch(event->m_x, event->m_y);

		getCameraDefend(&cameraDefend);
		if (cameraDefend != NULL)
			cameraDefend->Touch(event->m_x, event->m_y, event->m_Pressed);
	}

	getFightTutorial()->Touch(event->m_x, event->m_y);
}

void SingleTouchMotion(s3ePointerMotionEvent* event) {
	CameraDefend* cameraDefend;

	getCameraDefend(&cameraDefend);
	if (cameraDefend != NULL)
		cameraDefend->Motion(event->m_x, event->m_y);
}
