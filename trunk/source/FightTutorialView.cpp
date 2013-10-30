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

FightTutorialView::FightTutorialView() {
	bgTexture = Iw2DCreateImage("textures/tutorial/tutorial_bg.png");

	attackTexture = Iw2DCreateImage("textures/tutorial/tutorial_attack_the_ghost.png");
	defendTexture = Iw2DCreateImage("textures/tutorial/tutorial_defend_from_the_attacks.png");
	faceWarnTexture = Iw2DCreateImage("textures/tutorial/tutorial_face_the_ghost_to_attack.png");
	ghostWonTexture = Iw2DCreateImage("textures/tutorial/tutorial_ghost_won.png");
	searchTexture = Iw2DCreateImage("textures/tutorial/tutorial_search_the_ghost.png");
	youWonTexture = Iw2DCreateImage("textures/tutorial/tutorial_you_won.png");
}

void FightTutorialView::setTutorial(FightTutorial* fightTutorial) {
	this->tutorial = fightTutorial;
}

FightTutorialView::~FightTutorialView() {
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
	if (youWonTexture)
		delete youWonTexture;
}

void FightTutorialView::Render() {
	TutorialType type = tutorial->getTutorialType();

	if (type == TUTORIAL_NONE) {
		return;
	}

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetTransformMatrix(CIwFMat2D::g_Identity);

	CIwFVec2 bgSize = CIwFVec2(IwGxGetScreenWidth(), IwGxGetScreenHeight());
	CIwFVec2 bgTopLeft = CIwFVec2(0, 0);

	Iw2DDrawImage(bgTexture, bgTopLeft, bgSize);

	if (type == TUTORIAL_ATTACK) {
		drawText(attackTexture);
	} else if (type == TUTORIAL_DEFEND) {
		drawText(defendTexture);
	} else if (type == TUTORIAL_FACE_WARN) {
		drawText(faceWarnTexture);
	} else if (type == TUTORIAL_GHOST_WON) {
		drawText(ghostWonTexture);
	} else if (type == TUTORIAL_SEARCH) {
		drawText(searchTexture);
	} else if (type == TUTORIAL_YOU_WON) {
		drawText(youWonTexture);
	}
}

void FightTutorialView::drawText(CIw2DImage* text) {
	int16 w = IwGxGetScreenWidth();
	//if (w > text->GetWidth()) w = text->GetWidth();

	float whScale = (float)((double)text->GetWidth() / text->GetHeight());
	int16 h = w * 1/whScale;

	CIwFVec2 size = CIwFVec2(w, h);
	CIwFVec2 topLeft = CIwFVec2(IwGxGetScreenWidth()/2 - w/2, IwGxGetScreenHeight()*1.f - h);

	Iw2DDrawImage(text, topLeft, size);
}

void FightTutorialView::Touch(int x, int y, bool press) {
	if (press &&
		x > IwGxGetScreenWidth()*0.03 &&
		x < IwGxGetScreenWidth()*0.97 &&
		y > IwGxGetScreenHeight()*0.81 &&
		y < IwGxGetScreenHeight()*0.99) {

		if (tutorial->getTutorialType() == TUTORIAL_GHOST_WON ||
			tutorial->getTutorialType() == TUTORIAL_YOU_WON) {
			getGameState()->setGameMode(MAP_MODE);
		}
		tutorial->setTutorialType(TUTORIAL_NONE);
	}
}
