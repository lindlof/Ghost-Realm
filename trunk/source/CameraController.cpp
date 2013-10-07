/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"
#include "CameraView.h"

#include "s3eCompass.h"
#include "s3e.h"

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
    if (s3eAccelerometerStart() == S3E_RESULT_SUCCESS)
    {
        accelometerEnabled = true;
    }
	if (s3eCompassAvailable())
    {
        s3eCompassStart();
    }

	s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)SingleTouch, NULL);
}

void CameraControllerTerm()
{
    s3eAccelerometerStop();
	if (s3eCompassAvailable()) 
	{
        s3eCompassStop();
	}
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

	compassUpdate(compassHeading, compassError);

    return true;
}

void SingleTouch(s3ePointerEvent* event) {
	ghostClick(event->m_x, event->m_y);
}
