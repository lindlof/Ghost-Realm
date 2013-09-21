/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Ghost.h"

#include "IwRandom.h"
#include "s3eTimer.h"
#include "s3eCompass.h"
#include "IwDebug.h"

static bool compassError;
static double currentCompassHeading = 0;

Ghost::Ghost() {
	positionX = 0;
	positionY = 0;
	width = 150;
	height = 300;
	hit = false;

	staging = true;
	IwRandSeed((int32)s3eTimerGetMs());
	compassPoint = IwRandMinMax(0, 360);
};

void Ghost::ghostGotHit() {
	IwTrace(GHOST_HUNTER, ("Ghost got hit"));
	hit = true;
}

bool Ghost::isGhostHit() {
	return hit;
}

void GhostInit() {
	if (s3eCompassAvailable())
    {
        s3eCompassStart();
    }
}

bool GhostUpdate()
{
	s3eCompassHeading h;
	compassError = S3E_RESULT_SUCCESS != s3eCompassGetHeading(&h);
	currentCompassHeading = h.m_Heading;

    return true;
}

void GhostTerm()
{
	if (s3eCompassAvailable()) 
	{
        s3eCompassStop();
	}
}
