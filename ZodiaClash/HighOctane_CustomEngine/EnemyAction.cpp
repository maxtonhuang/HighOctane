#include "EnemyAction.h"


void EnemyAction::Start() 
{
    // moveSpeed = 50f;
    // startPosition = transform.position;

    /*battleManager = FindObjectOfType<BattleManager>();
    characterStats = GetComponent<CharacterStats>();
    vmath::Vector3 offset = new(10f, 0, 0);
    transform.position = startPosition + offset;*/

    enemyState = START;
}

void EnemyAction::EnemyRefreshTargets() 
{
    targetSelect.enemyTargets = characterStats.gameObject.FindGameObjectsWithTag("Enemy");
    targetSelect.playerTargets = characterStats.gameObject.FindGameObjectsWithTag("Player");
}

EntityState EnemyAction::EnemyAttackStartDelay(float startDelay, float endDelay)
{
    WaitForSeconds(startDelay); //walk delay before attacking
    EnemyApplySkill();
    WaitForSeconds(endDelay); //delay to play animation
    movingToStart = true;
    return ATTACKING;
}

EntityState EnemyAction::EnemyBuffStartDelay(float startDelay, float endDelay)
{
    WaitForSeconds(startDelay); //delay before buff
    EnemyApplySkill();
    WaitForSeconds(endDelay); //delay to play animation
    enemyEndingTurn = true;
    return WAITING; // Return a suitable state
}

EntityState EnemyAction::EnemyEndTurnDelay(float startDelay, float endDelay)
{
    // Wait until enemyEndingTurn becomes true
    while (!enemyEndingTurn)
    {
        WaitForSeconds(100);
    }
    WaitForSeconds(endDelay); // Assuming 'seconds' is defined elsewhere or replace with 'endDelay'
    enemyState = ENDING;
    return ENDING;
}