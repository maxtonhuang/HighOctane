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
*	@file		CharacterAction.cpp
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

#include "CharacterAction.h"
#include "CharacterStats.h"
#include "Events.h"

/**
 * @brief Updates the state of a character's action within a battle turn.
 *
 * This function manages the progression of a character's state through their turn,
 * handling actions such as starting their turn, waiting, and performing attacks.
 */
void CharacterAction::UpdateState() {
    std::string name;
    switch (entityState) {
    case START:
        entityState = WAITING;
        name = ECS::ecs().GetComponent<Name>(characterStats->entity).name;
        //DEBUG_PRINT("Current Turn: %s", name.c_str());
        if (battleManager->m_Entities.size() > 0) {
            printf("\nCurrent Turn %s\n", name.c_str());
            DEBUG_PRINT("Current Turn %s", name.c_str());
        }
        RefreshTargets();
        break;
    case WAITING:
        //Empty by design, to wait for other systems to change entity state
        break;
    case ATTACKING:
        ApplySkill();
        entityState = ENDING;
        break;
    }
}

/**
 * @brief Refreshes the lists of player and enemy targets available to a character.
 *
 * This function calls the battle manager's GetPlayers and GetEnemies functions to
 * update the target selection for a character's action, such as who they can attack.
 */
void CharacterAction::RefreshTargets() {
    std::vector<CharacterStats*> playerTargets{};
    std::vector<CharacterStats*> enemyTargets{};

    targetSelect.playerTargets = battleManager->GetPlayers();
    targetSelect.enemyTargets = battleManager->GetEnemies();
}

/**
 * @brief Executes the character's selected skill against the target(s).
 *
 * This function first sets the owner of the skill to the character executing the action,
 * refreshes the list of targets, and then applies the selected skill based on its type
 * (normal or area of effect) to the appropriate targets.
 */
void CharacterAction::ApplySkill() {
    selectedSkill.SetOwner(this->characterStats);
    RefreshTargets();
    if (battleManager->m_Entities.size() > 0) {
        printf("Using skill: %s\n", selectedSkill.attackName.c_str());
        DEBUG_PRINT("Using skill: %s", selectedSkill.attackName.c_str());
    }
    // deduct the respective chi cost when a skill is used
    BattleSystem* battlesys = events.GetBattleSystem();
    battlesys->chi -= selectedSkill.chiCost;

    // increase chi cost by 1 when skill 1 is used
    if (selectedSkill.attackName == "Skill 1") {
        battlesys->chi += 1;
    }

    if (selectedSkill.attacktype == AttackType::NORMAL) {
        selectedSkill.UseAttack(targetSelect.selectedTarget);
    }
    else if(selectedSkill.attacktype == AttackType::AOE) {
        if (characterStats->tag == CharacterType::PLAYER) {
            selectedSkill.UseAttack(targetSelect.enemyTargets);
        }
        else {
            selectedSkill.UseAttack(targetSelect.playerTargets);
        }
    }
    
}
