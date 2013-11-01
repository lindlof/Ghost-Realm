/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "FightButton.h"
#include "GameState.h"
#include "CameraModel.h"

#include "IwDebug.h"
#include "IwGx.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"

static CIwFVec2 uvs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
    CIwFVec2(1, 1),
    CIwFVec2(1, 0)
};

FightButton::FightButton() {

	fightTexture = new CIwTexture;
	fightTexture->LoadFromFile("textures/from_map_to_camera.png");
	fightTexture->Upload();

	{
		int16 w = IwGxGetScreenWidth();

		float whScale = (float)((double)fightTexture->GetWidth() / fightTexture->GetHeight());
		int16 h = w * 1/whScale;

		buttonXY = CIwSVec2(0, IwGxGetScreenHeight()-h);
		buttonWH = CIwSVec2(w, h);
	}

	pressedTime = 0;
}

FightButton::~FightButton() {
	if (fightTexture)
		delete fightTexture;
}

void FightButton::Render() {
	{
		IwGxLightingOff();

		CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
		pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		pMat->SetTexture(fightTexture);

		IwGxSetMaterial(pMat);
		IwGxSetUVStream(uvs);

		IwGxDrawRectScreenSpace(&buttonXY, &buttonWH);
	}
}

void FightButton::Touch(int x, int y, bool pressed) {
	if (getGameState()->getGhost() == NULL) return;

	if (x > buttonWH.x*0.03 &&
		x < buttonWH.x*0.97 &&
		y > buttonXY.y + buttonWH.y*0.43 && 
		y < buttonXY.y + buttonWH.y*0.98) {

		if (pressed) {
			pressedTime = clock();
		}

		if (clock() - pressedTime < 1500 && !pressed) {
			IwTrace(GHOST_HUNTER, ("Player wants to fight"));
			initFight();
			getGameState()->setGameMode(CAMERA_MODE);
		}
	} else {
		pressedTime = 0;
	}
}
