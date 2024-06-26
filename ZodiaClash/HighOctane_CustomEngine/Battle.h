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
*   @author		Foong Pun Yuen Nigel (Animation functions)
*
*	@email		p.foong\@digipen.edu
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

struct CharacterAnimator {
	Entity character;
	Entity healthbar;
	Entity healthbarIcon;
	Entity healthbarBase;
	Entity turnorder;
	Entity turnorderIcon;
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
	BattleState battleState{};
	RoundManagement roundManage{};
	TurnManagement turnManage{};
	void Initialize();
	void Update() override;
	CharacterStats* activeCharacter{};
	std::vector<CharacterStats*> GetPlayers();
	std::vector<CharacterStats*> GetEnemies();
	int chi{ 5 };
	bool speedup{ false };
	CharacterStats* speedupCharacter{};
	void AddCharacter(Entity addition); //Add a new character midway through a battle
	bool locked{};

	//for battle system to pass its version of characters
	//to the real entities version
	void ProcessDamage();
	//Prefab to use for damage effect, based on skill used
	std::string damagePrefab{};

	//Create target circles for enemy targeting
	void CreateTargets();
	//Destroy target circles after enemy has been chosen
	void DestroyTargets();
	//Updates targets for battle system UI animations
	void UpdateTargets();

	//Calls "Pop Out" animation for skill UI, to remove UI during attacking animations
	void MoveOutUIAnimation();
	//Calls "Pop In" animation for skill UI, to add back UI after attacking
	void MoveInUIAnimation();

	//Calls "Pop Out" animation for all UI, to remove UI during attacking animations
	void MoveOutAllUIAnimation();
	//Calls "Pop In" animation for all UI, to add back UI after attacking
	void MoveInAllUIAnimation();

	void SwitchTurnOrder(CharacterStats* target);
	void RevertTurnOrder(CharacterStats* target);

	std::vector<Entity> allBattleUI;
	std::vector<Entity> skillButtons;
	std::vector<Entity> allyHealthBars;
	std::vector<Entity> enemyHealthBars;
	std::deque<Entity> turnOrderQueueAnimator;
	std::vector<Entity> targetCircleList;

	Entity chiLabel;
	Entity battleInfoButton;

	int aiMultiplier; //in order to control certain AI logic
	int dialogueCalled{ 0 };
	int tutorialLock{ 0 };
	int skillTooltipCalled{ 0 };
	Entity battleinfo{}; //battle info tooltip
	bool attackingAnimation{ false };
	bool battlestarted{ false };
	bool emperorDead{ false }; //To call dialogue for emperor
private:
	
	bool roundInProgress;
	bool speedupAnimationPlayed{ false };

	//Variables for animation
	Entity turnOrderAnimator;
	std::deque<Entity> turnOrderQueueInitializer;
	std::vector<CharacterAnimator> enemyAnimators;
	std::vector<CharacterAnimator> allyAnimators;
	std::vector<std::string> tooltips{}; //for tooltip creation
	Entity turnIndicator{ 0 };
	bool battleUIMovedOut{ false };
	bool speedUpAndDeath{ false }; //For special case where there is a speed up and death animation
	
	//Animation methods
	//Initialises the battle system UI
	void InitialiseBattleUI();

	//Initialises the turn order animator
	void InitialiseTurnOrderAnimator();

	//To pop in all UI after turn order
	void InitialiseUIAnimation();

	//multiple stage animation to be called for amount of characters, returns true when animation is done
	bool AnimateInitialiseTurnOrder(); 
	//2 part animation to be called twice, returns true when animation is done
	bool AnimateUpdateTurnOrder(); 
	void AnimateSpeedupTurnOrder();
	void AnimateReturnTurnOrder();
	//Removes the entity from the turn order animator
	void AnimateRemoveTurnOrder(Entity entity);
	
	//Removes healthbar when character dies
	void AnimateRemoveHealthBar(Entity entity);

	//Updates skill icons
	void UpdateSkillIcons();

	//BattleState NewGameDelay(float startDelay, float nextDelay);
	void StartBattle();
	void CompleteBattle();
	bool DetermineTurnOrder(); //returns false if unable to properly initialise turn order
};