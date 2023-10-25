#include "CharacterStats.h"
#include "PlayerAction.h"
#include "Battle.h"

void CharacterStats::Start()
{
    checkedStatus = false;
}

void CharacterStats::Death(Entity& entity)
{
    if (gameObject.tag.tag == "Player")
    {
        // Assuming you have a way to get the _PlayerAction component from the gameObject
        PlayerAction player = ECS::ecs().GetComponent<PlayerAction>(entity);
        player.playerState = DYING;
    }
    else if (gameObject.tag.tag == "Enemy")
    {
        // Assuming you have a way to get the EnemyAction component from the gameObject
        EnemyAction enemy = ECS::ecs().GetComponent<EnemyAction>(entity);
        enemy.enemyState = DYING;
    }
}


void CharacterStats::TakeDamage(float damage, Entity& entity) 
{
    stats.health -= damage;
    if (stats.health <= 0)
    {
        stats.health = 0;
        Death(entity);
    }
}

void CharacterStats::HealBuff(float buffAmount) 
{
    stats.health += buffAmount;

    if (stats.health >= stats.maxHealth)
    {
        stats.health = stats.maxHealth;
    }
}