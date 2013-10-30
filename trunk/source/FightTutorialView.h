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

class FightTutorialView {
	private:
	FightTutorial* tutorial;

	CIw2DImage* bgTexture;

	CIw2DImage* attackTexture;
	CIw2DImage* defendTexture;
	CIw2DImage* faceWarnTexture;
	CIw2DImage* ghostWonTexture;
	CIw2DImage* searchTexture;
	CIw2DImage* youDiedTexture;
	CIw2DImage* youWonTexture;

	void drawText(CIw2DImage* text);

	public:
	FightTutorialView();
	~FightTutorialView();
	void setTutorial(FightTutorial* fightTutorial);

	void Render();
	void Touch(int x, int y, bool press);
};

#endif
