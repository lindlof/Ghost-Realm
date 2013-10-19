/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "s3e.h"
#include "GhostCollision.h"
#include "CameraDefend.h"

#define GHOST_HIT_LENGTH 500
#define GHOST_HIT_LIGHT 0xf0 // 0x00 - 0xff

void CameraViewInit();
void CameraViewTerm();
bool CameraViewUpdate();

void ghostTouch(int32 x, int32 y);

void getCameraDefend(CameraDefend** cameraDefend);
