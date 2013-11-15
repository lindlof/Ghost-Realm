/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Player.h"

#include "Iw2D.h"
#include "IwGraphics.h"

class PlayerAttackView {
private:
	static const int ANIM_FRAMES_PER_IMG = 2;
	static const int ANIM_LENGTH = 4 * ANIM_FRAMES_PER_IMG;

	CIw2DImage* attack1;
	CIw2DImage* attack2;
	CIw2DImage* attack3;
	CIw2DImage* attack4;

	Player* player;
	int state;

	void renderAttack(CIw2DImage* attack);

public:
	PlayerAttackView(Player*);
	~PlayerAttackView();

	void Render();
};
