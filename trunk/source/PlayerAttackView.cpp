/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

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
