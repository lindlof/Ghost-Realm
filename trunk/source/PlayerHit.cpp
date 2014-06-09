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
