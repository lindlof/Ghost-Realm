/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GameState.h"
#include "MapView.h"

#include "s3e.h"
#include "s3eCompass.h"

void MapSingleTouch(s3ePointerEvent* event);

void MapControllerInit() {
	if (s3eCompassAvailable())
    {
        s3eCompassStart();
    }

	s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)MapSingleTouch, NULL);
}

void MapControllerTerm() {
	if (s3eCompassAvailable()) 
	{
        s3eCompassStop();
	}
}

bool MapControllerUpdate() {

	s3eCompassHeading h;
	bool compassError = S3E_RESULT_SUCCESS != s3eCompassGetHeading(&h);
	double compassHeading = h.m_Heading;

	getGameState()->getPlayer()->compassUpdate(compassHeading, compassError);

	return true;
}

void MapSingleTouch(s3ePointerEvent* event) {
	if (getGameState()->getGameMode() == MAP_MODE) {
		MapRoamingGhost* mapGhost = getRoamingGhost();

		mapGhost->Touch(event->m_x, event->m_y, event->m_Pressed);
	}
}
