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

#include "Audio.h"

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

	bool sound = false;

	if (type == TUTORIAL_ATTACK && !attackShown) {
		sound = true;
		attackShown = true;
		currentType = type;
	} else if (type == TUTORIAL_DEFEND    && !defendShown) {
		sound = true;
		defendShown = true;
		currentType = type;
	} else if (type == TUTORIAL_FACE_WARN && !faceWarnShown) {
		sound = true;
		faceWarnShown = true;
		currentType = type;
	} else if (type == TUTORIAL_GHOST_WON && !ghostWonShown) {
		ghostWonShown = true;
		currentType = type;
	} else if (type == TUTORIAL_SEARCH    && !searchShown) {
		sound = true;
		searchShown = true;
		currentType = type;
	} else if (type == TUTORIAL_YOU_DIED   && !youWonShown) {
		youWonShown = true;
		currentType = type;
	} else if (type == TUTORIAL_YOU_WON   && !youWonShown) {
		youWonShown = true;
		currentType = type;
	}

	if (sound) {
		Audio::Play("sounds/camera_tutorial.mp3");
	}

	tutorialShowTime = clock();
}

bool FightTutorial::isTutorialOn() {
	return !(currentType == TUTORIAL_NONE);
}

TutorialType FightTutorial::getTutorialType() {
	return currentType;
}

void FightTutorial::tutorialAcknowledged() {
	if (clock() > tutorialShowTime + TUTORIAL_MIN_TIME) {
		if (currentType == TUTORIAL_GHOST_WON ||
			currentType == TUTORIAL_YOU_DIED ||
			currentType == TUTORIAL_YOU_WON) {

			getGameState()->deleteGhost();
			getGameState()->setGameMode(MAP_MODE);
		}

		currentType = TUTORIAL_NONE;
		tutorialShowTime = 0;
	}
}
