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

#ifndef _GHOST_ATTACK_H
#define _GHOST_ATTACK_H

#include "GhostType.h"
#include "Player.h"
class Player;

#include <time.h>

class GhostAttack {
	private:
	double hit;

	int interval;
	clock_t lastTime;

	Player *player;
	bool over;

	public:
	GhostAttack(Player* player, GhostType ghostType);

	void Update();

	void setDefended();

	bool isOver();
	clock_t getInterval();
};

#endif
