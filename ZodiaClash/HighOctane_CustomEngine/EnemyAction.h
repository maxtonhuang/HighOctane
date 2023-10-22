#pragma once
#include "CharacterStats.h"
#include "Battle.h"

enum EnemyState 
{
	START, WAITING, CHECKSTATUS, SELECTION, ATTACKING, ENDING, DYING
};

struct SkillSelection 
{
	GameObject skill1Prefab;
	GameObject skill2Prefab;
	GameObject skill3Prefab;
	GameObject selectedSkillPrefab;
};

struct TargetSelection 
{
	std::vector<GameObject> enemyTargets;
	std::vector<GameObject> playerTargets;
	std::vector<GameObject> selectedTarget;
};

class EnemyAction
{
public:
	EnemyState enemyState;
	SkillSelection skillSelect;
	TargetSelection targetSelect;

protected:
	BattleSystem battleSystem;
	CharacterStats characterStats;
	bool enemyAttacking;
	bool enemyEndingTurn;
	bool checkingStatus;
	virtual void UpdateEnemyState();
	void EnemyRefreshTargets();
	virtual void EnemySelection();
	virtual void EnemyUseSkill();
	virtual void EnemyAttackAnimation();
	virtual void EnemyApplySkill();
	virtual EnemyState EnemyDeath();
	EnemyState EnemyAttackStartDelay(float startDelay, float endDelay);
	EnemyState EnemyBuffStartDelay(float startDelay, float endDelay);
	EnemyState EnemyEndTurnDelay(float startDelay, float endDelay);
private:
	void Start();
};