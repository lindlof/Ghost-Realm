/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "FightTutorialView.h"
#include "GameState.h"

CIwFVec2 FightTutorialView::attack_anim_uvs[4] =
{
    CIwFVec2(0,                   0),
    CIwFVec2(0,                   1.f/attackAnimRows),
    CIwFVec2(1.f/attackAnimCells, 1.f/attackAnimRows),
    CIwFVec2(1.f/attackAnimCells, 0),
};

CIwFVec2 FightTutorialView::defend_anim_uvs[4] =
{
    CIwFVec2(0,                   0),
    CIwFVec2(0,                   1.f/defendAnimRows),
    CIwFVec2(1.f/defendAnimCells, 1.f/defendAnimRows),
    CIwFVec2(1.f/defendAnimCells, 0),
};

CIwFVec2 FightTutorialView::search_anim_uvs[4] =
{
    CIwFVec2(0,                   0),
    CIwFVec2(0,                   1.f/searchAnimRows),
    CIwFVec2(1.f/searchAnimCells, 1.f/searchAnimRows),
    CIwFVec2(1.f/searchAnimCells, 0),
};

FightTutorialView::FightTutorialView() {
	bgTexture = Iw2DCreateImage("textures/tutorial/tutorial_bg.png");
	buttonTexture = Iw2DCreateImage("textures/tutorial/tutorial_button.png");

	attackTexture = Iw2DCreateImage("textures/tutorial/tutorial_attack_the_ghost.png");
	defendTexture = Iw2DCreateImage("textures/tutorial/tutorial_defend_from_the_attacks.png");
	faceWarnTexture = Iw2DCreateImage("textures/tutorial/tutorial_face_the_ghost_to_attack.png");
	ghostWonTexture = Iw2DCreateImage("textures/tutorial/tutorial_ghost_won.png");
	searchTexture = Iw2DCreateImage("textures/tutorial/tutorial_search_the_ghost.png");
	youDiedTexture = Iw2DCreateImage("textures/tutorial/tutorial_you_died.png");
	youWonTexture = Iw2DCreateImage("textures/tutorial/tutorial_you_won.png");
	
	{ // Attack animation
		attackAnimTexture = new CIwTexture;
		attackAnimTexture->LoadFromFile("textures/tutorial/attack_animation.png");
		attackAnimTexture->Upload();

		attackAnimMat = new CIwMaterial;
		attackAnimMat->SetTexture(attackAnimTexture);

		attackAnimMat->CreateAnim();
		attackAnimMat->SetAnimCelW((double)attackAnimTexture->GetWidth()/attackAnimCells);
		attackAnimMat->SetAnimCelH((double)attackAnimTexture->GetHeight()/attackAnimRows);
		attackAnimMat->SetAnimCelPeriod(5);

		int h = (double)IwGxGetScreenHeight()*0.35f;
		float whScale = 
			(float)(((double)attackAnimTexture->GetWidth()/attackAnimCells) /
			((double)attackAnimTexture->GetHeight()/attackAnimRows));
		int16 w = h * whScale;

		int x1 = (double)IwGxGetScreenWidth()*0.33f;
		int x2 = x1 + w;
		int y1 = (double)IwGxGetScreenHeight()*0.51f - h;
		int y2 = y1 + h;

		attackAnimVerts[0] = CIwFVec2(x1, y1);
		attackAnimVerts[1] = CIwFVec2(x1, y2);
		attackAnimVerts[2] = CIwFVec2(x2, y2);
		attackAnimVerts[3] = CIwFVec2(x2, y1);
	}

	{ // Defend animation
		defendAnimTexture = new CIwTexture;
		defendAnimTexture->LoadFromFile("textures/tutorial/defend_animation.png");
		defendAnimTexture->Upload();

		defendAnimMat = new CIwMaterial;
		defendAnimMat->SetTexture(defendAnimTexture);

		defendAnimMat->CreateAnim();
		defendAnimMat->SetAnimCelW((double)defendAnimTexture->GetWidth()/defendAnimCells);
		defendAnimMat->SetAnimCelH((double)defendAnimTexture->GetHeight()/defendAnimRows);
		defendAnimMat->SetAnimCelPeriod(5);

		int h = (double)IwGxGetScreenHeight()*0.34f;
		float whScale = 
			(float)(((double)defendAnimTexture->GetWidth()/defendAnimCells) / 
			((double)defendAnimTexture->GetHeight()/defendAnimRows));
		int16 w = h * whScale;

		int x1 = (double)IwGxGetScreenWidth()*0.5f - (float)w*0.49f;
		int x2 = x1 + w;
		int y1 = (double)IwGxGetScreenHeight()*0.15f;
		int y2 = y1 + h;

		defendAnimVerts[0] = CIwFVec2(x1, y1);
		defendAnimVerts[1] = CIwFVec2(x1, y2);
		defendAnimVerts[2] = CIwFVec2(x2, y2);
		defendAnimVerts[3] = CIwFVec2(x2, y1);
	}

	{ // Search animation
		searchAnimTexture = new CIwTexture;
		searchAnimTexture->LoadFromFile("textures/tutorial/search_animation.png");
		searchAnimTexture->Upload();

		searchAnimMat = new CIwMaterial;
		searchAnimMat->SetTexture(searchAnimTexture);

		searchAnimMat->CreateAnim();
		searchAnimMat->SetAnimCelW((double)searchAnimTexture->GetWidth()/searchAnimCells);
		searchAnimMat->SetAnimCelH((double)searchAnimTexture->GetHeight()/searchAnimRows);
		searchAnimMat->SetAnimCelPeriod(2.66);

		int h = (double)IwGxGetScreenHeight()*0.35f;
		float whScale = 
			(float)(((double)searchAnimTexture->GetWidth()/searchAnimCells) / 
			((double)searchAnimTexture->GetHeight()/searchAnimRows));
		int16 w = h * whScale;

		int x1 = (double)IwGxGetScreenWidth()*0.50f - (float)w*0.50f;
		int x2 = x1 + w;
		int y1 = (double)IwGxGetScreenHeight()*0.51f - h;
		int y2 = y1 + h;

		searchAnimVerts[0] = CIwFVec2(x1, y1);
		searchAnimVerts[1] = CIwFVec2(x1, y2);
		searchAnimVerts[2] = CIwFVec2(x2, y2);
		searchAnimVerts[3] = CIwFVec2(x2, y1);
	}

	pressedTime = 0;
}

void FightTutorialView::setTutorial(FightTutorial* fightTutorial) {
	this->tutorial = fightTutorial;
}

FightTutorialView::~FightTutorialView() {
	if (bgTexture)
		delete bgTexture;
	if (buttonTexture)
		delete buttonTexture;

	if (attackTexture)
		delete attackTexture;
	if (defendTexture)
		delete defendTexture;
	if (faceWarnTexture)
		delete faceWarnTexture;
	if (ghostWonTexture)
		delete ghostWonTexture;
	if (searchTexture)
		delete searchTexture;
	if (youDiedTexture)
		delete youDiedTexture;
	if (youWonTexture)
		delete youWonTexture;

	if (attackAnimTexture)
		delete attackAnimTexture;
	if (attackAnimMat)
		delete attackAnimMat;
	if (defendAnimTexture)
		delete defendAnimTexture;
	if (defendAnimMat)
		delete defendAnimMat;
	if (searchAnimTexture)
		delete searchAnimTexture;
	if (searchAnimMat)
		delete searchAnimMat;
}

void FightTutorialView::Render() {
	TutorialType type = tutorial->getTutorialType();

	if (type == TUTORIAL_NONE) return;

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	CIwFVec2 bgSize = CIwFVec2(IwGxGetScreenWidth(), IwGxGetScreenHeight());
	CIwFVec2 bgTopLeft = CIwFVec2(0, 0);

	Iw2DDrawImage(bgTexture, bgTopLeft, bgSize);

	if (type == TUTORIAL_ATTACK) {
		drawText(attackTexture, true);
		drawAttackAnim();
	} else if (type == TUTORIAL_DEFEND) {
		drawText(defendTexture, true);
		drawDefendAnim();
	} else if (type == TUTORIAL_FACE_WARN) {
		drawText(faceWarnTexture, false);
	} else if (type == TUTORIAL_GHOST_WON) {
		drawText(ghostWonTexture, false);
	} else if (type == TUTORIAL_SEARCH) {
		drawText(searchTexture, true);
		drawSearchAnim();
	} else if (type == TUTORIAL_YOU_DIED) {
		drawText(youDiedTexture, false);
	} else if (type == TUTORIAL_YOU_WON) {
		drawText(youWonTexture, false);
	}
}

void FightTutorialView::drawText(CIw2DImage* text, bool animated) {

	float verticalMul = animated ? 0.75f : 0.5f;

	{ // Button
		int16 w = IwGxGetScreenWidth();
		//if (w > text->GetWidth()) w = text->GetWidth();

		float whScale = (float)((double)buttonTexture->GetWidth() / buttonTexture->GetHeight());
		int16 h = w * 1/whScale;

		buttonSize = CIwFVec2(w, h);
		buttonTopLeft = CIwFVec2(IwGxGetScreenWidth()/2 - w/2, IwGxGetScreenHeight()*verticalMul - h/2);

		Iw2DDrawImage(buttonTexture, buttonTopLeft, buttonSize);
	}

	{ // Text
		int16 w = IwGxGetScreenWidth();
		//if (w > text->GetWidth()) w = text->GetWidth();

		float whScale = (float)((double)text->GetWidth() / text->GetHeight());
		int16 h = w * 1/whScale;

		CIwFVec2 size = CIwFVec2(w, h);
		CIwFVec2 topLeft = CIwFVec2(IwGxGetScreenWidth()/2 - w/2, IwGxGetScreenHeight()/2 - h/2);
		if (animated) topLeft = CIwFVec2(IwGxGetScreenWidth()/2 - w/2, IwGxGetScreenHeight()*verticalMul - h/2);

		Iw2DDrawImage(text, topLeft, size);
	}
}

void FightTutorialView::drawAttackAnim() {
	IwGxLightingOff();

	attackAnimMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	attackAnimMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	IwGxSetMaterial(attackAnimMat);
	IwGxSetUVStream(attack_anim_uvs);

	IwGxSetVertStreamScreenSpace(attackAnimVerts, 4);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void FightTutorialView::drawDefendAnim() {
	IwGxLightingOff();

	defendAnimMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	defendAnimMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	IwGxSetMaterial(defendAnimMat);
	IwGxSetUVStream(defend_anim_uvs);

	IwGxSetVertStreamScreenSpace(defendAnimVerts, 4);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void FightTutorialView::drawSearchAnim() {
	IwGxLightingOff();

	searchAnimMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	searchAnimMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	IwGxSetMaterial(searchAnimMat);
	IwGxSetUVStream(search_anim_uvs);

	IwGxSetVertStreamScreenSpace(searchAnimVerts, 4);
	IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void FightTutorialView::Touch(int x, int y, bool pressed) {
	if (x > buttonTopLeft.x + buttonSize.x*0.03 &&
		x < buttonTopLeft.x + buttonSize.x*0.97 &&
		y > buttonTopLeft.y + buttonSize.y*0.23 &&
		y < buttonTopLeft.y + buttonSize.y*0.77) {

		if (pressed) {
			pressedTime = clock();
		}

		if (clock() - pressedTime < 1500 && !pressed) {
			tutorial->tutorialAcknowledged();
		}
	} else {
		pressedTime = 0;
	}
}
