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

#define MAX_PLAYER_VITALITY 10000
#define HEADING_FILTER 0.2f

class Player {
	private:
	int ready;
	int vitality;
	double heading;
	double headingFilter;
	Ghost *ghost;
	Strike *strike;

	public:
	Player();
	bool playerUpdate();
	bool isReady();
	void compassUpdate(double heading, bool error);
	void playerGotHit(int vitality);
	int getVitality();
	double getHeading();
	void accelometerUpdate(int32 x, int32 y, int32 z);
	void setGhost(Ghost *ghost);
};

#endif
