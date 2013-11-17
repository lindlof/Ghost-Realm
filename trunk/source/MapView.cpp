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
#include "MapRoamingGhost.h"
#include "MapModel.h"

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"
#include "Iw2D.h"

void renderMap();
void renderMapPlayer();
void renderMapHealth();
void renderMapXpBar();

static MapRoamingGhost* mapGhost;
static MapRoamingGhost* mapGhost2;
static bool defendIntroSet;

static CIw2DImage* playerTexture;
static CIw2DImage* healthTexture;
static CIw2DImage* xpBarTexture;
static CIwTexture* mapTexture;

void mapInit(int mapW, int mapH);
static CIwFVec2 mapVert[4];
static CIwFVec2 mapVertZoom[4];

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
	playerTexture = Iw2DCreateImage("textures/map_player.png");
	healthTexture = Iw2DCreateImage("textures/map_health.png");
	xpBarTexture  = Iw2DCreateImage("textures/map_xp_bar.png");
	
	mapTexture = new CIwTexture;
	mapTexture->LoadFromFile ("textures/map_template.png");
	mapTexture->Upload();
	
	mapInit(mapTexture->GetWidth(), mapTexture->GetHeight());

	ghostInit();
}

void ghostInit() {
	mapGhost = new MapRoamingGhost("textures/map_ghost.png", 
		CIwFVec2(IwGxGetScreenWidth()/2, IwGxGetScreenHeight()/2));
	mapGhost->modifyCentreWithTexture(-0.18f, -0.13f);
	mapGhost->setNotice(true);

	mapGhost2 = new MapRoamingGhost("textures/map_ghost_xtra.png",
		CIwFVec2(IwGxGetScreenWidth()*0.80f, IwGxGetScreenHeight()*0.70f));
	defendIntroSet = false;
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

	if (playerTexture)
		delete playerTexture;

	if (mapTexture)
		delete mapTexture;

	if (mapGhost)
		delete mapGhost;

	if (mapGhost2)
		delete mapGhost2;

	if (xpBarTexture)
		delete xpBarTexture;
}

void arrivalCallback(MapRoamingGhost* ghost) {
	ghost->setNotice(true);
}

bool MapViewUpdate() {

	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	renderMap();
	renderMapPlayer();
	renderMapXpBar();
	renderMapHealth();

	IntroState introState = getGameState()->getIntroState();

	if (introState == INTRO_ATTACK) {
		mapGhost->Update();
		mapGhost->Render();
	} else if (introState == INTRO_DEFEND && !defendIntroSet) {
		defendIntroSet = true;
		mapGhost2->moveGhost(CIwFVec2(IwGxGetScreenWidth()/2, IwGxGetScreenHeight()/2), arrivalCallback);
	}

	mapGhost2->Update();
	mapGhost2->Render();

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

	float zoom = getMapZoom()->getZoom();

	int16 hScrW = (double)IwGxGetScreenWidth()*0.5f;
	int16 hScrH = (double)IwGxGetScreenHeight()*0.5f;
	int16 x1 = hScrW - hScrW*zoom;
	int16 x2 = hScrW + hScrW*zoom;
	int16 y1 = hScrH - hScrH*zoom;
	int16 y2 = hScrH + hScrH*zoom;

	mapVertZoom[0].x = x1, mapVertZoom[0].y = y1;
	mapVertZoom[1].x = x1, mapVertZoom[1].y = y2;
	mapVertZoom[2].x = x2, mapVertZoom[2].y = y2;
	mapVertZoom[3].x = x2, mapVertZoom[3].y = y1;

    IwGxSetUVStream(mapDefaultUvs);
    IwGxSetVertStreamScreenSpace(mapVertZoom, 4);

    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void renderMapPlayer() {
	IwGxLightingOff();

	int16 w = IwGxGetScreenWidth()*0.085f;

	float whScale = (float)((double)playerTexture->GetWidth() / playerTexture->GetHeight());
	int16 h = w * 1/whScale;

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	double angle = rad(getGameState()->getPlayer()->getHeading());

	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);
	CIwFVec2 centre =
			CIwFVec2((int16)Iw2DGetSurfaceWidth()/2,
					 (int16)Iw2DGetSurfaceHeight()/2);
	CIwFMat2D rot;
	rot.SetRot(angle, (CIwFVec2)centre);
	Iw2DSetTransformMatrix(rot);

	CIwFVec2 size = CIwFVec2(w, h);
	CIwFVec2 topLeft = CIwFVec2(centre.x-size.x/2, centre.y-size.y/2);

	Iw2DDrawImage(playerTexture, topLeft, size);
}

MapRoamingGhost* getRoamingGhost() {
	return getGameState()->getIntroState() == INTRO_ATTACK ? mapGhost : mapGhost2;
}

void renderMapXpBar() {
	IwGxLightingOff();

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	int16 w = (double)IwGxGetScreenWidth() * 0.40f;
	if (w > (double)xpBarTexture->GetWidth() * 1.80f) w = (double)xpBarTexture->GetWidth() * 1.80f;

	float whScale = (float)((double)xpBarTexture->GetWidth() / xpBarTexture->GetHeight());
	int16 h = w * 1/whScale;

	int rightPadding = IwGxGetScreenWidth() * 0.10f;
	int topPadding  = IwGxGetScreenHeight() * 0.015f;

	CIwFVec2 size = CIwFVec2(w, h);
	CIwFVec2 topLeft = CIwFVec2(IwGxGetScreenWidth() - rightPadding - w, topPadding);

	Iw2DDrawImage(xpBarTexture, topLeft, size);
}

void renderMapHealth() {
	IwGxLightingOff();

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	int16 w = (double)IwGxGetScreenWidth() * 0.09f;
	if (w > (int16)healthTexture->GetWidth()) w = (int16)healthTexture->GetWidth();

	float whScale = (float)((double)healthTexture->GetWidth() / healthTexture->GetHeight());
	int16 h = w * 1/whScale;

	int leftPadding = IwGxGetScreenWidth() * 0.10f;
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
