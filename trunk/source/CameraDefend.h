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
	CIw2DImage* dotTexture;
	CIw2DImage* dotTextureGreen;
	CIw2DImage* dotTextureRed;

	CIwFVec2 dotVertsTopLeftLeft;
	CIwFVec2 dotVertsSizeLeft;
	CIwFVec2 dotVertsTopLeftRight;
	CIwFVec2 dotVertsSizeRight;

	CIwMaterial* animMat;
	CIwTexture* animTexturePress;

	CIwFVec2 animVertsLeft[4];
	CIwFVec2 animVertsRight[4];

	bool defended;
	bool defendedPlayed;
	CIwMaterial* animMatSuccess;
	CIwTexture* animTextureSuccess;

	CIwFVec2 animVertsLeftSuccess[4];
	CIwFVec2 animVertsRightSuccess[4];

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
