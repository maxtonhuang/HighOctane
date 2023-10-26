#include "Attack.h"
#include "Battle.h"
#include "CharacterStats.h"
#include <random>

void Attack::UseAttack(CharacterStats* target) {
    CalculateDamage(*target);
    target->TakeDamage(damage);
}

void Attack::UseAttack(std::vector<CharacterStats*> target) {
    for (CharacterStats* t : target) {
        CalculateDamage(*t);
        t->TakeDamage(damage);
    }
}

void Attack::CalculateDamage(CharacterStats const& target)
{
    //attackerStats = owner.GetComponent<CharacterStats>();
    //targetStats = target.GetComponent<CharacterStats>();

    //critical hit chance
    std::default_random_engine rng;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    float randomValue = rand(rng);

    if (randomValue <= critRate)
    {
        //critical hit
        critCheck = true;

        damage = (std::max(minAttackMultiplier, maxAttackMultiplier) *
            ((float)skillAttackPercent / 100.f) * (owner->stats.attack * (100.f / (100.f + target.stats.defense)))
            * critMultiplier);
    }
    else
    {
        damage = (std::max(minAttackMultiplier, maxAttackMultiplier) *
            ((float)skillAttackPercent / 100.f) * (owner->stats.attack * (100.f / (100.f + target.stats.defense))));
    }
}