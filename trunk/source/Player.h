/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

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

	bool animAttack;
	bool poll(bool& poll);

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
	void resetMana();
	void lostBattle();
	void wonBattle();
	bool isDead();
	void resurrect();
	clock_t getHitTime();

	bool pollAnimAttack();
};

#endif
