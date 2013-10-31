/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Iw2D.h"
#include "s3eTimer.h"
#include "time.h"

class MapRoamingGhost {
	private:
	CIwFVec2 centre;
	CIwFVec2 size;

	CIw2DImage* ghostTexture;
	CIwFMat2D *matrix;

	clock_t roamingUpdate;

	int ghostRoamingAngle;
	int ghostRoamingRadius;

	public:
	MapRoamingGhost(char *texture, CIwFVec2 centre);
	~MapRoamingGhost();

	void Render();
	void Update();

	void modifyCentreWithTexture(float x, float y);
};
