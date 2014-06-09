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

#ifndef _FIGHT_TUTORIAL_H
#define _FIGHT_TUTORIAL_H

#include "time.h"

#define TUTORIAL_MIN_TIME 500

enum TutorialType { TUTORIAL_NONE, TUTORIAL_ATTACK, TUTORIAL_DEFEND, TUTORIAL_FACE_WARN, 
	TUTORIAL_GHOST_WON,TUTORIAL_SEARCH, TUTORIAL_YOU_DIED, TUTORIAL_YOU_WON };

class FightTutorial {
	private:
	TutorialType currentType;

	clock_t tutorialShowTime;
	clock_t dismissTime;

	bool attackShown, defendShown, faceWarnShown, ghostWonShown, searchShown, youDiedShown, youWonShown;

	public:
	FightTutorial();
	~FightTutorial();

	TutorialType getTutorialType();
	void tutorialAcknowledged();
	bool isTutorialOn();
	void triggerTutorial(TutorialType type);
	void counterTrigger(TutorialType type);
	void resetShown();
};

#endif
