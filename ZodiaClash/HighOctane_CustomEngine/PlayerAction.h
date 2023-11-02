//#pragma once
//#include "EnemyAction.h"

//class PlayerAction 
//{
//public:
//    EntityState playerState;
//    SkillSelection skillSelect;
//    TargetSelection targetSelect;
//    void SelectSkill(std::string btn);
//
//private:
//    void Start();
//
//protected:
//    BattleSystem* battleManager;
//    CharacterStats* characterStats;
//    bool playerAttacking;
//    bool endingTurn;
//    bool checkingStatus;
//    void UpdatePlayerState();
//    void RefreshTargets();
//    virtual void SelectTarget() {};
//    virtual void UseSkill() {};
//    virtual void AttackAnimation() {};
//    virtual void ApplySkill() {};
//    EntityState PlayerDeath();
//    EntityState AttackStartDelay(float startDelay, float endDelay);
//    EntityState BuffStartDelay(float startDelay, float endDelay);
//    EntityState EndTurnDelay(float seconds);
//};