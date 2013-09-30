/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#define MAX_PLAYER_VITALITY 10000

class Player {
	private:
	int vitality;
	double heading;

	public:
	Player();
	void compassUpdate(double heading, bool error);
	void playerGotHit(int vitality);
	int getVitality();
	double getHeading();
};
