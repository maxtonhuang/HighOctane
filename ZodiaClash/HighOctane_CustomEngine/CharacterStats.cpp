#include "CharacterStats.h"

void CharacterStats::Start() 
{
   /* gameManager = FindObjectOfType<GameManager>();
    battleManager = FindObjectOfType<BattleSystem>();
    checkedStatus = false;*/
}

void CharacterStats::Death() 
{
   /* if (gameObject.CompareTag("Player"))
    {
        _PlayerAction player = GetComponent<_PlayerAction>();
        player.playerState = PlayerState.DYING;
    }
    else if (gameObject.CompareTag("Enemy"))
    {
        _EnemyAction enemy = GetComponent<_EnemyAction>();
        enemy.enemyState = EnemyState.DYING;
    }*/
}

void CharacterStats::TakeDamage(float damage) 
{
    stats.health -= damage;
    if (stats.health <= 0)
    {
        stats.health = 0;
        Death();
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