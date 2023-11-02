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
*	@file		CharacterStats.cpp
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
*	@brief		Battle Character Statistics
*
*	This file contains implementations of functions related to a character's
*	statistics and actions during a battle scene. These functions are part of the game
*	logic and are called by the Game State Manager (GSM) during battle events.
*
******************************************************************************/

#include "CharacterStats.h"
#include "CharacterAction.h"
#include "PlayerAction.h"
#include "Battle.h"
#include <thread>

/**
 * @brief Default constructor for CharacterStats. It initializes health to maxHealth and sets this instance for action.characterStats.
 */
CharacterStats::CharacterStats() {
    stats.health = stats.maxHealth;
    action.characterStats = this;
}

/**
 * @brief Copy constructor for CharacterStats. It copies the input instance into this instance and sets proper references for actions and battle manager.
 * @param input A constant reference to another CharacterStats object.
 */
CharacterStats::CharacterStats(CharacterStats const& input) {
    checkedStatus = input.checkedStatus;
    action = input.action;
    entity = input.entity;
    stats = input.stats;
    tag = input.tag;
    action.characterStats = this;
    gameObject = input.gameObject;
    parent = input.parent;
    action.battleManager = input.action.battleManager;
}

/**
 * @brief Initializes the CharacterStats by setting the battle manager and the characterStats reference for the action.
 */
void CharacterStats::Start()
{
    action.battleManager = parent;
    action.characterStats = this;
}

//void CharacterStats::Death(Entity& entity)
//{
    //if (tag == CharacterType::PLAYER)
    //{
    //    // Assuming you have a way to get the _PlayerAction component from the gameObject
    //    PlayerAction player = ECS::ecs().GetComponent<PlayerAction>(entity);
    //    player.playerState = DYING;
    //    
    //}
    //else if (tag == CharacterType::ENEMY)
    //{
    //    // Assuming you have a way to get the EnemyAction component from the gameObject
    //    EnemyAction enemy = ECS::ecs().GetComponent<EnemyAction>(entity);
    //    enemy.enemyState = DYING;
    //}
    
//}

/**
 * @brief Applies damage to the character and updates the character's state if health drops to or below zero.
 * @param damage The amount of damage to apply to the character.
 */
void CharacterStats::TakeDamage(float damage) 
{
    if (parent->m_Entities.size() > 0) {
        std::string name{ ECS::ecs().GetComponent<Name>(entity).name };
        printf("%s took %f damage!\n", name.c_str(), damage);
    }
    stats.health -= damage;
    if (stats.health <= 0)
    {
        if (parent->m_Entities.size() > 0) {
            std::string name{ ECS::ecs().GetComponent<Name>(entity).name };
            Model& model{ ECS::ecs().GetComponent<Model>(entity) };
            printf("%s is dying!\n", name.c_str());
            model.SetColor(0.2f, 0.2f, 0.2f);
        }
        stats.health = 0;
        action.entityState = DYING;
        //Death(entity);
    }
}

/**
 * @brief Applies a healing buff to the character, ensuring the health does not exceed maxHealth.
 * @param buffAmount The amount of health to add to the character.
 */
void CharacterStats::HealBuff(float buffAmount) 
{
    stats.health += buffAmount;

    if (stats.health >= stats.maxHealth)
    {
        stats.health = stats.maxHealth;
    }
}

/**
 * @brief Pauses the thread for a given number of seconds.
 * @param seconds The amount of time in seconds to pause the thread.
 */
void WaitForSeconds(float seconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
}

/**
 * @brief Compares this CharacterStats instance with another for equality based on the entity identifier.
 * @param input A constant reference to another CharacterStats object.
 * @return bool True if the entities are the same; otherwise, false.
 */
bool CharacterStats::operator==(const CharacterStats& input) const {
    if (entity == input.entity) {
        return true;
    }
    return false;
}
