/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Player.h"

#include <time.h>
#include "IwGraphics.h"
#include "Iw2D.h"

class PlayerHit {
	private:
	Player* player;

	CIw2DImage* bloodTexture;

	public:
	PlayerHit(Player* player);
	~PlayerHit();

	void Render();
};
