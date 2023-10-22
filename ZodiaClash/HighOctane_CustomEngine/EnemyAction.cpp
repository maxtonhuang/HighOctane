//#include "EnemyAction.h"
//
//void EnemyAction::Start() 
//{
//    // moveSpeed = 50f;
//    // startPosition = transform.position;
//
//    battleManager = FindObjectOfType<BattleManager>();
//    characterStats = GetComponent<CharacterStats>();
//    Vector3 offset = new(10f, 0, 0);
//    transform.position = startPosition + offset;
//
//    enemyState = EnemyState.START;
//}
//
//void EnemyAction::UpdateEnemyState() 
//{
//
//}
//void EnemyAction::EnemyRefreshTargets() 
//{
//    enemyTargets = GameObject.FindGameObjectsWithTag("Enemy");
//    playerTargets = GameObject.FindGameObjectsWithTag("Player");
//}
//void EnemyAction::EnemySelection() 
//{
//
//}
//void EnemyAction::EnemyUseSkill() 
//{
//
//}
//void EnemyAction::EnemyAttackAnimation() 
//{
//
//}
//void EnemyAction::EnemyApplySkill() 
//{
//
//}
//EnemyState EnemyAction::EnemyDeath() 
//{
//
//}
//EnemyState EnemyAction::EnemyAttackStartDelay(float startDelay, float endDelay) 
//{
//    yield return new WaitForSeconds(startDelay); //walk delay before attacking
//
//    EnemyApplySkill();
//
//    yield return new WaitForSeconds(endDelay); //delay to play animation
//
//    movingToStart = true;
//}
//EnemyState EnemyAction::EnemyBuffStartDelay(float startDelay, float endDelay) 
//{
//    yield return new WaitForSeconds(startDelay); //delay before buff
//
//    EnemyApplySkill();
//
//    yield return new WaitForSeconds(endDelay); //delay to play animation
//
//    enemyEndingTurn = true;
//}
//EnemyState EnemyAction::EnemyEndTurnDelay(float startDelay, float endDelay) 
//{
//    yield return new WaitUntil(() = > enemyEndingTurn);
//
//    yield return new WaitForSeconds(seconds);
//
//    enemyState = EnemyState.ENDING;
//}