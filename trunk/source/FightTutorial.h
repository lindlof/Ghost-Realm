/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

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

	bool attackShown, defendShown, faceWarnShown, ghostWonShown, searchShown, youDiedShown, youWonShown;

	public:
	FightTutorial();
	~FightTutorial();

	TutorialType getTutorialType();
	void setTutorialType(TutorialType type);
	bool isTutorialOn();
	void triggerTutorial(TutorialType type);
	void resetShown();
};

#endif
