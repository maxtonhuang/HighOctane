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
#include "CheatCode.h"
#include <thread>
#include "Global.h"

/**
 * @brief Default constructor for CharacterStats. It initializes health to maxHealth and sets this instance for action.characterStats.
 */
CharacterStats::CharacterStats() {
    stats.health = stats.maxHealth;
    action.characterStats = this;
    boss = false;
    charge = false;
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
    icon = input.icon;
    action.characterStats = this;
    gameObject = input.gameObject;
    parent = input.parent;
    action.battleManager = input.action.battleManager;
    boss = input.boss;
    charge = input.charge;
    debuffs = input.debuffs;
    buffs = input.buffs;
    cycle = input.cycle;
    crit = input.crit;
}

/**
 * @brief Initializes the CharacterStats by setting the battle manager and the characterStats reference for the action.
 */
void CharacterStats::Start()
{
    action.battleManager = parent;
    action.characterStats = this;
}

/**
 * @brief Applies damage to the character and updates the character's state if health drops to or below zero.
 * @param damage The amount of damage to apply to the character.
 */
void CharacterStats::TakeDamage(float damage) 
{
    if (godModeOn && this->tag == CharacterType::PLAYER) {
        // in God Mode, the player does not take damage
        RestoreFullHealth(*this);
        return;
    }

    if (godModeOn && this->tag == CharacterType::ENEMY) {
        // in God Mode, the enemy takes double the damage from the player
        damage *= 2;
    }

    if (endGameOn && this->tag == CharacterType::ENEMY) {
        DestroyAllHealth(*this);
    }

    if (parent->m_Entities.size() > 0) {
        std::string name{ ECS::ecs().GetComponent<Name>(entity).name };
        DEBUG_PRINT("%s took %f damage!", name.c_str(), damage);
    }

    this->stats.health -= damage;
    this->damage = damage;
    if (stats.health <= 0)
    {
        stats.health = 0;
        action.entityState = DYING;
    }
    else if (stats.health > stats.maxHealth) {
        stats.health = stats.maxHealth;
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

void CharacterStats::SpeedBuff(CharacterStats* target)
{
    //if (user->tag == CharacterType::ENEMY) {
    //    CharacterStats* tmp{};
    //    for (CharacterStats* character : parent->GetEnemies()) {
    //        if (character->entity != user->entity) {
    //            tmp = character;
    //            break;
    //        }
    //    }
    //    //tmp->tag = CharacterType::ENEMYSPEDUP;
    //    parent->SwitchTurnOrder(tmp);
    //    goatSpeedup = true;

    //}
    parent->SwitchTurnOrder(target);
}

void CharacterStats::ApplyBloodStack() 
{
    const float bleedPercent{ 0.1f };
    static float catAttack{ 0.f };

    //Find cat attack
    if (catAttack == 0.f) {
        for (auto c : parent->GetPlayers()) {
            if (c->icon == "mockup_icon_cat.png") {
                catAttack = c->stats.attack;
                break;
            }
        }
    }
    if (debuffs.bloodStack > 0) {
        float damage = catAttack * bleedPercent;
        TakeDamage(damage);
        debuffs.bloodStack--;
        if (parent->m_Entities.size() > 0) {
            parent->damagePrefab = "VFX_Bleed.prefab";
        }
    }
    else return; //when there is no bloodstack effect
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
