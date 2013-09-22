/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <time.h>

class Ghost {
	private:
	int positionX;
	int positionY;
	int width;
	int height;

	clock_t hitTime;

	// Ghost is not initially in combat but you find it
	// from a specific point of compass around you.
	bool staging;
	int compassPoint;

	public:
	Ghost();
	bool ghostUpdate();
	void compassUpdate(double heading, bool error);
	void ghostGotHit();
	clock_t getHitTime();
};
