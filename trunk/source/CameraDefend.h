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
#include "Iw2D.h"

#define DEFEND_TOUCHES_MAX 5

class DefendTouch {
	public:
	bool drawing;
	CIwFVec2 drawStart;
};

class CameraDefend {
	private:
	CIwMaterial* animMat;
	CIw2DImage* dotTexture;
	CIwTexture* animTexture;

	CIwFVec2 dotVertsTopLeftLeft;
	CIwFVec2 dotVertsSizeLeft;
	CIwFVec2 dotVertsTopLeftRight;
	CIwFVec2 dotVertsSizeRight;

	CIwFVec2 animVertsLeft[4];
	CIwFVec2 animVertsRight[4];

	int dotAngle;
	bool active;

	DefendTouch* touch[DEFEND_TOUCHES_MAX];

	public:
	CameraDefend();
	~CameraDefend();

	void Update();
	void Render();
	void Touch(int32 x, int32 y, bool press, uint32 id);
	void Motion(int32 x, int32 y, uint32 id);
	bool isActive();

	void reinit();
};

#endif
