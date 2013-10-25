/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "PlayerHit.h"

#include "IwRandom.h"

PlayerHit::PlayerHit(Player* player) {
	this->player = player;

	bloodTexture = Iw2DCreateImage("textures/player_blood.png");

	IwRandSeed((int32)s3eTimerGetMs());
	float width = (float)IwGxGetScreenWidth()*0.13f;
}

PlayerHit::~PlayerHit() {
	if (bloodTexture)
		delete bloodTexture;
}

void PlayerHit::Render() {
	IwGxLightingOff();
	IwGxSetColEmissive(true);

	int sinceHit = clock() - player->getHitTime();
	int animation = 400;

	if (sinceHit < animation) {
		int halfAnimation = animation / 2;

		float animState = (float)sinceHit / halfAnimation;
		if (animState > 1) {
			animState = 2 - animState;
		}

		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

		CIwFVec2 size = CIwFVec2(IwGxGetScreenWidth(), IwGxGetScreenHeight());
		CIwFVec2 topLeft = CIwFVec2(0, 0);
		
		CIwColour colour;
		colour.Set(0xFF, 0xFF, 0xFF, 0xFF*animState);
		Iw2DSetColour(colour);

		Iw2DDrawImage(bloodTexture, topLeft, size);

		colour.Set(0xFF, 0xFF, 0xFF, 0xFF);
		Iw2DSetColour(colour);
	}
}
