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
*	@file		Attack.cpp
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

#include "Attack.h"
#include "Battle.h"
#include "CharacterStats.h"
#include "AssetManager.h"
#include <random>

#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/writer.h>
#include <rapidjson-master/include/rapidjson/prettywriter.h>
#include <rapidjson-master/include/rapidjson/stringbuffer.h>
#include <rapidjson-master/include/rapidjson/istreamwrapper.h>

/**
 * @brief Executes the skill against the target(s).
 *
 * This function applies the skill to the selected target
 */
void Attack::UseAttack(CharacterStats* target) {
    std::random_device rd;
    std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> rand(0.f, 1.f);

    CalculateDamage(*target);

    //Cat ult
    if (attackName == "Secret Arts: Pounce") {
        while (target->debuffs.bloodStack) {
            target->ApplyBloodStack();
        }
    
    }
    //Enemy goat attack
    else if (attackName == "Raging Soul Surge") {
        if (owner->stats.health < 0.5f * owner->stats.maxHealth && owner->charge) {
            owner->TakeDamage(-damage);
            owner->action.battleManager->aiMultiplier += 1000000;
            owner->charge = false;
        }
    }
    //Player goat attack
    else if (attackName == "Yin-Yang Strike") {
        if (rand(gen) > 0.6f) {
            target->buffs.attackStack = 0;
            target->buffs.attackBuff = 0.f;
            target->buffs.defenseStack = 0;
            target->buffs.defenseBuff = 0.f;
        }
    }
    //Enemy goat heal
    else if (attackName == "Dark Rising") {
        damage = -0.3f * owner->stats.maxHealth;
        target->debuffs.bloodStack = 0;
        target->debuffs.tauntStack = 0;
        target->debuffs.stunStack = 0;
    }
    //Player goat heal
    else if (attackName == "Shepherd's Grace") {
        damage = -0.3f * owner->stats.maxHealth;
        target->debuffs.bloodStack = 0;
        target->debuffs.tauntStack = 0;
        target->debuffs.stunStack = 0;
        target->debuffs.huntedStack = 0;
    }
    //Enemy goat speedup
    else if (attackName == "Dark Chi Surge") {
        target->SpeedBuff(target);
        target->buffs.attackBuff = 0.3f;
        target->buffs.attackStack = 1;
        if (target->tag == CharacterType::ENEMY) {
            owner->action.battleManager->aiMultiplier += 100000;
        }
    }
    //Player goat speedup
    else if (attackName == "Chi Surge") {
        target->SpeedBuff(target);
        target->buffs.attackBuff = 0.3f;
        target->buffs.attackStack = 1;
    }
    //Enemy goat charge
    else if (attackName == "Raging Soul Surge will be cast next!") {
        if (owner->stats.health < 0.5f * owner->stats.maxHealth && !owner->charge) {
            owner->action.battleManager->aiMultiplier += 1000000;
        }
        owner->charge = true;
    }
    //Player ox attack
    else if (attackName == "Shield Thrust") {
        target->debuffs.defenseDebuff = 0.5f;
        target->debuffs.defenseStack = 2;
    }
    //Enemy ox attack
    else if (attackName == "Shield Smash") {
        if (owner->cycle > 0 && owner->charge) {
            owner->action.battleManager->aiMultiplier += 100000;
            owner->cycle = owner->cycle > 2 ? 0 : owner->cycle + 1;
        }
    }
    //Enemy ox skill
    else if (attackName == "Resolute Charge") {
        target->debuffs.stunStack += 1;
    }
    //Enemy ox skill
    else if (attackName == "Unstoppable Charge") {
        target->debuffs.stunStack += 1;
        if (owner->cycle > 0 && !owner->charge) {
            owner->action.battleManager->aiMultiplier += 100000;
            owner->cycle = owner->cycle > 2 ? 0 : owner->cycle + 1;
            if (owner->cycle > 2) {
                owner->cycle = 0;
                owner->charge = true;
            }
        }
    }
    //Enemy ox ult
    else if (attackName == "Cursed War God's Wrath") {
        target->debuffs.tauntStack += 1;
        target->debuffs.tauntTarget = owner->entity;
        owner->buffs.defenseBuff = 0.5f;
        owner->buffs.defenseStack = 2;
        if (owner->cycle == 0) {
            owner->action.battleManager->aiMultiplier += 100000;
            owner->cycle = 1;
        }
    }
    //Player ox ult
    else if (attackName == "War God's Wrath") {
        target->debuffs.tauntStack += 1;
        target->debuffs.tauntTarget = owner->entity;
        owner->buffs.defenseBuff = 0.5f;
        owner->buffs.defenseStack = 2;
    }
    //Enemy emperor buff
    else if (attackName == "Emperor Buff") {
        if (owner->cycle == 0 && target->entity == owner->entity) {
            owner->action.battleManager->aiMultiplier += 100000;
            owner->cycle++;
        }
        owner->buffs.attackBuff += 0.5;
        owner->buffs.attackStack += 3;
    }
    //Enemey emperor lock-on
    else if (attackName == "Locking on!") {
        if (owner->cycle == 1 && target->entity == owner->parent->GetPlayers()[0]->entity) {
            owner->action.battleManager->aiMultiplier += 100000;
            owner->cycle++;
        }
        target->debuffs.huntedStack = 9;
    }
    //Enemy emperor attack
    else if (attackName == "Emperor Attack") {
        if (owner->cycle > 1) {
            owner->action.battleManager->aiMultiplier += 10000;
        }
        owner->cycle++;
        if (owner->cycle > 3) {
            owner->cycle = 0;
        }
    }
    //Enemy monkey attack
    else if (attackName == "Monkey Attack") {
        owner->cycle++;
    }
    //Enemy monkey AOE
    else if (attackName == "Monkey AOE") {
        if (!owner->charge) {
            damage = 0;
        }
        target->debuffs.igniteStack += 1;
    }
    //Enemy monkey charge
    else if (attackName == "Monkey Charge") {
        if (!owner->charge && !target->charge && target->tag == CharacterType::ENEMY && owner->cycle >= 2) {
            owner->action.battleManager->aiMultiplier += 100000;
            owner->cycle = 0;
        }
        else {
            owner->action.battleManager->aiMultiplier -= 100000;
        }
        target->charge = true;
    }

    target->debuffs.bloodStack += bleed;
    if (target->debuffs.bloodStack > 5) {
        target->debuffs.bloodStack = 5;
    }

    if (damage > 0 && target->tag == owner->tag) {
        owner->action.battleManager->aiMultiplier -= 100000;
    }

    if (owner->debuffs.tauntTarget == target->entity) {
        owner->action.battleManager->aiMultiplier += 50000;
    }

    if (target->debuffs.huntedStack) {
        owner->action.battleManager->aiMultiplier += 10000;
    }

    target->TakeDamage(damage);
    if (target->buffs.reflectStack > 0) {
        owner->TakeDamage(0.5f * damage);
    }

    if (owner->debuffs.igniteStack && chiCost > 0 && attacktype != AttackType::AOE) {
        owner->TakeDamage(10.f);
    }
}

/**
 * @brief Executes the skill against the target(s).
 *
 * This function applies the skill to the selected AOE targets
 */
void Attack::UseAttack(std::vector<CharacterStats*> target) {
    for (CharacterStats* t : target) {
        UseAttack(t);
    }

    if (attackName == "Monkey AOE") {
        if (!owner->charge) {
            owner->action.battleManager->aiMultiplier -= 100000;
        }
        else if (owner->cycle >= 2) {
            owner->charge = false;
            owner->action.battleManager->aiMultiplier += 100000;
            owner->cycle = 0;
        }
    }

    if (owner->debuffs.igniteStack && chiCost > 0) {
        owner->TakeDamage(10.f);
    }
}

/**
 * @brief Damage formula of the game
 *
 * This function applies damage formula of the game onto the target
 */
void Attack::CalculateDamage(CharacterStats& target)
{
    //critical hit chance
    std::random_device rd;
    std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> rand(0.f, 1.f);
    
    float randomValue{1.f};

    //NO CRITS IF ITS AN AI SIMULATION
    if (target.parent->m_Entities.size() > 0) {
        randomValue = rand(gen);
    }
    
    float finalAttack{ owner->stats.attack * (1 + owner->buffs.attackBuff - owner->debuffs.attackDebuff) };
    float finalDefense{ target.stats.defense * (1 + owner->buffs.defenseBuff - owner->debuffs.defenseDebuff) };

    std::uniform_real_distribution<float> drand(minAttackMultiplier, maxAttackMultiplier);

    if (randomValue <= critRate)
    {
        //critical hit
        target.crit = true;

        damage = (drand(gen) *
            ((float)skillAttackPercent / 100.f) * (finalAttack * (100.f / (100.f + finalDefense)))
            * critMultiplier);
    }
    else
    {
        target.crit = false;

        damage = (drand(gen) *
            ((float)skillAttackPercent / 100.f) * (finalAttack * (100.f / (100.f + finalDefense))));
    }
    damage = roundf(damage);
}

/**
 * @brief Set owner of the skill
 *
 * This function ties the owner of the skill to a set character
 */
void Attack::SetOwner(CharacterStats* input) {
    owner = input;
}

/**
 * @brief Get attack names loaded
 *
 * This function returns all attack names loaded by the system
 */
std::vector<std::string> AttackList::GetAttackNames() {
    std::vector<std::string> output{};
    for (auto& a : data) {
        output.push_back(a.first + ".skill");
    }
    return output;
}

/**
 * @brief Save Attack
 *
 * This function saves an attack to a .skill file
 */
void AttackList::SaveAttack(Attack const& attack) {
    rapidjson::Document document;
    document.SetArray();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    std::string attackPath{ assetmanager.GetDefaultPath() + "Skills/" + attack.attackName + ".skill" };
    rapidjson::Value object(rapidjson::kObjectType);

    rapidjson::Value nameValue;
    nameValue.SetString(attack.attackName.c_str(), static_cast<rapidjson::SizeType>(attack.attackName.length()), allocator);
    object.AddMember("Name", nameValue, allocator);

    rapidjson::Value skilltextureValue;
    skilltextureValue.SetString(attack.skillTexture.c_str(), static_cast<rapidjson::SizeType>(attack.skillTexture.length()), allocator);
    object.AddMember("Texture", skilltextureValue, allocator);

    rapidjson::Value skillTooltip;
    skilltextureValue.SetString(attack.skillTexture.c_str(), static_cast<rapidjson::SizeType>(attack.skillTooltip.length()), allocator);
    object.AddMember("Tooltip", skillTooltip, allocator);

    object.AddMember("Type", (int)attack.attacktype, allocator);
    object.AddMember("Skill Attack", attack.skillAttackPercent, allocator);
    object.AddMember("Minimum Attack Multiplier", attack.minAttackMultiplier, allocator);
    object.AddMember("Maximum Attack Multiplier", attack.maxAttackMultiplier, allocator);
    object.AddMember("Crit Rate", attack.critRate, allocator);
    object.AddMember("Crit Multiplier", attack.critMultiplier, allocator);
    object.AddMember("Chi Cost", attack.chiCost, allocator);
    object.AddMember("Bleed", attack.bleed, allocator);
    object.AddMember("Static", attack.staticAnimation, allocator);
    document.PushBack(object, allocator);

    // Save the JSON document to a file
    std::ofstream ofs(attackPath);
    if (ofs.is_open()) {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        ofs << buffer.GetString();
        ofs.close();
    }
}

/**
 * @brief Load Attack
 *
 * This function loads an attack into the system
 */
void AttackList::LoadAttack(std::string attackPath) {
    Attack atk{};
    std::string atkname{};
    rapidjson::Document document;
    std::ifstream file(attackPath);
    
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << attackPath << "\n";
        ASSERT(1, "Unable to open attack file!");
        return;
    }
    rapidjson::IStreamWrapper isw(file);
    document.ParseStream(isw);

    if (document.HasParseError()) {
        std::cerr << "Failed to parse attack file: " << attackPath << "\n";
        ASSERT(1, "Unable to parse attack file!");
        return;
    }

    for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
        const rapidjson::Value& mainObject = document[i];

        if (mainObject.HasMember("Name")) {
            const rapidjson::Value& object = mainObject["Name"];
            atkname = object.GetString();
            atk.attackName = atkname;
        }

        if (mainObject.HasMember("Texture")) {
            const rapidjson::Value& object = mainObject["Texture"];
            atk.skillTexture = object.GetString();
        }

        if (mainObject.HasMember("Tooltip")) {
            const rapidjson::Value& object = mainObject["Tooltip"];
            atk.skillTooltip = object.GetString();
        }

        if (mainObject.HasMember("Type")) {
            const rapidjson::Value& object = mainObject["Type"];
            atk.attacktype = (AttackType)object.GetInt();
        }

        if (mainObject.HasMember("Skill Attack")) {
            const rapidjson::Value& object = mainObject["Skill Attack"];
            atk.skillAttackPercent = object.GetInt();
        }

        if (mainObject.HasMember("Minimum Attack Multiplier")) {
            const rapidjson::Value& object = mainObject["Minimum Attack Multiplier"];
            atk.minAttackMultiplier = object.GetFloat();
        }

        if (mainObject.HasMember("Maximum Attack Multiplier")) {
            const rapidjson::Value& object = mainObject["Maximum Attack Multiplier"];
            atk.maxAttackMultiplier = object.GetFloat();
        }

        if (mainObject.HasMember("Crit Rate")) {
            const rapidjson::Value& object = mainObject["Crit Rate"];
            atk.critRate = object.GetFloat();
        }

        if (mainObject.HasMember("Crit Multiplier")) {
            const rapidjson::Value& object = mainObject["Crit Multiplier"];
            atk.critMultiplier = object.GetFloat();
        }

        if (mainObject.HasMember("Chi Cost")) {
            const rapidjson::Value& object = mainObject["Chi Cost"];
            atk.chiCost = object.GetInt();
        }

        if (mainObject.HasMember("Bleed")) {
            const rapidjson::Value& object = mainObject["Bleed"];
            atk.bleed = object.GetInt();
        }

        if (mainObject.HasMember("Static")) {
            const rapidjson::Value& object = mainObject["Static"];
            atk.staticAnimation = object.GetBool();
        }
    }
    data[atkname] = atk;
}

/**
 * @brief Load all attack
 *
 * This function loads all attacks found in the skills folder into the system
 */
void AttackList::LoadAllAttacks() {
    std::filesystem::path skillFolder{ assetmanager.GetDefaultPath() + "Skills/" };
    for (auto& entry : std::filesystem::directory_iterator(skillFolder)) {
        std::string path{ entry.path().string() };
        LoadAttack(path);
    }
}
