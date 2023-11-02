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
*	@file		CharacterCommon.h
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
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief		Common Character Definitions
*
*	This file contains common structures, enumerations, and declarations used
*	across various components of the battle scene's game logic. These are
*	utilized by other systems and may be activated or referenced by the
*	Game State Manager (GSM).
*
******************************************************************************/

#pragma once
#include <vector>
#include <string>
#include "Components.h"

class CharacterStats;

/**
 * @struct GameObject
 * @brief Represents a generic object within the game, with activation and tagging properties.
 *
 * This structure encapsulates the properties and behaviors of a generic game object,
 * including its tag, name, active state, and methods for finding objects with specific tags.
 */
struct GameObject {
    Tag tag;
    std::string name;
    bool isnull;
    bool isActive;
public:
    static std::vector<GameObject*> FindGameObjectsWithTag(std::string tag)
    {
        //std::vector<GameObject*> output{};
        //auto tagArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Tag>().GetArray() };
        //for (auto t : tagArray) {
            //if (t->tag == tag) {
                //output.push_back(t);
            //}
        //}
        return std::vector<GameObject*>{};
    }
    GameObject() : isActive(true), isnull() {}  // By default, the game object is active
    // set the active state of the game object
    void SetActive(bool active)
    {
        isActive = active;
    }

    // check if the game object is active
    bool IsActive() const
    {
        return isActive;
    }
};

/**
 * @enum EntityState
 * @brief Enumerates the possible states of an entity during the battle.
 *
 * This enumeration defines the different stages an entity can be in during the course
 * of a battle, from starting their turn to potentially dying.
 */
enum EntityState
{
    START,
    WAITING,
    CHECKSTATUS,
    SELECTION,
    ATTACKING,
    ENDING,
    DYING,
    DEAD
};

/**
 * @enum class CharacterType
 * @brief Enumerates the types of characters that can participate in battle.
 *
 * This enumeration distinguishes between player-controlled characters and enemy characters.
 */
enum class CharacterType {
    PLAYER,
    ENEMY
};

/**
 * @struct SkillSelection
 * @brief Holds prefabs for skills and the currently selected skill.
 *
 * This structure is used to manage the selection of skills during a character's turn,
 * keeping references to skill prefabs and the selected skill.
 */
struct SkillSelection
{
    GameObject skill1Prefab;
    GameObject skill2Prefab;
    GameObject skill3Prefab;
    GameObject selectedSkillPrefab;
};

/**
 * @struct TargetSelection
 * @brief Manages the potential targets and the currently selected target for an action.
 *
 * This structure keeps track of potential targets for player and enemy actions,
 * as well as the currently selected target.
 */
struct TargetSelection
{
    std::vector<CharacterStats*> enemyTargets{};
    std::vector<CharacterStats*> playerTargets{};
    CharacterStats* selectedTarget{};
};
