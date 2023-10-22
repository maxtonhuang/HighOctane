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
*	@file		Battle.h
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
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Battle Scene
*
*	This file contains the battle scene's game logic's function declarations,
*	and will be activated by the Game State Manager (GSM).
*
******************************************************************************/

#pragma once
#include "CharacterStats.h"

enum BattleState
{
	NEWGAME,
	NEWROUND,
	NEXTTURN,
	PLAYERTURN,
	ENEMYTURN,
	WIN,
	LOSE
};
struct TurnManagement
{
	std::string activePlayer;
	std::string activeEnemy;
	std::vector <CharacterStats> characterList;
	std::vector <CharacterStats> turnOrderList;
	std::vector <CharacterStats> originalTurnOrderList;
};
struct RoundManagement
{
	int characterCount;
	int roundCounter;
};

class BattleSystem : public System 
{
public:
	BattleState battleState;
	RoundManagement roundManage;
	TurnManagement turnManage;
	void Initialize();
	void Update() override;

private:
	CharacterStats activeCharacter;
	std::vector <GameObject> gameObjects;
	bool roundInProgress;

	BattleState NewGameDelay(float startDelay, float nextDelay);
	void DetermineTurnOrder();
};