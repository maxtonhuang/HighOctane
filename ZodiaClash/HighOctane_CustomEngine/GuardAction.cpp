#include "EnemyAction.h"
#include <random>

class GuardAction : EnemyAction
{
private:
	void Update()
	{
		UpdateEnemyState();
	}

protected:
    void UpdateEnemyState() override {
        if (battleSystem.battleState == ENEMYTURN && battleSystem.turnManage.activeEnemy == characterStats.gameObject.name) {
            if (enemyState == WAITING) {
                enemyState = CHECKSTATUS;
            }
            else if (enemyState == SELECTION) {
                EnemyRefreshTargets();
                EnemySelection();
            }
            else if (enemyState == ATTACKING) {
                if (!enemyAttacking) {
                    // EnemyToggleSkillText(true);
                    EnemyUseSkill();
                }
                // EnemyMovement();
            }
            else if (enemyState == ENDING) {
                battleSystem.battleState = NEXTTURN;
                // skill
                skillSelect.selectedSkillPrefab.isnull = true;  // Marking the character as inactive or "dead"
                targetSelect.selectedTarget.clear();

                // target
                targetSelect.playerTargets.clear();
                enemyAttacking = false;
                enemyEndingTurn = false;
                enemyState = WAITING;
            }
        }
        if (enemyState == DYING) {
            EnemyDeath(); // Replaced StartCoroutine with direct call
        }
    }


    void EnemySelection() {
        // Generate a random index
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, targetSelect.playerTargets.size() - 1);

        int randomIndex = distrib(gen);

        // Assuming selectedTarget is a vector and we're setting its first element
        targetSelect.selectedTarget[0] = targetSelect.playerTargets[randomIndex];

        std::string output = "Enemy Selected Target: " + targetSelect.selectedTarget[0]->name + "\n";
        //debuglog::Logger::debug(output);

        skillSelect.selectedSkillPrefab = skillSelect.skill1Prefab;
        enemyState = ATTACKING;
    }

	void EnemyUseSkill() override
	{
        enemyAttacking = true;
        EnemyAttackAnimation();
	}

    void EnemyAttackAnimation() override
    {
        EnemyAttackStartDelay(0.5f, 1.0f); 
    }

	void EnemyApplySkill() override
	{
        EnemyEndTurnDelay(0.5f, 0.5f);
	}

    EntityState EnemyDeath() override
    {
        characterStats.gameObject.tag.tag = "Dead";
        WaitForSeconds(500); 

        enemyState = ENDING;
        WaitForSeconds(500);

        characterStats.gameObject.SetActive(false); //hide game object

        return enemyState;
    }

};