#include "CharacterStats.h"

class Attack {
public:
	void CalculateDamage(CharacterStats const& target);
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
