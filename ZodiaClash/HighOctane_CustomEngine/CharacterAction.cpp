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
#include "Animation.h"
#include "EntityFactory.h"
#include "Layering.h"

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
        RefreshTargets();
        break;
    case WAITING:
        //Empty by design, to wait for other systems to change entity state
        break;
    case ATTACKING:
        ApplySkill();

        //Attack animation
        if (battleManager->m_Entities.size() > 0) {
            LayerOrderBringToFront(characterStats->entity);
            if (ECS::ecs().HasComponent<AnimationSet>(characterStats->entity)) {
                AnimationSet& animation{ ECS::ecs().GetComponent<AnimationSet>(characterStats->entity) };
                std::stringstream animationName{};
                for (size_t i = 0; i < skills.size(); i++) {
                    if (skills[i].attackName == selectedSkill.attackName) {
                        animationName << "Attack " << i + 1;
                        break;
                    }
                }
                static Entity returnpos{ EntityFactory::entityFactory().ClonePrefab("returnpos.prefab")};
                static Entity attackpos{ EntityFactory::entityFactory().ClonePrefab("attackpoint.prefab") };
                static vmath::Vector2 aoePoint{ ECS::ecs().GetComponent<Transform>(attackpos).position };
                if (!ECS::ecs().EntityExists(returnpos)) {
                    returnpos = EntityFactory::entityFactory().ClonePrefab("returnpos.prefab");
                }
                if (!ECS::ecs().EntityExists(attackpos)) {
                    attackpos = EntityFactory::entityFactory().ClonePrefab("attackpoint.prefab");
                }
                ECS::ecs().GetComponent<Transform>(returnpos).position = ECS::ecs().GetComponent<Transform>(characterStats->entity).position;
                
                if (selectedSkill.attacktype != AttackType::AOE) {
                    Transform* attacktrans{ &ECS::ecs().GetComponent<Transform>(attackpos) };
                    attacktrans->position = ECS::ecs().GetComponent<Transform>(targetSelect.selectedTarget->entity).position;
                    attacktrans->position.y += (ECS::ecs().GetComponent<Size>(characterStats->entity).height * ECS::ecs().GetComponent<Transform>(characterStats->entity).scale
                        - ECS::ecs().GetComponent<Size>(targetSelect.selectedTarget->entity).height * ECS::ecs().GetComponent<Transform>(targetSelect.selectedTarget->entity).scale) / 2;
                    if (targetSelect.selectedTarget->tag == CharacterType::PLAYER) {
                        attacktrans->position.x += 50.f;
                    }
                    else {
                        attacktrans->position.x -= 50.f;
                    }
                }
                else {
                    ECS::ecs().GetComponent<Transform>(attackpos).position = aoePoint;
                }
                animation.Start("Attack Start", characterStats->entity);
                animation.Queue(animationName.str(), characterStats->entity);
            }
            Entity battlelabel = EntityFactory::entityFactory().ClonePrefab("battlelabel.prefab");
            ECS::ecs().GetComponent<TextLabel>(battlelabel).textString = selectedSkill.attackName;
            battleManager->locked = true;
            battleManager->MoveOutUIAnimation();
        }
        
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
    battleManager->chi -= selectedSkill.chiCost;

    if (battleManager->chi > 5) {
        battleManager->chi = 5;
    }

    // increase chi cost by 1 when skill 1 is used
    if (selectedSkill.attackName == "Skill 1") {
        battleManager->chi += 1;
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
