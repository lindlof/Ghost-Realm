/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "IwGx.h"

class FightButton {
	private:
	CIwTexture* fightTexture;
	CIwSVec2 buttonXY, buttonWH;

	public:
	FightButton();
	~FightButton();

	void Render();
	void Touch(int x, int y);
};
