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

#ifndef _PLAYER_H
#define _PLAYER_H
#include "Strike.h"

#include "Ghost.h"
class Ghost;

#define PLAYER_MAX_VITALITY 3
#define PLAYER_MAX_MANA 100.f
#define HEADING_FILTER_CAMERA 0.2f
#define HEADING_FILTER_MAP 0.8f

#define PLAYER_HIT_LENGTH 600
#define PLAYER_HIT_FLASHES 3

class Player {
	private:
	int ready;
	int vitality;
	clock_t hitTime;
	int mana;
	double heading;
	double headingFilter;
	Ghost *ghost;
	Strike *strike;

	void init();

	bool ghostSeen;

	bool animAttack;
	bool poll(bool& poll);

	int winCount;
	int loseCount;
	int deadCount;

	public:
	Player();
	bool headingUpdate();
	bool isReady();
	void compassUpdate(double heading, bool error);
	void playerGotHit(int hit);
	double getHeading();
	void accelometerUpdate(int32 x, int32 y, int32 z);
	void setGhost(Ghost *ghost);

	int getVitality();
	int getMana();
	void initFight();
	void lostBattle();
	void wonBattle();
	bool isDead();
	void resurrect();
	clock_t getHitTime();

	bool pollAnimAttack();

	int getWinCount();
	int getLoseCount();
	int getDeadCount();
	void increaseWinCount();
	void increaseLoseCount();
	void increaseDeadCount();
};

#endif
