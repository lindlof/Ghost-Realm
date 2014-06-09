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
