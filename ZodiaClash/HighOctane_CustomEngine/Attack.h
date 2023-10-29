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
