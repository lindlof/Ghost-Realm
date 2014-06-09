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
#include "MapView.h"
#include "MapModel.h"

#include "s3e.h"
#include "s3eCompass.h"

void MapSingleTouch(s3ePointerEvent* event);
void MapMultiTouch(s3ePointerTouchEvent* event);
void MapMultiTouchMotion(s3ePointerTouchMotionEvent* event);

void MapControllerInit() {
	if (s3eCompassAvailable())
    {
        s3eCompassStart();
    }

	s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)MapSingleTouch, NULL);

	s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MapMultiTouch, NULL);
    s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MapMultiTouchMotion, NULL);
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
		MapZoom* mapZoom = getMapZoom();

		bool ghostTouched = false;
		if (mapGhost != NULL)
			ghostTouched = mapGhost->Touch(event->m_x, event->m_y, event->m_Pressed);

		if (mapZoom != NULL)
				mapZoom->Touch(event->m_x, event->m_y, event->m_Pressed, ghostTouched);
	}
}

void MapMultiTouch(s3ePointerTouchEvent* event) {
	if (getGameState()->getGameMode() == MAP_MODE) {
		MapZoom* mapZoom = getMapZoom();
		MapRoamingGhost* mapGhost = getRoamingGhost();

		if (mapZoom != NULL)
			mapZoom->Touch(event->m_x, event->m_y, event->m_Pressed, event->m_TouchID);

		if (mapGhost != NULL)
			mapGhost->Touch(event->m_TouchID);
	}
}

void MapMultiTouchMotion(s3ePointerTouchMotionEvent* event)
{
	if (getGameState()->getGameMode() == MAP_MODE) {
		MapZoom* mapZoom = getMapZoom();

		if (mapZoom != NULL)
				mapZoom->Motion(event->m_x, event->m_y, event->m_TouchID);
	}
}
