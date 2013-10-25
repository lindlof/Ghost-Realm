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
void renderMapGhost();
void renderMapPlayer();
void renderMapHealth();

static FightButton* fightButton;

static CIw2DImage* ghostTexture;
static CIw2DImage* playerTexture;
static CIw2DImage* healthTexture;
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
	ghostTexture = Iw2DCreateImage("textures/map_ghost.png");
	playerTexture = Iw2DCreateImage("textures/map_player.png");
	healthTexture = Iw2DCreateImage("textures/map_health.png");
	
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
}

bool MapViewUpdate() {

	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	renderMap();
	renderMapGhost();
	renderMapPlayer();
	renderMapHealth();

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

void renderMapGhost() {
	IwGxLightingOff();

	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	CIwFVec2 centre =
			CIwFVec2((int16)Iw2DGetSurfaceWidth()/2,
					 (int16)Iw2DGetSurfaceHeight()/2);

	CIwFVec2 size = CIwFVec2(259, 259);
	CIwFVec2 topLeft = CIwFVec2(centre.x-50-size.x/2, centre.y-50-size.y/2);

	Iw2DDrawImage(ghostTexture, topLeft, size);
}

void renderMapPlayer() {
	IwGxLightingOff();

	double angle = rad(getGameState()->getPlayer()->getHeading());

	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);
	CIwFVec2 centre =
			CIwFVec2((int16)Iw2DGetSurfaceWidth()/2,
					 (int16)Iw2DGetSurfaceHeight()/2);
	CIwFMat2D rot;
	rot.SetRot(angle, (CIwFVec2)centre);
	Iw2DSetTransformMatrix(rot);

	CIwFVec2 size = CIwFVec2(53, 57);
	CIwFVec2 topLeft = CIwFVec2(centre.x-size.x/2, centre.y-size.y/2);

	Iw2DDrawImage(playerTexture, topLeft, size);
}

FightButton* getFightButton() {
	return fightButton;
}

void renderMapHealth() {
	IwGxLightingOff();

	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	int16 w = (double)IwGxGetScreenWidth() * 0.11f;
	if (w > (int16)healthTexture->GetWidth()) w = (int16)healthTexture->GetWidth();

	float whScale = (float)((double)healthTexture->GetWidth() / healthTexture->GetHeight());
	int16 h = w * 1/whScale;

	int leftPadding = IwGxGetScreenWidth() * 0.20f;
	int topPadding = IwGxGetScreenHeight() * 0.02f;

	int leftMargin = IwGxGetScreenWidth() * 0.03f;
	int topMargin = IwGxGetScreenHeight() * 0.05f;

	CIwFVec2 size = CIwFVec2(w, h);
	int vitality = getGameState()->getPlayer()->getVitality();

	for (int i = 0; i < vitality; i++) {
		CIwFVec2 topLeft = CIwFVec2(leftPadding + (leftMargin+w)*i, topPadding);

		Iw2DDrawImage(healthTexture, topLeft, size);
	}
}
