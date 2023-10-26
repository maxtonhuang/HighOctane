#pragma once
#include "CharacterCommon.h"
#include "Attack.h"
#include "Battle.h"

class CharacterAction
{
public:
    EntityState entityState;
    //SkillSelection skillSelect;
    TargetSelection targetSelect;
    std::vector<Attack> skills;
    Attack selectedSkill;
    //void SelectSkill(std::string btn);
    void UpdateState();
    BattleSystem* battleManager;
    CharacterStats* characterStats;
private:
    //void Start();

protected:
    bool playerAttacking;
    bool endingTurn;
    bool checkingStatus;
    
    void RefreshTargets();
    //virtual void SelectTarget() {};
    //virtual void UseSkill() {};
    //virtual void AttackAnimation() {};
    void ApplySkill();
    //EntityState PlayerDeath();
    //EntityState AttackStartDelay(float startDelay, float endDelay);
    //EntityState BuffStartDelay(float startDelay, float endDelay);
    //EntityState EndTurnDelay(float seconds);
};
