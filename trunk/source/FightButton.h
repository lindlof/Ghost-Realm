/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _FIGHT_BUTTON_H
#define _FIGHT_BUTTON_H

#include "IwGx.h"

class FightButton {
	private:
	CIwTexture* fightTextTexture;
	CIwSVec2 buttonTextXY, buttonTextWH;

	CIwTexture* fightBgTexture;
	CIwSVec2 buttonBgXY, buttonBgWH;

	float buttonPercY;

	public:
	FightButton();
	~FightButton();

	void Render();
	void Touch(int x, int y);
};

#endif
