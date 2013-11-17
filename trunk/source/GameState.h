/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Player.h"
#include "Ghost.h"

enum GameMode { NO_GAMEMODE, CAMERA_MODE, MAP_MODE };
enum IntroState { INTRO_ATTACK, INTRO_DEFEND };
class GameState {
	private:
	GameMode gameMode;
	GameMode lastGameMode;
	IntroState introState;

	Player *player;
	Ghost *ghost;

	public:
	GameState();
	~GameState();
	void setGameMode(GameMode gameMode);
	void updateLastGameMode();
	GameMode getGameMode();
	GameMode getLastGameMode();

	Player* getPlayer();
	Ghost* getGhost();
	void setGhost(Ghost* ghost);
	void deleteGhost();

	IntroState getIntroState();
	void introProceed();
	void introReset();
};

GameState* getGameState();
