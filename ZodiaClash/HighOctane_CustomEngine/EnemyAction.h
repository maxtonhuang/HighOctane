//#pragma once
//#include "Battle.h"
//#include "CharacterStats.h"
//#include "vmath.h"
//#include <thread>
//#include <chrono>

//class EnemyAction
//{
//public:
//	EntityState enemyState;
//	SkillSelection skillSelect;
//	TargetSelection targetSelect;
//
//protected:
//	BattleSystem* battleSystem;
//	CharacterStats* characterStats;
//	bool enemyAttacking;
//	bool enemyEndingTurn;
//	bool checkingStatus;
//	bool movingToStart;
//	void EnemyRefreshTargets();
//	virtual void UpdateEnemyState() {};
//	virtual void EnemySelection() {};
//	virtual void EnemyUseSkill() {};
//	virtual void EnemyAttackAnimation() {};
//	virtual void EnemyApplySkill() {};
//	virtual EntityState EnemyDeath() { return EntityState{}; };
//
//	EntityState EnemyAttackStartDelay(float startDelay, float endDelay);
//	EntityState EnemyBuffStartDelay(float startDelay, float endDelay);
//	EntityState EnemyEndTurnDelay(float startDelay, float endDelay);
//
//private:
//	void Start();
//};
//
//void WaitForSeconds(float seconds);
