/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _FIGHT_TUTORIAL_VIEW_H
#define _FIGHT_TUTORIAL_VIEW_H

#include "FightTutorial.h"

#include "IwGraphics.h"
#include "Iw2D.h"
#include "time.h"

class FightTutorialView {
	private:
	FightTutorial* tutorial;

	CIw2DImage* bgTexture;
	CIw2DImage* buttonTexture;
	CIwFVec2 buttonSize;
	CIwFVec2 buttonTopLeft;

	CIw2DImage* attackTexture;
	CIw2DImage* defendTexture;
	CIw2DImage* faceWarnTexture;
	CIw2DImage* ghostWonTexture;
	CIw2DImage* searchTexture;
	CIw2DImage* youDiedTexture;
	CIw2DImage* youWonTexture;

	static const int attackAnimCells = 2;
	static const int attackAnimRows = 2;
	static CIwFVec2 attack_anim_uvs[4];
	CIwFVec2 attackAnimVerts[4];
	CIwTexture* attackAnimTexture;
	CIwMaterial* attackAnimMat;

	static const int defendAnimCells = 4;
	static const int defendAnimRows = 4;
	static CIwFVec2 defend_anim_uvs[4];
	CIwFVec2 defendAnimVerts[4];
	CIwTexture* defendAnimTexture;
	CIwMaterial* defendAnimMat;

	static const int searchAnimCells = 4;
	static const int searchAnimRows = 2;
	static CIwFVec2 search_anim_uvs[4];
	CIwFVec2 searchAnimVerts[4];
	CIwTexture* searchAnimTexture;
	CIwMaterial* searchAnimMat;

	clock_t pressedTime;


	void drawText(CIw2DImage* text, bool animated);
	void drawAttackAnim();
	void drawDefendAnim();
	void drawSearchAnim();

	public:
	FightTutorialView();
	~FightTutorialView();
	void setTutorial(FightTutorial* fightTutorial);

	void Render();
	void Touch(int x, int y, bool pressed);
};

#endif
