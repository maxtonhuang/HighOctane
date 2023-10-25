#include "CharacterAction.h"
#include "CharacterStats.h"

void CharacterAction::UpdateState() {
    switch (entityState) {
    case START:
        RefreshTargets();
        entityState = WAITING;
        break;
    case WAITING:
        //Empty by design, to wait for other systems to change entity state
        break;
    case ATTACKING:
        ApplySkill();
        entityState = ENDING;
        break;
    }
    //if (entityState == WAITING)
    //{
    //    entityState = CHECKSTATUS;
    //}

    //else if (entityState == CHECKSTATUS) {}

    //else if (entityState == SELECTION)
    //{
    //    RefreshTargets();
    //    //SelectTarget();
    //}

    //else if (entityState == ATTACKING)
    //{
    //    // ToggleSkillUi(false);
    //    if (!playerAttacking)
    //    {
    //        // ToggleSkillText(true);
    //        //UseSkill();
    //    }
    //}

    //else if (entityState == ENDING)
    //{
    //    battleManager->battleState = BattleState::NEXTTURN;
    //    // skill
    //    skillSelect.selectedSkillPrefab = GameObject(); // Resetting to a default GameObject
    //    targetSelect.selectedTarget.clear(); // Clearing the selected target

    //    playerAttacking = false;
    //    endingTurn = false;
    //    // characterStats.checkedStatus = false;

    //    entityState = WAITING;
    //}
}

void CharacterAction::RefreshTargets() {
    std::vector<CharacterStats*> playerTargets{};
    std::vector<CharacterStats*> enemyTargets{};
    for (CharacterStats& c : battleManager->turnManage.characterList) {
        if (c.tag == CharacterType::PLAYER) {
            playerTargets.push_back(&c);
        }
        else if (c.tag == CharacterType::ENEMY) {
            enemyTargets.push_back(&c);
        }
    }
    targetSelect.playerTargets = playerTargets;
    targetSelect.enemyTargets = enemyTargets;
}

void CharacterAction::ApplySkill() {
    if (selectedSkill->attacktype == AttackType::NORMAL) {
        selectedSkill->UseAttack(targetSelect.selectedTarget);
    }
    else if(selectedSkill->attacktype == AttackType::AOE) {
        if (characterStats->tag == CharacterType::PLAYER) {
            selectedSkill->UseAttack(targetSelect.enemyTargets);
        }
        else {
            selectedSkill->UseAttack(targetSelect.playerTargets);
        }
    }
}
