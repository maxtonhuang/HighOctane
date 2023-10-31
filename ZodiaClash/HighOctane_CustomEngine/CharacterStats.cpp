#include "CharacterStats.h"
#include "CharacterAction.h"
#include "PlayerAction.h"
#include "Battle.h"

CharacterStats::CharacterStats() {
    stats.health = stats.maxHealth;
    action.characterStats = this;
}

CharacterStats::CharacterStats(CharacterStats const& input) {
    checkedStatus = input.checkedStatus;
    action = input.action;
    entity = input.entity;
    stats = input.stats;
    tag = input.tag;
    action.characterStats = this;
    gameObject = input.gameObject;
    parent = input.parent;
    action.battleManager = input.action.battleManager;
}

void CharacterStats::Start()
{
    action.battleManager = parent;
    action.characterStats = this;
}

//void CharacterStats::Death(Entity& entity)
//{
    //if (tag == CharacterType::PLAYER)
    //{
    //    // Assuming you have a way to get the _PlayerAction component from the gameObject
    //    PlayerAction player = ECS::ecs().GetComponent<PlayerAction>(entity);
    //    player.playerState = DYING;
    //    
    //}
    //else if (tag == CharacterType::ENEMY)
    //{
    //    // Assuming you have a way to get the EnemyAction component from the gameObject
    //    EnemyAction enemy = ECS::ecs().GetComponent<EnemyAction>(entity);
    //    enemy.enemyState = DYING;
    //}
    
//}


void CharacterStats::TakeDamage(float damage) 
{
    if (parent->m_Entities.size() > 0) {
        std::string name{ ECS::ecs().GetComponent<Name>(entity).name };
        printf("%s took %f damage!\n", name.c_str(), damage);
    }
    stats.health -= damage;
    if (stats.health <= 0)
    {
        if (parent->m_Entities.size() > 0) {
            std::string name{ ECS::ecs().GetComponent<Name>(entity).name };
            Model& model{ ECS::ecs().GetComponent<Model>(entity) };
            printf("%s is dying!\n", name.c_str());
            model.SetColor(0.2f, 0.2f, 0.2f);
        }
        stats.health = 0;
        action.entityState = DYING;
        //Death(entity);
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

void WaitForSeconds(float seconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
}

bool CharacterStats::operator==(const CharacterStats& input) const {
    if (entity == input.entity) {
        return true;
    }
    return false;
}
