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
*	@date		22 October 2023
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
//#include "CharacterStats.h"
#include "ECS.h"
#include <list>
#include <vector>

class CharacterStats;

/**
 * @enum BattleState
 * @brief Enumerates the possible states of a battle in the game.
 *
 * This enumeration defines the various stages a battle can be in, from the start of a new game to the outcome of the battle.
 */
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

/**
 * @struct TurnManagement
 * @brief Manages the turn order and active characters in a battle.
 *
 * This structure maintains the order in which characters take their turns,
 * as well as the active player and enemy for the current turn.
 */
struct TurnManagement
{
	std::string activePlayer;
	std::string activeEnemy;
	std::list <CharacterStats> characterList;
	std::list <CharacterStats*> turnOrderList;
	std::list <CharacterStats*> originalTurnOrderList;
};

/**
 * @struct RoundManagement
 * @brief Manages the rounds within a battle.
 *
 * This structure keeps track of the number of characters that have taken their turn
 * and the total number of rounds that have passed in the battle.
 */
struct RoundManagement
{
	int characterCount;
	int roundCounter;
};

/**
 * @class BattleSystem
 * @brief Handles the turn-based battle logic within the game.
 *
 * The BattleSystem class is responsible for managing the state of the battle, including
 * initialization, updating turns, and maintaining the turn and round order of characters.
 */
class BattleSystem : public System 
{
public:
	BattleSystem() = default;
	BattleSystem(BattleSystem const&);
	BattleState battleState;
	RoundManagement roundManage;
	TurnManagement turnManage;
	void Initialize();
	void Update() override;
	CharacterStats* activeCharacter;
	std::vector<CharacterStats*> GetPlayers();
	std::vector<CharacterStats*> GetEnemies();
	int chi{ 5 };

	bool locked;
private:
	
	//std::vector <GameObject> gameObjects;
	bool roundInProgress;

	//Variables for animation
	bool battlestarted{ false };
	Entity turnOrderAnimator;
	std::deque<Entity> turnOrderQueueInitializer;
	std::deque<Entity> turnOrderQueueAnimator;

	//Animation methods
	void InitialiseTurnOrderAnimator();
	bool AnimateInitialiseTurnOrder(); //multiple stage animation, returns true when animation is done
	bool AnimateUpdateTurnOrder(); //2 part animation, returns true when animation is done
	void AnimateRemoveTurnOrder(Entity entity);

	//BattleState NewGameDelay(float startDelay, float nextDelay);
	void StartBattle();
	void DetermineTurnOrder();
	void SwitchTurnOrder(CharacterStats* target);
	void RevertTurnOrder(CharacterStats* target);
};