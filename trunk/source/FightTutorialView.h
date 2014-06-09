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

#ifndef _FIGHT_TUTORIAL_VIEW_H
#define _FIGHT_TUTORIAL_VIEW_H

#include "FightTutorial.h"

#include "IwGraphics.h"
#include "Iw2D.h"
#include "time.h"

class FightTutorialView {
	private:
	FightTutorial* tutorial;

	CIw2DImage* bgTexture;
	CIw2DImage* buttonTexture;
	CIwFVec2 buttonSize;
	CIwFVec2 buttonTopLeft;

	CIw2DImage* attackTexture;
	CIw2DImage* defendTexture;
	CIw2DImage* faceWarnTexture;
	CIw2DImage* ghostWonTexture;
	CIw2DImage* searchTexture;
	CIw2DImage* youDiedTexture;
	CIw2DImage* youWonTexture;

	static const int attackAnimCells = 2;
	static const int attackAnimRows = 2;
	static CIwFVec2 attack_anim_uvs[4];
	CIwFVec2 attackAnimVerts[4];
	CIwTexture* attackAnimTexture;
	CIwMaterial* attackAnimMat;

	static const int defendAnimCells = 4;
	static const int defendAnimRows = 4;
	static CIwFVec2 defend_anim_uvs[4];
	CIwFVec2 defendAnimVerts[4];
	CIwTexture* defendAnimTexture;
	CIwMaterial* defendAnimMat;

	static const int searchAnimCells = 4;
	static const int searchAnimRows = 4;
	static CIwFVec2 search_anim_uvs[4];
	CIwFVec2 searchAnimVerts[4];
	CIwTexture* searchAnimTexture;
	CIwMaterial* searchAnimMat;

	clock_t pressedTime;


	void drawText(CIw2DImage* text, bool animated);
	void drawAttackAnim();
	void drawDefendAnim();
	void drawSearchAnim();

	public:
	FightTutorialView();
	~FightTutorialView();
	void setTutorial(FightTutorial* fightTutorial);

	void Render();
	void Touch(int x, int y, bool pressed);
};

#endif
