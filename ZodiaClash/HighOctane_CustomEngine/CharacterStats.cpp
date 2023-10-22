#include "CharacterStats.h"
#include "PlayerAction.h"

void CharacterStats::Start()
{
    // Assuming GameManager and BattleSystem are globally accessible or you have some other mechanism to get references
    // gameManager = GetGameManagerInstance();
    // battleManager = GetBattleSystemInstance();
    checkedStatus = false;
}

void CharacterStats::Death()
{
    //if (gameObject.tag == "Player")
    //{
    //    // Assuming you have a way to get the _PlayerAction component from the gameObject
    //    PlayerAction player = gameObject.GetComponent<_PlayerAction>();
    //    player.playerState = DYING;
    //}
    //else if (gameObject.tag == "Enemy")
    //{
    //    // Assuming you have a way to get the EnemyAction component from the gameObject
    //    EnemyAction enemy = gameObject.GetComponent<EnemyAction>();
    //    enemy.enemyState = DYING;
    //}
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