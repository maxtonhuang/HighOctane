#include "Attack.h"
#include "Battle.h"
#include <random>

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

        damage = (int)((
            std::max(minAttackMultiplier, maxAttackMultiplier) *
            ((float)skillAttackPercent / 100.f) * (owner->stats.attack * (100.f / (100.f + target.stats.defense)))
            * critMultiplier));
    }
    else
    {
        damage = (int)(
            std::max(minAttackMultiplier, maxAttackMultiplier) *
            ((float)skillAttackPercent / 100.f) * (owner->stats.attack * (100.f / (100.f + target.stats.defense))));
    }
}