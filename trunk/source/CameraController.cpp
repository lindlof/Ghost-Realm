/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"
#include "s3eAccelerometer.h"
#include <math.h>
#include "s3eCompass.h"

static bool accelometerEnabled = false;

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
