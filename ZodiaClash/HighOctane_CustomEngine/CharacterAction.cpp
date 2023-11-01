#include "CharacterAction.h"
#include "CharacterStats.h"

void CharacterAction::UpdateState() {
    std::string name;
    switch (entityState) {
    case START:
        entityState = WAITING;
        name = ECS::ecs().GetComponent<Name>(characterStats->entity).name;
        //DEBUG_PRINT("Current Turn: %s", name.c_str());
        if (battleManager->m_Entities.size() > 0) {
            printf("\nCurrent Turn %s\n", name.c_str());
        }
        RefreshTargets();
        break;
    case WAITING:
        //Empty by design, to wait for other systems to change entity state
        break;
    case ATTACKING:
        ApplySkill();
        entityState = ENDING;
        break;
    }
}

void CharacterAction::RefreshTargets() {
    std::vector<CharacterStats*> playerTargets{};
    std::vector<CharacterStats*> enemyTargets{};

    targetSelect.playerTargets = battleManager->GetPlayers();
    targetSelect.enemyTargets = battleManager->GetEnemies();
}

void CharacterAction::ApplySkill() {
    selectedSkill.SetOwner(this->characterStats);
    RefreshTargets();
    if (battleManager->m_Entities.size() > 0) {
        printf("Using skill: %s\n", selectedSkill.attackName.c_str());
    }
    
    if (selectedSkill.attacktype == AttackType::NORMAL) {
        selectedSkill.UseAttack(targetSelect.selectedTarget);
    }
    else if(selectedSkill.attacktype == AttackType::AOE) {
        if (characterStats->tag == CharacterType::PLAYER) {
            selectedSkill.UseAttack(targetSelect.enemyTargets);
        }
        else {
            selectedSkill.UseAttack(targetSelect.playerTargets);
        }
    }
    
}
