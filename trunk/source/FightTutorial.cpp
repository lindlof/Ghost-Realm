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

#include "FightTutorial.h"
#include "GameState.h"

#include "Audio.h"

FightTutorial::FightTutorial() {
	resetShown();
	currentType = TUTORIAL_NONE;
	tutorialShowTime = 0;
	dismissTime = 0;
}

FightTutorial::~FightTutorial() {

}

void FightTutorial::resetShown() {
	attackShown = defendShown = faceWarnShown = ghostWonShown = searchShown = youDiedShown = youWonShown = false;
}

void FightTutorial::counterTrigger(TutorialType type) {
	if (currentType == type) {
		tutorialAcknowledged();
	}
}

void FightTutorial::triggerTutorial(TutorialType type) {
	if (isTutorialOn()) return;
	if (clock() < dismissTime + 500) return; // 0,5s delay between tuts

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
		dismissTime = clock();
	}
}
