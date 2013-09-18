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

static bool g_Enabled = false;

void CameraControllerInit()
{
    if (s3eAccelerometerStart() == S3E_RESULT_SUCCESS)
    {
        g_Enabled = true;
    }
}

void CameraControllerTerm()
{
    s3eAccelerometerStop();
}

bool CameraControllerUpdate()
{
    if (g_Enabled)
    {
		int32 x = s3eAccelerometerGetX();
		int32 y = s3eAccelerometerGetY();
		int32 z = s3eAccelerometerGetZ();

		accelometerUpdate(x, y, z);
    }

    return true;
}
