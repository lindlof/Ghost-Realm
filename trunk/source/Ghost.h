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
#define GHOST_ATTACK_ANIM_WAIT 1820 // Longer wait -> ghost hits later

class Ghost {
	private:
	GhostType ghostType;
	Player *player;
	bool found;

	int ectoplasm;
	clock_t lastUpdate;

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
	bool ghostHasBeenHit;

	int nextAttackInterval;

	double floatingAngle;

	// Ghost is not initially in combat but you find it
	// from a specific point of compass around you.
	double bearing;

	bool animDodge;
	bool animAttack;
	int animAttackInterval;
	bool agroPlayed;
	bool capturedPlayed;
	bool poll(bool& poll);

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
	bool isDead();
	
	int getDistance();
	double getBearing();
	float getRotation();

	void tapped();
	GhostAttack* getAttack();
	void deleteAttack();
	bool isAttackDefendable();

	void floatingAngleUpdate(float x, float y, float z);
	double getFloatingAngle();

	bool pollAnimDodge();
	bool pollAnimAttack();
	bool pollAnimAgro();
	bool pollAnimCaptured();

	GhostType getGhostType();
};

#endif
