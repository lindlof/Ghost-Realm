/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "FightButton.h"

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
	fightTexture->LoadFromFile ("textures/map_fight.png");
	fightTexture->Upload();

	{
		int16 w = IwGxGetScreenWidth() * 0.7f;
		if (w > (int16)fightTexture->GetWidth()) w = (int16)fightTexture->GetWidth();

		float whScale = (float)((double)fightTexture->GetWidth() / fightTexture->GetHeight());
		int16 h = w * 1/whScale;
		int bottomPadding = IwGxGetScreenHeight() * 0.03f;

		int16 x1 = IwGxGetScreenWidth()/2 - w/2;
		int16 y1 = IwGxGetScreenHeight() - h - bottomPadding;

		buttonXY = CIwSVec2(x1, y1);
		buttonWH = CIwSVec2(w, h);
	}
}

FightButton::~FightButton() {
	if (fightTexture)
		delete fightTexture;
}

void FightButton::Render() {
	IwGxLightingOff();
	
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
    pMat->SetTexture(fightTexture);

	IwGxSetMaterial(pMat);
    IwGxSetUVStream(uvs);

	IwGxDrawRectScreenSpace(&buttonXY, &buttonWH);
}

void FightButton::Touch(int x, int y) {
	if (x > buttonXY.x && x < buttonXY.x + buttonWH.x &&
		y > buttonXY.y && y < buttonXY.y + buttonWH.y) {

		IwTrace(GHOST_HUNTER, ("Player wants to fight"));
	}
}
