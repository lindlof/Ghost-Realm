/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _GHOST_H
#define _GHOST_H

#include <time.h>

#include "GhostType.h"
class GhostType;
#include "GhostAttack.h"
class GhostAttack;
#include "Player.h"
class Player;

#define GHOST_MAX_ECTOPLASM 100.f

class Ghost {
	private:
	GhostType ghostType;
	Player *player;
	bool found;

	int ectoplasm;

	float initRotation;
	float rotation;

	bool foundUnintialized;
	clock_t foundInitTime;
	clock_t foundAnimTime;
	float scale;
	int foundAnimProgress;
	
	GhostAttack* ghostAttack;
	bool attackDefendable;
	clock_t hitTime;

	clock_t playerAttackLast;
	int nextAttackInterval;

	double floatingAngle;

	// Ghost is not initially in combat but you find it
	// from a specific point of compass around you.
	double bearing;

	public:
	Ghost(GhostType ghostType, Player *player);
	void cameraInit();

	bool ghostUpdate();
	void ghostGotHit(int hit);
	clock_t getHitTime();

	float getStrength();

	void setFound();
	bool isFound();
	int getEctoplasm();
	
	int getDistance();
	double getBearing();
	float getRotation();

	void tapped();
	GhostAttack* getAttack();
	void deleteAttack();
	bool isAttackDefendable();

	void floatingAngleUpdate(float x, float y, float z);
	double getFloatingAngle();
};

#endif
