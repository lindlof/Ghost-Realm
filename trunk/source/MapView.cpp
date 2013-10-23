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

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"
#include "Iw2D.h"

void renderMap();
void renderPlayer();

static FightButton* fightButton;

static CIwTexture* ghostTexture;
static CIw2DImage* playerTexture;
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

double inline rad(double d) {
    return d / 180.0f * PI;
}

double inline deg(double d) {
    return d / PI * 180.0f;
}

void MapViewInit()
{	
	Iw2DInit();

	ghostTexture = new CIwTexture;
	ghostTexture->LoadFromFile ("textures/map_ghost.png");
	ghostTexture->Upload();

	playerTexture = Iw2DCreateImage("textures/map_player.png");
	
	mapTexture = new CIwTexture;
	mapTexture->LoadFromFile ("textures/map_template.png");
	mapTexture->Upload();
	
	mapInit(mapTexture->GetWidth(), mapTexture->GetHeight());

	fightButton = new FightButton();
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

	if (ghostTexture)
		delete ghostTexture;

	if (playerTexture)
		delete playerTexture;

	if (mapTexture)
		delete mapTexture;

	if (fightButton)
		delete fightButton;

	Iw2DTerminate();
}

bool MapViewUpdate() {

	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	renderMap();
	renderPlayer();

	fightButton->Render();

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

void renderPlayer() {
	IwGxLightingOff();

	double angle = rad(getGameState()->getPlayer()->getHeading());

	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);
	CIwFVec2 centre =
			CIwFVec2((int16)Iw2DGetSurfaceWidth()/2,
					 (int16)Iw2DGetSurfaceHeight()/2);
	CIwFMat2D rot;
	rot.SetRot(angle, (CIwFVec2)centre);
	Iw2DSetTransformMatrix(rot);

	CIwFVec2 size = CIwFVec2(41, 47);
	CIwFVec2 topLeft = CIwFVec2(centre.x-size.x/2, centre.y-size.y/2);

	Iw2DDrawImage(playerTexture, topLeft, size);
}

FightButton* getFightButton() {
	return fightButton;
}
