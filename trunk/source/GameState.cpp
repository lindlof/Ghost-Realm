/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GameState.h"

// GameState singleton
static GameState* gameState = new GameState();

GameState* getGameState() {
	return gameState;
}


GameState::GameState() {
	GameState::gameMode = NO_GAMEMODE;
	player = new Player();
	ghost = NULL;
}

GameState::~GameState() {
	delete player;
	delete ghost;
}

void GameState::setGameMode(GameMode gameMode) {
	GameState::lastGameMode = GameState::gameMode;
	GameState::gameMode = gameMode;
}

void GameState::updateLastGameMode() {
	GameState::lastGameMode = GameState::gameMode;
}

GameMode GameState::getGameMode() {
	return gameMode;
}

GameMode GameState::getLastGameMode() {
	return lastGameMode;
}

Ghost* GameState::getGhost() {
	return ghost;
}

Player* GameState::getPlayer() {
	return player;
}

void GameState::setGhost(Ghost* ghost) {
	GameState::ghost = ghost;
}

void GameState::deleteGhost() {
	delete ghost;
	GameState::ghost = NULL;
}
