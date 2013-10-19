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

CIwTexture* mapGhost;
CIwTexture* mapPlayer;
CIwTexture* mapTemplate;

void MapViewInit()
{
	// Set screen clear colour
    IwGxSetColClear(0xff, 0xff, 0xff, 0xff);
    IwGxPrintSetColour(128, 128, 128);

	mapGhost = new CIwTexture;
	mapGhost->LoadFromFile ("textures/map_ghost.png");
	mapGhost->Upload();

	mapPlayer = new CIwTexture;
	mapPlayer->LoadFromFile ("textures/map_player.png");
	mapPlayer->Upload();

	mapTemplate = new CIwTexture;
	mapTemplate->LoadFromFile ("textures/map_template.png");
	mapTemplate->Upload();
}

void MapViewTerm() {

	if (mapGhost)
		delete mapGhost;

	if (mapPlayer)
		delete mapPlayer;

	if (mapTemplate)
		delete mapTemplate;
}

bool MapViewUpdate() {
	return true;
}
