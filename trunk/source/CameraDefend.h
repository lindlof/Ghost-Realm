/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _GHOST_DEFEND_H
#define _GHOST_DEFEND_H

#include "GhostAttack.h"

#include <time.h>
#include "IwGraphics.h"

class CameraDefend {
	private:
	CIwMaterial* pMat;
	CIwTexture* defendingDotTexture;

	CIwFVec2 defendVertsLeft[4];
	CIwFVec2 defendVertsRight[4];

	CIwFVec2 drawStart;
	bool drawing;
	bool active;

	public:
	CameraDefend();
	~CameraDefend();

	void Update();
	void Render();
	void Touch(int32 x, int32 y, bool press);
	void Motion(int32 x, int32 y);
	bool isActive();
};

#endif
