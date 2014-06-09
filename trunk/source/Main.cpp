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

#include "Main.h"
#include "GameState.h"
#include "Audio.h"
#include "Splash.h"

#include <time.h>

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"
#include "IwAnim.h"
#include "Iw2D.h"

int main() {

	IwGxInit();
	IwGraphicsInit();
	IwAnimInit();
	Iw2DInit();

	IwTrace(GHOST_HUNTER, ("Ghost Realm booting..."));
	IwTrace(GHOST_HUNTER, ("Audio channels: %d", s3eAudioGetInt(S3E_AUDIO_NUM_CHANNELS)));

	// Set screen clear colour
    IwGxSetColClear(0x0, 0x0, 0x0, 0xff);

	Splash* splash = new Splash();
	s3eDeviceYield(20);
	{ // Init game
		CameraViewPreInit();

		MapControllerInit();
		MapModelInit();
		MapViewInit();

		CameraControllerInit();
		CameraModelInit();
		CameraViewInit();
	}
	delete splash;

	getGameState()->setGameMode(MAP_MODE);

    while (1) {
        s3eDeviceYield(0);

        int start = clock();

		Audio::GetInstance().Update();

		if (getGameState()->getGameMode() == CAMERA_MODE) {
			CameraControllerUpdate();
			CameraModelUpdate();
			CameraViewUpdate();
		} else if (getGameState()->getGameMode() == MAP_MODE) {
			MapControllerUpdate();
			MapModelUpdate();
			MapViewUpdate();
		}
		
		// Cap FPS
		int yield;
        do {
            yield = FRAME_DURATION - (clock() - start);
            if (yield > 0) {
				s3eDeviceYield(yield);
			}
        } while (yield > 0);
    }

	CameraControllerTerm();
	CameraModelTerm();
    CameraViewTerm();

	MapControllerTerm();
	MapModelTerm();
	MapViewTerm();

	delete getGameState();

	Iw2DTerminate();
	IwAnimTerminate();
	IwGraphicsTerminate();
    IwGxTerminate();

    return 0;
}
