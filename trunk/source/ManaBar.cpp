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

#include "ManaBar.h"
#include "Player.h"
#include "GameState.h"

#include "time.h"

ManaBar::ManaBar() {
	borderTexture = Iw2DCreateImage("textures/mana/mana_bar_border.png");
	centerTexture = Iw2DCreateImage("textures/mana/mana_bar_center.png");
	endTexture = Iw2DCreateImage("textures/mana/mana_bar_end.png");
	startTexture = Iw2DCreateImage("textures/mana/mana_bar_start.png");

	borderTopLeft = CIwFVec2((double)IwGxGetScreenWidth()*0.05, (double)IwGxGetScreenHeight()*0.01);
	borderSize = CIwFVec2((double)IwGxGetScreenWidth()*0.90, (double)IwGxGetScreenHeight()*0.02);
}

ManaBar::~ManaBar() {
	if (borderTexture)
		delete borderTexture;

	if (centerTexture)
		delete centerTexture;

	if (endTexture)
		delete endTexture;

	if (startTexture)
		delete startTexture;
}

void ManaBar::Render() {
	IwGxLightingOff();

	float mana = getGameState()->getPlayer()->getMana() / PLAYER_MAX_MANA;

	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
		Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

		Iw2DDrawImage(borderTexture, borderTopLeft, borderSize);
	}

	if (mana > 0) {
		// Bar start
		float y = borderTopLeft.y + (float)borderSize.y*0.22;
		float h = (float)borderSize.y*0.56;

		float whScale = (float)((double)startTexture->GetWidth() / startTexture->GetHeight());
		int16 w = h * whScale;

		CIwFVec2 startTopLeft = CIwFVec2(borderTopLeft.x + (float)borderSize.x*0.007, y);
		CIwFVec2 startSize = CIwFVec2(w, h);
		Iw2DDrawImage(startTexture, startTopLeft, startSize);

		// Bar end
		whScale = (float)((double)endTexture->GetWidth() / endTexture->GetHeight());
		w = h * whScale;

		int endMaxX = borderTopLeft.x + borderSize.x - (float)borderSize.x*0.007 - w;
		int endMinX = startTopLeft.x + startSize.x;
		int endX = (double)(endMaxX - endMinX)*mana + endMinX;

		CIwFVec2 endTopLeft = CIwFVec2(endX, y);
		CIwFVec2 endSize = CIwFVec2(w, h);
		Iw2DDrawImage(endTexture, endTopLeft, endSize);

		// Bar center
		CIwFVec2 centerTopLeft = CIwFVec2(startTopLeft.x + startSize.x - 1, y);
		CIwFVec2 centerSize = CIwFVec2(endTopLeft.x - centerTopLeft.x + 2, h);
		Iw2DDrawImage(centerTexture, centerTopLeft, centerSize);
	}
}
