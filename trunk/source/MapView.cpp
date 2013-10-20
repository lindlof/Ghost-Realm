/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"

void renderMap();
void renderFight();

static CIwTexture* fightTexture;

static CIwTexture* ghostTexture;
static CIwTexture* playerTexture;
static CIwTexture* mapTexture;

void mapInit(int mapW, int mapH);
static CIwFVec2 mapVert[4];

static CIwFVec2 mapDefaultUvs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
    CIwFVec2(1, 1),
    CIwFVec2(1, 0)
};

void MapViewInit()
{
	// Set screen clear colour
    IwGxSetColClear(0xff, 0xff, 0xff, 0xff);
    IwGxPrintSetColour(128, 128, 128);

	fightTexture = new CIwTexture;
	fightTexture->LoadFromFile ("textures/map_fight.png");
	fightTexture->Upload();
	
	ghostTexture = new CIwTexture;
	ghostTexture->LoadFromFile ("textures/map_ghost.png");
	ghostTexture->Upload();

	playerTexture = new CIwTexture;
	playerTexture->LoadFromFile ("textures/map_player.png");
	playerTexture->Upload();
	
	/*default_uvs[0] = CIwFVec2(0, 0);
    default_uvs[1] = CIwFVec2(0, 1);
    default_uvs[2] = CIwFVec2(1, 1);
    default_uvs[3] = CIwFVec2(1, 0);*/
	
	mapTexture = new CIwTexture;
	mapTexture->LoadFromFile ("textures/map_template.png");
	mapTexture->Upload();
	
	mapInit(mapTexture->GetWidth(), mapTexture->GetHeight());
}

void mapInit(int mapW, int mapH) {
	double w = IwGxGetScreenWidth();
	double h = IwGxGetScreenHeight();
	{
		double whRatio = ((double)mapW) / mapH;

		if (whRatio > w/h) {
			w = h * whRatio;
		} else if (whRatio < w/h) {
			h = w * 1/whRatio;
		}
	}

	// Full screen screenspace vertex coords
	int16 x1 = (int16)(-abs(w - IwGxGetScreenWidth())/2); // Negative or 0
	int16 x2 = (int16)(IwGxGetScreenWidth()  - x1);
	int16 y1 = (int16)(-abs(h - IwGxGetScreenHeight())/2); // Negative or 0
	int16 y2 = (int16)(IwGxGetScreenHeight() - y1);
	
	mapVert[0].x = x1, mapVert[0].y = y1;
	mapVert[1].x = x1, mapVert[1].y = y2;
	mapVert[2].x = x2, mapVert[2].y = y2;
	mapVert[3].x = x2, mapVert[3].y = y1;
}

void MapViewTerm() {

	if (fightTexture)
		delete fightTexture;

	if (ghostTexture)
		delete ghostTexture;

	if (playerTexture)
		delete playerTexture;

	if (mapTexture)
		delete mapTexture;
}

bool MapViewUpdate() {

	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	renderMap();

	renderFight();

	IwGxFlush();
    IwGxSwapBuffers();

	return true;
}

void renderMap() {
	IwGxLightingOff();
	
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    pMat->SetTexture(mapTexture);

	IwGxSetMaterial(pMat);

	IwGxSetScreenSpaceSlot(-1);

    IwGxSetUVStream(mapDefaultUvs);
    IwGxSetVertStreamScreenSpace(mapVert, 4);

    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void renderFight() {
	IwGxLightingOff();
	
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
    pMat->SetTexture(fightTexture);

	int16 w = IwGxGetScreenWidth() * 0.7f;
	if (w > (int16)fightTexture->GetWidth()) w = (int16)fightTexture->GetWidth();

	float whScale = (float)((double)fightTexture->GetWidth() / fightTexture->GetHeight());
	int16 h = w * 1/whScale;
	int bottomPadding = IwGxGetScreenHeight() * 0.03f;

	int16 x1 = IwGxGetScreenWidth()/2 - w/2;
    int16 y1 = IwGxGetScreenHeight() - h - bottomPadding;

	IwGxSetMaterial(pMat);
    IwGxSetUVStream(mapDefaultUvs);

    CIwSVec2 XY(x1, y1), dXY(w, h);
	IwGxDrawRectScreenSpace(&XY, &dXY);
}
