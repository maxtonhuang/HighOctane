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
*	@file		Attack.h
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		19 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains class for skill system and damage
*   formula of the game as well as a skill manager for loaded skills
*
******************************************************************************/

#pragma once
#include "CharacterCommon.h"
class CharacterStats;

enum class AttackType {
    NORMAL,
    AOE
};

class Attack {
public:
    void UseAttack(CharacterStats* target);
    void UseAttack(std::vector<CharacterStats*> target);
	void CalculateDamage(CharacterStats const& target);
    void SetOwner(CharacterStats* input);

    AttackType attacktype;
    std::string attackName;

    int skillAttackPercent;
    float minAttackMultiplier;
    float maxAttackMultiplier;
    float critRate;
    float critMultiplier;
private:
    CharacterStats* owner;
    bool critCheck;

    float damage;
};

class AttackList {
public:
    void SaveAttack(Attack const& attack);
    void LoadAttack(std::string attackPath);
    std::vector<std::string> GetAttackNames();
    std::unordered_map<std::string, Attack> data;
};
