#include "PlayerAction.h"
#include "debugdiagnostic.h"


void PlayerAction::Start()
{
    playerState = WAITING;

   /* battleManager = FindObjectOfType<BattleManager>();
    characterStats = GetComponent<CharacterStats>();*/

}

void PlayerAction::UpdatePlayerState()
{
    if (battleManager.battleState == BattleState::PLAYERTURN && battleManager.turnManage.activePlayer == characterStats.gameObject.name)
    {
        if (playerState == WAITING)
        {
            playerState = CHECKSTATUS;
        }

        else if (playerState == CHECKSTATUS){}

        else if (playerState == SELECTION)
        {
            RefreshTargets();
            SelectTarget();
        }

        else if (playerState == ATTACKING)
        {
            // ToggleSkillUi(false);
            if (!playerAttacking)
            {
                // ToggleSkillText(true);
                UseSkill();
            }
        }

        else if (playerState == ENDING)
        {
            battleManager.battleState = BattleState::NEXTTURN;
            // skill
            skillSelect.selectedSkillPrefab = GameObject(); // Resetting to a default GameObject
            targetSelect.selectedTarget.clear(); // Clearing the selected target

            playerAttacking = false;
            endingTurn = false;
            // characterStats.checkedStatus = false;

            playerState = WAITING;
        }
    }

    if (playerState == DYING)
    {
        PlayerDeath(); // Removed StartCoroutine and called the function directly
    }
}


void PlayerAction::RefreshTargets()
{
    targetSelect.playerTargets = GameObject::FindGameObjectsWithTag("Player");
    targetSelect.enemyTargets = GameObject::FindGameObjectsWithTag("Enemy");
}

void PlayerAction::SelectSkill(std::string btn)
{
    //show specific target ui for specific skillPrefabs in override methods

    if (btn == "skill1")
    {
        skillSelect.selectedSkillPrefab = skillSelect.skill1Prefab;
    }
    else if (btn == "skill2")
    {
        skillSelect.selectedSkillPrefab = skillSelect.skill2Prefab;
    }
    else if (btn == "skill3")
    {
        skillSelect.selectedSkillPrefab = skillSelect.skill3Prefab;
    }
    std::string output = "Player Skill Chosen: " + skillSelect.selectedSkillPrefab.name + "\n";
    DEBUG_PRINT("%s", output);
}

EntityState PlayerAction::PlayerDeath()
{
    characterStats.gameObject.name = "Dead";

    WaitForSeconds(1); // 1 second
    playerState = ENDING;

    WaitForSeconds(0.5); // 500 milliseconds

    characterStats.gameObject.SetActive(false);

    return playerState;
}

EntityState PlayerAction::AttackStartDelay(float startDelay, float endDelay)
{
    WaitForSeconds(startDelay); //delay before attacking

    ApplySkill();

    WaitForSeconds(endDelay); //delay to play animation

    endingTurn = true;

    return playerState; // Return the current state or adjust as needed
}

EntityState PlayerAction::BuffStartDelay(float startDelay, float endDelay)
{
    WaitForSeconds(startDelay); //delay before buff

    ApplySkill();

    WaitForSeconds(endDelay); //delay to play animation

    endingTurn = true;

    return playerState; // Return the current state or adjust as needed
}

EntityState PlayerAction::EndTurnDelay(float seconds)
{
    while (!endingTurn)
    {
        WaitForSeconds(0.01); // Check every 10 milliseconds
    }

    WaitForSeconds(seconds);

    playerState = ENDING;

    return playerState;
}
