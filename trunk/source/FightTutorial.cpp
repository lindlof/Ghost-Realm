/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "FightTutorial.h"
#include "GameState.h"

FightTutorial::FightTutorial() {
	resetShown();
	currentType = TUTORIAL_NONE;
	tutorialShowTime = 0;
}

FightTutorial::~FightTutorial() {

}

void FightTutorial::resetShown() {
	attackShown = defendShown = faceWarnShown = ghostWonShown = searchShown = youDiedShown = youWonShown = false;
}

void FightTutorial::triggerTutorial(TutorialType type) {
	if (isTutorialOn()) return;

	if (type == TUTORIAL_ATTACK && !attackShown) {
		attackShown = true;
		currentType = type;
	} else if (type == TUTORIAL_DEFEND    && !defendShown) {
		defendShown = true;
		currentType = type;
	} else if (type == TUTORIAL_FACE_WARN && !faceWarnShown) {
		faceWarnShown = true;
		currentType = type;
	} else if (type == TUTORIAL_GHOST_WON && !ghostWonShown) {
		ghostWonShown = true;
		currentType = type;
	} else if (type == TUTORIAL_SEARCH    && !searchShown) {
		searchShown = true;
		currentType = type;
	} else if (type == TUTORIAL_YOU_DIED   && !youWonShown) {
		youWonShown = true;
		currentType = type;
	} else if (type == TUTORIAL_YOU_WON   && !youWonShown) {
		youWonShown = true;
		currentType = type;
	}

	tutorialShowTime = clock();
}

bool FightTutorial::isTutorialOn() {
	return !(currentType == TUTORIAL_NONE);
}

TutorialType FightTutorial::getTutorialType() {
	return currentType;
}

void FightTutorial::setTutorialType(TutorialType type) {
	if (type == TUTORIAL_NONE && 
			clock() < tutorialShowTime + TUTORIAL_MIN_TIME) {
		return;
	}

	currentType = type;
	tutorialShowTime = clock();
}
