/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <time.h>
#include "Player.h"

#define MAX_GHOST_ECTOPLASM 100

enum GhostType { GHOST_NORMAL };

class Ghost {
	private:
	GhostType ghostType;
	Player *player;
	bool found;

	int positionX;
	int positionY;
	int width;
	int height;
	int ectoplasm;

	clock_t middleMagnetTime;
	int middleMagnet;

	clock_t foundAnimTime;
	int compassX;
	float scale;
	int foundAnimProgress;
	
	clock_t playerHitTime;
	clock_t hitTime;

	int16 getMidPositionX();

	// Ghost is not initially in combat but you find it
	// from a specific point of compass around you.
	bool staging;
	double bearing;

	public:
	Ghost(GhostType ghostType, Player *player);
	bool ghostUpdate();
	void ghostGotHit();
	clock_t getHitTime();

	int getStrength();
	int getWidth();
	int getHeight();
	int getPositionX();

	void floatingUpdate(int32 x, int32 y, int32 z);
	bool isFound();
	int getEctoplasm();
};
