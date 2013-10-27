/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

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
};

#endif
