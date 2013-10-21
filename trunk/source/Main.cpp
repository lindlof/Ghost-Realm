/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Main.h"
#include "GameState.h"

#include <time.h>

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"
#include "IwAnim.h"

int main() {

	IwGxInit();
	IwGraphicsInit();
	IwAnimInit();

	// Set screen clear colour
    IwGxSetColClear(0x0, 0x0, 0x0, 0xff);

	MapControllerInit();
	MapModelInit();
	MapViewInit();

	CameraControllerInit();
	CameraModelInit();
	CameraViewInit();

	getGameState()->setGameMode(MAP_MODE);

    while (1) {
        s3eDeviceYield(0);

        int start = clock();

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

	IwAnimTerminate();
	IwGraphicsTerminate();
    IwGxTerminate();

    return 0;
}
