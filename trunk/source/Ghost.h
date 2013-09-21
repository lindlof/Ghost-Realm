/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

class Ghost {
	private:
	int positionX;
	int positionY;
	int width;
	int height;

	bool hit;

	// Ghost is not initially in combat but you find it
	// from a specific point of compass around you.
	bool staging;
	int compassPoint;

	public:
	Ghost();
	void ghostGotHit();
	bool isGhostHit();
};
