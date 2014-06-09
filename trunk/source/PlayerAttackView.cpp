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

#include "PlayerAttackView.h"

PlayerAttackView::PlayerAttackView(Player* player) {
	attack1 = Iw2DCreateImage("textures/attack/attack_1.png");
	attack2 = Iw2DCreateImage("textures/attack/attack_2.png");
	attack3 = Iw2DCreateImage("textures/attack/attack_3.png");
	attack4 = Iw2DCreateImage("textures/attack/attack_4.png");

	this->player = player;

	state = 0;
}

PlayerAttackView::~PlayerAttackView() {
	if (attack1)
		delete attack1;
	if (attack2)
		delete attack2;
	if (attack3)
		delete attack3;
	if (attack4)
		delete attack4;
}

void PlayerAttackView::Render() {
	if (state == 0 && !player->pollAnimAttack()) return;

	float currentImage = (float)state/ANIM_FRAMES_PER_IMG;

	if (currentImage <= 0)
		renderAttack(attack1);
	else if (currentImage <= 1)
		renderAttack(attack2);
	else if (currentImage <= 2)
		renderAttack(attack3);
	else if (currentImage <= 3)
		renderAttack(attack4);

	if (++state >= ANIM_LENGTH) state = 0;
}

void PlayerAttackView::renderAttack(CIw2DImage* attack) {

	float h = IwGxGetScreenHeight()*1.0f;

	float whScale = (float)((double)attack->GetWidth() / attack->GetHeight());
	int16 w = h * whScale;

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	CIwFVec2 bgSize = CIwFVec2(w, h);
	CIwFVec2 bgTopLeft = CIwFVec2(0, IwGxGetScreenHeight() - h);

	Iw2DDrawImage(attack, bgTopLeft, bgSize);
}
