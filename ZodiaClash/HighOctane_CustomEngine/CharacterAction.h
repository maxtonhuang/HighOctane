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
*	@file		CharacterAction.h
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
*	@date		30 September 2023
*
* *****************************************************************************
*
*	@brief		Battle Character Actions
*
* This file contains the declarations of the functions that manage the actions
* of characters during a battle scene. These functions handle state updates,
* target selection, and skill application for characters. The Game State Manager (GSM)
* activates the logic encapsulated in these functions.

*
******************************************************************************/

#pragma once
#include "CharacterCommon.h"
#include "Attack.h"
#include "Battle.h"

/**
 * @class CharacterAction
 * @brief Manages the actions of a character during a battle sequence.
 *
 * The CharacterAction class is responsible for handling the state and behavior of a
 * character's actions during their turn in battle. It includes the management of the
 * character's state, the selection and application of skills, and the determination of
 * targets for those skills. The class interfaces with the BattleSystem and CharacterStats
 * to execute actions within the game's battle logic.
 */
class CharacterAction
{
public:
    EntityState entityState{};
    //SkillSelection skillSelect;
    TargetSelection targetSelect{};
    std::vector<Attack> skills{};
    Attack selectedSkill{};
    //void SelectSkill(std::string btn);
    void UpdateState();
    BattleSystem* battleManager{};
    CharacterStats* characterStats{};
private:
    //void Start();

protected:
    bool playerAttacking{};
    bool endingTurn{};
    bool checkingStatus{};
    
    void RefreshTargets();
    void ApplySkill();
};
