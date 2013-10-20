/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "MapView.h"

#include "s3eCompass.h"
#include "s3e.h"

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
	return true;
}

void MapSingleTouch(s3ePointerEvent* event) {

	FightButton* fightButton = getFightButton();

	fightButton->Touch(event->m_x, event->m_y);
}
