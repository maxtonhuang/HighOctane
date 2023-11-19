/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		GameStateManager.h
*
*	@author		Liu WanTing
*
*	@email		wanting.liu\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		26 October 2023
*
* *****************************************************************************
*
*	@brief		Game State Management
*
*	This file contains the implementation of the game state management system.
*	It manages the different states of the game such as loading, running, and
*	unloading game states. The system is used by the Game State Manager (GSM)
*	to transition between different phases of the game.
*
******************************************************************************/

#pragma once

// game state function pointers typedef
typedef void (*LoadFunc)();
typedef void (*InitFunc)();
typedef void (*UpdateFunc)(double dt);
typedef void (*DrawFunc)(double dt);
typedef void (*FreeFunc)();
typedef void (*UnloadFunc)();

/**
 * Enumerates the different game states that can exist within the game.
 * These states represent different screens or phases of the game such as menus, gameplay, etc.
 */
enum GS_STATES
{
	GS_MENU = 0,
	GS_MAP,
	GS_BATTLE,
	GS_QUIT,
	GS_RESTART
};

/**
 * Class representing an individual game state, with function pointers for each lifecycle event.
 */
class GameState {
private:
	int idx;
	LoadFunc loadFunc;
	InitFunc initFunc;
	UpdateFunc updateFunc;
	DrawFunc drawFunc;
	FreeFunc freeFunc;
	UnloadFunc unloadFunc;
public:
	GameState();
	void SetGameState(int gsIdx, LoadFunc load, InitFunc init, UpdateFunc update, DrawFunc draw, FreeFunc free, UnloadFunc unload);
	void GSM_Load();
	void GSM_Init();
	void GSM_Update(double dt);
	void GSM_Draw(double dt);
	void GSM_Free();
	void GSM_Unload();
	int  GetIdx();
};

class GameLevel {
private:
	int currentLevel;
	int totalLevels;

public:
	GameLevel(int total) : totalLevels(total), currentLevel(0) {}

	void IncreaseLevel()
	{
		if (currentLevel < totalLevels) {
			currentLevel++;
		}
	}

	void JumpToEndLevel()
	{
		currentLevel = totalLevels - 1;
	}

	int GetCurrentLevel() const
	{
		return currentLevel;
	}
};

/**
 * Singleton class managing the different game states and transitions between them.
 */
class GameStateMgr {
private:
	enum 
	{ 
		GSLOAD, 
		GSINIT, 
		GSRUNNING, 
		GSFREE, 
		GSUNLOAD 
	};
	unsigned gsmState;
	unsigned gameStateCount;
	GameState* stateArray;
	int state;
	int nextState;
	bool continueNextState;
	static GameStateMgr* gamestatemgr_;
	GameStateMgr(unsigned gsmCount, int totalLevels);
	GameLevel* gameLevel;

public:
	bool isRunning;
	static GameStateMgr* GetInstance(unsigned gsmCount, int totalCount);
	static GameStateMgr* GetInstance();
	static void RemoveInstance();
	void InsertGameState(int gsmIdx, LoadFunc load, InitFunc init, UpdateFunc update, DrawFunc draw, FreeFunc free, UnloadFunc unload);
	void UpdateGameStateMgr();
	void ChangeGameState(int gsmIdx);
	void QuitGame();
	void NextLevel();
	void GoToEndLevel();
	~GameStateMgr();
};


