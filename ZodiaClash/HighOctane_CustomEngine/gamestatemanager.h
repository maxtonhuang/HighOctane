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
*	@date		22 October 2023
*
* *****************************************************************************
*
*	@brief		Game State Manager
*
*	This file contains the declarations of the different parts of the Game
*	State Manager.
*
******************************************************************************/

#pragma once

// game state functions typedef
typedef void (*LoadFunc)();
typedef void (*InitFunc)();
typedef void (*UpdateFunc)(float dt);
typedef void (*DrawFunc)(float dt);
typedef void (*FreeFunc)();
typedef void (*UnloadFunc)();

enum GS_STATES
{
	GS_MAINMENU = 0,
	GS_CREDITS,
	GS_EXPLORE,
	GS_BATTLE,
	GS_QUIT,
	GS_RESTART
};

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
	void GSM_Update(float dt);
	void GSM_Draw(float dt);
	void GSM_Free();
	void GSM_Unload();
	int GetIdx();
};

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
	GameStateMgr(unsigned gsmCount);

public:
	bool isRunning;
	static GameStateMgr* GetInstance(unsigned gsmCount);
	/// ONLY USE THIS IN YOUR INIT FUNCTION
	static GameStateMgr* GetInstance();
	static void RemoveInstance();
	/// <summary>
	/// sets game state at stateArray[gsIdx]
	/// </summary>
	void InsertGameState(int gsmIdx, LoadFunc load, InitFunc init, UpdateFunc update, DrawFunc draw, FreeFunc free, UnloadFunc unload);
	void UpdateGameStateMgr();
	void ChangeGameState(int gsmIdx);
	void QuitGame();
	~GameStateMgr();
};

