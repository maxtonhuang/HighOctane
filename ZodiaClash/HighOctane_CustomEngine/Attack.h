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
private:
    CharacterStats* owner;

    int skillAttackPercent;

    float minAttackMultiplier;
    float maxAttackMultiplier;
    float critRate;
    float critMultiplier;
    bool critCheck;

    float damage;
};
