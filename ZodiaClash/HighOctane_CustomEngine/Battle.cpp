/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		Battle.cpp
*
*	@author		Liu WanTing
*
*	@email		wanting.liu\@digipen.edu
*
* 	@author		Foong Pun Yuen Nigel (Animation functions)
*
*	@email		p.foong\@digipen.edu
*
* 
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 October 2023
*
* *****************************************************************************
*
*	@brief		Battle Scene Game Logic
*
*	This file contains the battle scene's game logic, and will be activated by
*	the Game State Manager (GSM).
*
******************************************************************************/

#include "Battle.h"
#include "GameStateManager.h"
#include "debuglog.h"
#include "Events.h"
#include "GameAITree.h"
#include "ECS.h"
#include <algorithm>
#include <iostream>
#include "CheatCode.h"
#include "message.h"
#include "EntityFactory.h"
#include "Animation.h"
#include "Camera.h"
#include "AssetManager.h"
#include "Utilities.h"
#include "Global.h"

//For animating skill buttons
const float skillButtonOffset{ 180.f };

//for animating health bats
const float healthBarOffset{ -120.f };

//For camera shake
const float MAGNITUDE_PER_HEALTH{ 2.f };

/**
 * @brief Constructor that copies the state of another BattleSystem instance.
 *
 * @param input A constant reference to another BattleSystem object.
 */
BattleSystem::BattleSystem(BattleSystem const& input) {
    battleState = input.battleState;
    roundManage = input.roundManage;
    turnManage.characterList = input.turnManage.characterList;
    for (CharacterStats& c : turnManage.characterList) {
        c.parent = this;
        c.Start();
        if (c.entity == input.activeCharacter->entity) {
            activeCharacter = &c;
        }
    }

    turnManage.turnOrderList = input.turnManage.turnOrderList;
    turnManage.originalTurnOrderList = input.turnManage.originalTurnOrderList;
    for (CharacterStats*& c : turnManage.turnOrderList) {
        for (CharacterStats& character : turnManage.characterList) {
            if (c->entity == character.entity) {
                c = &character;
                break;
            }
        }
    }
    for (CharacterStats*& c : turnManage.originalTurnOrderList) {
        for (CharacterStats& character : turnManage.characterList) {
            if (c->entity == character.entity) {
                c = &character;
                break;
            }
        }
    }
    roundInProgress = input.roundInProgress;
}

/**
 * @brief Initializes the battle system by setting the battle state to NEWGAME.
 */
void BattleSystem::Initialize() 
{
    battleState = NEWGAME;
    battlestarted = false;
    attackingAnimation = false;
    activeCharacter = nullptr;
    turnOrderQueueInitializer.clear();
    turnOrderQueueAnimator.clear();
    targetCircleList.clear();
}

/**
 * @brief Starts the battle, initializes event connections, and determines the turn order.
 */
void BattleSystem::StartBattle() {
    events.ConnectBattleSystem(this);
    roundInProgress = false;
    roundManage.characterCount = 0;
    roundManage.roundCounter = 0;
    chi = 5;

    if (DetermineTurnOrder() == false) {
        return;
    }

    for (auto& c : turnManage.characterList) {
        std::string name = ECS::ecs().GetComponent<Name>(c.entity).name;
    }

    //Initialise turn order animator
    InitialiseBattleUI();

    battlestarted = true;
}

/**
 * @brief Updates the state of the battle each frame, manages the game logic depending on the current battle state.
 */
void BattleSystem::Update() 
{
    int enemyAmount = 0;
    int playerAmount = 0;
    TreeManager gameAI{};
    
    // Access component arrays through the ComponentManager
    ComponentArray<CharacterStats>* statsArray{};
    ComponentArray<Model>* modelArray{};
    if (m_Entities.size() > 0) {

        UpdateTargets();

        //If animation system is playing battle animation, do not progress game system
        if (locked) {
            return;
        }
        for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::BATTLE]) {
            switch (msg.type) {
            case(TYPE::ANIMATING):
                Mail::mail().mailbox[ADDRESS::BATTLE].clear();
                return;
            }
        }

        ComponentManager& componentManager = ECS::ecs().GetComponentManager();
        statsArray = &componentManager.GetComponentArrayRef<CharacterStats>();
        modelArray = &componentManager.GetComponentArrayRef<Model>();

    }

    switch (battleState) {
    case NEWGAME:
        //LOG_WARNING("Initializing battle system");

        if (!battlestarted) {
            StartBattle();
        }
        
        if (!AnimateInitialiseTurnOrder()) {
            break;
        }

        InitialiseUIAnimation();

        battleState = NEWROUND;
        break;
    case NEWROUND:
        if (!roundInProgress)
        {
            roundInProgress = true;
            ++roundManage.roundCounter;

            battleState = NEXTTURN;
        }
        break;
    case NEXTTURN:

        //then check if player has won
        for (auto& c : turnManage.characterList) {
            if (c.tag == CharacterType::PLAYER) {
                playerAmount++;
            }
            else if (c.tag == CharacterType::ENEMY) {
                enemyAmount++;
            }
        }
        if (!enemyAmount) //no enemies left
        {
            if (m_Entities.size() > 0) {
                EntityFactory::entityFactory().ClonePrefab("wintext.prefab");
            }
            
            battleState = WIN;
        }
        //then check if enemy has won
        else if (!playerAmount) //no players left
        {
            if (m_Entities.size() > 0) {
                EntityFactory::entityFactory().ClonePrefab("losetext.prefab");
            }
            
            battleState = LOSE;
        }
        //continue battle
        else
        {
            if (roundManage.characterCount < turnManage.characterList.size())
            {
                activeCharacter = turnManage.turnOrderList.front();

                if (activeCharacter->tag == CharacterType::ENEMY)
                {
                    turnManage.activeEnemy = activeCharacter->gameObject.name;
                    turnManage.activePlayer = "";

                    activeCharacter->ApplyBloodStack(); //apply blood stack, the function will check if the enemy is on bloodstack
                    ProcessDamage();

                    // check if the enemy died after bloodstack effect
                    if (activeCharacter->stats.health <= 0) 
                    {
                        activeCharacter->action.entityState = EntityState::DYING;
                    }
                    if (goatSpeedup == true) {
                        CharacterStats* tmp{};
                        for (CharacterStats* character : activeCharacter->parent->GetEnemies()) {
                            if (character->entity != activeCharacter->entity) {
                                tmp = character;
                                break;
                            }
                        }
                        if (tmp->tag == CharacterType::ENEMYSPEDUP) {
                            activeCharacter->parent->RevertTurnOrder(tmp);
                        }
                        goatSpeedup = false;
                    }
                    //STILL NEED TO SWAP THE TURN TO ENEMY TURN TO PROCESS THEM DYING
                    battleState = ENEMYTURN;
                }
                else if (activeCharacter->tag == CharacterType::PLAYER)
                {
                    turnManage.activePlayer = activeCharacter->gameObject.name;
                    turnManage.activeEnemy = "";

                    battleState = PLAYERTURN;

                    if (m_Entities.size() > 0 && attackingAnimation == true) {
                        MoveInUIAnimation();
                    }
                }
                if (activeCharacter->action.entityState != EntityState::DYING) {
                    activeCharacter->action.entityState = EntityState::START;
                }
                ++roundManage.characterCount;
            }
            else
            {
                battleState = NEWROUND;

                activeCharacter->gameObject.isnull = true;
                turnManage.activeEnemy = "";
                turnManage.activePlayer = "";

                roundManage.characterCount = 0;
                roundInProgress = false;
            }
           
        }
        break;
    case ENEMYTURN:
        if (activeCharacter->action.entityState == WAITING) {
            gameAI.Search(this);
        }
        [[fallthrough]];

    case PLAYERTURN:
        activeCharacter->action.UpdateState();
        if (activeCharacter->action.entityState == EntityState::ENDING) {

            //Update turn order animator
            if (locked) {
                return;
            }

            if (!AnimateUpdateTurnOrder()) {
                return;
           }

            turnManage.turnOrderList.splice(turnManage.turnOrderList.end(), turnManage.turnOrderList, turnManage.turnOrderList.begin()); //SEND TO BACK OF TURN ORDER LIST

            //Process dead characters
            std::vector<CharacterStats*> deadchars{};
            for (CharacterStats* c : turnManage.turnOrderList) {
                if (c->stats.health == 0) {
                    deadchars.push_back(c);
                }
            }
            for (CharacterStats* c : deadchars) {
                turnManage.turnOrderList.remove(c);
                turnManage.originalTurnOrderList.remove(c);
                turnManage.characterList.remove(*c);
                battleState = NEXTTURN;
            }
            deadchars.clear();

            battleState = NEXTTURN;
        }
        else if (activeCharacter->action.entityState == EntityState::DYING) {
            //Process dead characters
            std::vector<CharacterStats*> deadchars{};
            for (CharacterStats* c : turnManage.turnOrderList) {
                if (c->stats.health == 0) {
                    deadchars.push_back(c);
                }
            }
            for (CharacterStats* c : deadchars) {
                turnManage.turnOrderList.remove(c);
                turnManage.originalTurnOrderList.remove(c);
                turnManage.characterList.remove(*c);
                battleState = NEXTTURN;
            }
            deadchars.clear();

            battleState = NEXTTURN;
        }
        break;
    }
}

/**
 * @brief Determines the turn order of characters based on their stats and adds them to the turn management lists.
 */
bool BattleSystem::DetermineTurnOrder()
{
    //charactersList
    //getting an array of CharacterStats component 
    ComponentArray<CharacterStats>& characters =  ECS::ecs().GetComponentManager().GetComponentArrayRef<CharacterStats>(); // GameObject::FindObjectsOfType();
    int enemyCount{ 0 };
    int allyCount{ 0 };

    turnManage.characterList.clear();

    for(auto& chara : m_Entities)
    {
        CharacterStats* m = &characters.GetData(chara);
        m->entity = chara;
        m->parent = this;
        m->action.battleManager = this;
        turnManage.characterList.push_back(*m);
        if (m->tag == CharacterType::PLAYER) {
            allyCount++;
        }
        else if (m->tag == CharacterType::ENEMY) {
            enemyCount++;
        }
    }

    if (allyCount < 1 || enemyCount < 1) {
        turnManage.characterList.clear();
        return false;
    }

    //turnOrderList
    turnManage.turnOrderList.clear();

    for(CharacterStats& chara : turnManage.characterList) //add the sorted charactersList to turnOrderList
    {
        turnManage.turnOrderList.push_back(&chara);
    }
    turnManage.turnOrderList.sort();

    //originalTurnOrderList
    turnManage.originalTurnOrderList = turnManage.turnOrderList;
    return true;
}

/**
 * @brief Retrieves a list of CharacterStats pointers to all player characters.
 *
 * @return std::vector<CharacterStats*> A vector containing pointers to the player characters.
 */
std::vector<CharacterStats*> BattleSystem::GetPlayers() {
    std::vector<CharacterStats*> output;
    for (auto& c : turnManage.characterList) {
        if (c.tag == CharacterType::PLAYER) {
            output.push_back(&c);
        }
    }
    return output;
}

/**
 * @brief Retrieves a list of CharacterStats pointers to all enemy characters.
 *
 * @return std::vector<CharacterStats*> A vector containing pointers to the enemy characters.
 */
std::vector<CharacterStats*> BattleSystem::GetEnemies() {
    std::vector<CharacterStats*> output;
    for (auto& c : turnManage.characterList) {
        if (c.tag == CharacterType::ENEMY) {
            output.push_back(&c);
        }
    }
    return output;
}

/**
 * @brief Moves a character's turn order immediately after the next character in the current turn order list.
 *
 * @param target A pointer to the CharacterStats of the target character.
 */
void BattleSystem::SwitchTurnOrder(CharacterStats* target)
{
    turnManage.turnOrderList.remove(target);
    auto iterator = turnManage.turnOrderList.begin();
    iterator++;
    turnManage.turnOrderList.insert(iterator, target);
}

/**
 * @brief Reverts the turn order change for a character to its original position.
 *
 * @param target A pointer to the CharacterStats of the target character.
 */
void BattleSystem::RevertTurnOrder(CharacterStats* target)
{
    auto& ogTurnList = turnManage.originalTurnOrderList;
    auto originalIndex = std::find(ogTurnList.begin(), ogTurnList.end(), target);
    turnManage.turnOrderList.remove(target);
    turnManage.turnOrderList.insert(originalIndex, target);
}

void BattleSystem::ProcessDamage() {
    if (m_Entities.size() > 0) {
        ComponentManager& componentManager = ECS::ecs().GetComponentManager();
        ComponentArray<CharacterStats>* statsArray = &componentManager.GetComponentArrayRef<CharacterStats>();
        ComponentArray<Model>* modelArray = &componentManager.GetComponentArrayRef<Model>();
        ComponentArray<AnimationSet>* animationArray = &componentManager.GetComponentArrayRef<AnimationSet>();
        ComponentArray<Transform>* transformArray = &componentManager.GetComponentArrayRef<Transform>();
        ComponentArray<TextLabel>* textArray = &componentManager.GetComponentArrayRef<TextLabel>();

        float totalDamage{ 0.f };
        for (Entity entity : m_Entities) {
            CharacterStats* cs = &statsArray->GetData(entity);
            if (cs->action.entityState == DEAD) {
                continue;
            }
            bool found = false;
            for (CharacterStats const& c : turnManage.characterList) {
                if (c.entity == entity) {
                    if (c.stats.health < cs->stats.health) {
                        if (c.stats.health != 0) {
                            animationArray->GetData(entity).Start("Damaged", entity);
                        }
                        else {
                            animationArray->GetData(entity).Start("Death", entity);
                        }
                        Entity damagelabel{ EntityFactory::entityFactory().ClonePrefab("damagelabel.prefab") };
                        transformArray->GetData(damagelabel).position = transformArray->GetData(entity).position;
                        textArray->GetData(damagelabel).textString = std::to_string((int)(cs->stats.health - c.stats.health));
                        totalDamage += cs->stats.health - c.stats.health;
                    }
                    *cs = c;
                    found = true;
                    break;
                }
            }
            //Death animation and logic
            if (found == false) {
                Model* model = &modelArray->GetData(entity);
                cs->action.entityState = DEAD;
                cs->debuffs.bloodStack = 0;
                model->SetAlpha(0.2f);
                AnimateRemoveTurnOrder(entity);
                AnimateRemoveHealthBar(entity);
                if (cs->tag == CharacterType::PLAYER) {
                    std::vector<CharacterAnimator> newAnimators{};
                    for (CharacterAnimator& ca : allyAnimators) {
                        if (ca.character != cs->entity) {
                            newAnimators.push_back(ca);
                        }
                    }
                    allyAnimators = newAnimators;
                }
                else {
                    std::vector<CharacterAnimator> newAnimators{};
                    for (CharacterAnimator& ca : enemyAnimators) {
                        if (ca.character != cs->entity) {
                            newAnimators.push_back(ca);
                        }
                    }
                    enemyAnimators = newAnimators;
                }
            }
        }
        if (totalDamage > 0.f) {
            camera.SetShake(totalDamage / MAGNITUDE_PER_HEALTH);
        }
    }
    
}

void BattleSystem::InitialiseBattleUI() {
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static auto& turnorderArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<TurnIndicator>() };
    if (m_Entities.size() > 0) {
        skillButtons.clear();
        enemyAnimators.clear();
        allyAnimators.clear();
        allyHealthBars.clear();
        enemyHealthBars.clear();
        allBattleUI.clear();

        InitialiseTurnOrderAnimator();

        //Initialise chi system
        chiLabel = EntityFactory::entityFactory().ClonePrefab("skillpointsys.prefab");
        allBattleUI.push_back(chiLabel);

        //Initialise skill buttons
        float skill_offset{ 0.f };
        for (int i = 0; i < 3; i++) {
            Entity skill{ EntityFactory::entityFactory().ClonePrefab("attack_skill.prefab") };
            ECS::ecs().GetComponent<Transform>(skill).position.x += skill_offset;
            std::stringstream skillNumber{};
            skillNumber << i + 1;
            ECS::ecs().GetComponent<Button>(skill).eventInput = skillNumber.str();
            ECS::ecs().GetComponent<AttackSkill>(skill).skillIndex = i;
            skillButtons.push_back(skill);
            allBattleUI.push_back(skill);
            skill_offset += skillButtonOffset;
        }

        //Initialise player healthbars
        float ally_hp_offset{ 0.f };
        for (CharacterStats* c : GetPlayers()) {
            Entity healthbar{ EntityFactory::entityFactory().ClonePrefab("ally_healthbar.prefab") };
            ECS::ecs().GetComponent<Transform>(healthbar).position.y += ally_hp_offset;
            //ECS::ecs().GetComponent<HealthBar>(healthbar).charaStatsRef = c;
            ECS::ecs().GetComponent<HealthBar>(healthbar).entity = c->entity;
            ECS::ecs().GetComponent<HealthBar>(healthbar).charaStatsRef = &ECS::ecs().GetComponent<CharacterStats>(c->entity);
            allyHealthBars.push_back(healthbar);
            allBattleUI.push_back(healthbar);
            ally_hp_offset += healthBarOffset;

            CharacterAnimator animator{};
            animator.character = c->entity;
            animator.healthbar = healthbar;
            animator.healthbarIcon = parentArray.GetData(healthbar).GetChildByName("hpBarIcon");
            animator.healthbarBase = parentArray.GetData(healthbar).GetChildByName("hpBarBase");
            for (Entity& turnUI : turnOrderQueueInitializer) {
                if (turnorderArray.GetData(turnUI).character == animator.character) {
                    animator.turnorder = turnUI;
                    animator.turnorderIcon = parentArray.GetData(turnUI).GetChildByName("turnOrderIcon");
                    break;
                }
            }
            allyAnimators.push_back(animator);
        }

        //Initialise enemy healthbars
        float enemy_hp_offset{ 0.f };
        for (CharacterStats* c : GetEnemies()) {
            Entity healthbar{ EntityFactory::entityFactory().ClonePrefab("enemy_healthbar.prefab") };
            ECS::ecs().GetComponent<Transform>(healthbar).position.y += enemy_hp_offset;
            //ECS::ecs().GetComponent<HealthBar>(healthbar).charaStatsRef = c;
            ECS::ecs().GetComponent<HealthBar>(healthbar).entity = c->entity;
            ECS::ecs().GetComponent<HealthBar>(healthbar).charaStatsRef = &ECS::ecs().GetComponent<CharacterStats>(c->entity);
            enemyHealthBars.push_back(healthbar);
            allBattleUI.push_back(healthbar);
            enemy_hp_offset += healthBarOffset;

            CharacterAnimator animator{};
            animator.character = c->entity;
            animator.healthbar = healthbar;
            animator.healthbarIcon = parentArray.GetData(healthbar).GetChildByName("hpBarIcon");
            animator.healthbarBase = parentArray.GetData(healthbar).GetChildByName("hpBarBase");
            for (Entity& turnUI : turnOrderQueueInitializer) {
                if (turnorderArray.GetData(turnUI).character == animator.character) {
                    animator.turnorder = turnUI;
                    animator.turnorderIcon = parentArray.GetData(turnUI).GetChildByName("turnOrderIcon");
                    break;
                }
            }
            enemyAnimators.push_back(animator);
        }
    }
}

void BattleSystem::InitialiseTurnOrderAnimator() {
    if (m_Entities.size() > 0 && !ECS::ecs().EntityExists(turnOrderAnimator)) {
        turnOrderAnimator = EntityFactory::entityFactory().ClonePrefab("turnorderattach.prefab");
        //ECS::ecs().GetComponent<Transform>(turnOrderAnimator).position.y -= turnOrderOffset * turnManage.characterList.size();
        for (auto& c : turnManage.characterList) {
            if (c.tag == CharacterType::PLAYER) {
                Entity turnUI{ EntityFactory::entityFactory().ClonePrefab("turn_ally.prefab") };
                turnOrderQueueInitializer.push_back(turnUI);
                //allBattleUI.push_back(turnUI);
                ECS::ecs().GetComponent<TurnIndicator>(turnUI).character = c.entity;
            }
            else if (c.tag == CharacterType::ENEMY) {
                Entity turnUI{ EntityFactory::entityFactory().ClonePrefab("turn_enemy.prefab") };
                turnOrderQueueInitializer.push_back(turnUI);
                //allBattleUI.push_back(turnUI);
                ECS::ecs().GetComponent<TurnIndicator>(turnUI).character = c.entity;
            }
        }
    }
}

bool BattleSystem::AnimateInitialiseTurnOrder() {
    if (m_Entities.size() > 0) {
        if (!turnOrderQueueInitializer.empty()) {
            ECS::ecs().GetComponent<AnimationSet>(turnOrderQueueInitializer.front()).Start("Shift In", turnOrderQueueInitializer.front());
            for (Entity& e : turnOrderQueueAnimator) {
                if (turnOrderQueueInitializer.size() == 1 && e == turnOrderQueueAnimator.front()) {
                    ECS::ecs().GetComponent<AnimationSet>(e).Start("Expand", e);
                    Entity icon{ ECS::ecs().GetComponent<Parent>(e).GetChildByName("turnOrderIcon") };
                    ECS::ecs().GetComponent<AnimationSet>(icon).Start("Expand", icon);
                }
                else {
                    ECS::ecs().GetComponent<AnimationSet>(e).Start("Next Turn", e);
                }
            }
            turnOrderQueueAnimator.push_back(turnOrderQueueInitializer.front());
            turnOrderQueueInitializer.pop_front();
            return false;
        }
    }
    return true;
}

bool BattleSystem::AnimateUpdateTurnOrder() {
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    if (m_Entities.size() > 0) {
        static bool turnanimated{ false };
        if (turnanimated == false) {
            Entity queueFront{ turnOrderQueueAnimator.front() };
            ECS::ecs().GetComponent<AnimationSet>(turnOrderQueueAnimator.front()).Start("Pop Out", turnOrderQueueAnimator.front());
            Entity retractingIcon{ parentArray.GetData(turnOrderQueueAnimator.front()).GetChildByName("turnOrderIcon") };
            ECS::ecs().GetComponent<AnimationSet>(retractingIcon).Start("Unexpand", retractingIcon);
            turnOrderQueueAnimator.pop_front();

            ECS::ecs().GetComponent<AnimationSet>(turnOrderQueueAnimator.front()).Start("Expand", turnOrderQueueAnimator.front());
            Entity expandingIcon = parentArray.GetData(turnOrderQueueAnimator.front()).GetChildByName("turnOrderIcon");
            ECS::ecs().GetComponent<AnimationSet>(expandingIcon).Start("Expand", expandingIcon);
            for (Entity& e : turnOrderQueueAnimator) {
                if (e != turnOrderQueueAnimator.front()) {
                    ECS::ecs().GetComponent<AnimationSet>(e).Start("Next Turn", e);
                }
            }
            turnOrderQueueAnimator.push_back(queueFront);
            
            turnanimated = true;
            return false;
        }
        else {
            ECS::ecs().GetComponent<AnimationSet>(turnOrderQueueAnimator.back()).Start("Shift In", turnOrderQueueAnimator.back());
            turnanimated = false;
        }
    }
    return true;
}

void BattleSystem::InitialiseUIAnimation() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    if (m_Entities.size() > 0) {
        UpdateSkillIcons();
        for (Entity& e : allBattleUI) {
            animationArray.GetData(e).Start("Pop In", e);
        }
    }
}

void BattleSystem::AnimateRemoveTurnOrder(Entity entity) {
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    if (m_Entities.size() > 0) {
        std::deque<Entity> newTurnOrderQueueAnimator{};
        bool moveup{ false };
        for (Entity e : turnOrderQueueAnimator) {
            if (ECS::ecs().GetComponent<TurnIndicator>(e).character == entity) {
                ECS::ecs().GetComponent<AnimationSet>(e).Start("Pop Out", e);
                Entity icon{ parentArray.GetData(e).GetChildByName("turnOrderIcon") };
                ECS::ecs().GetComponent<AnimationSet>(icon).Start("Unexpand", icon);
                moveup = true;
                continue;
            }
            newTurnOrderQueueAnimator.push_back(e);
            if (moveup) {
                //ECS::ecs().GetComponent<AnimationSet>(e).Start("Next Turn", e);
            }
        }
        turnOrderQueueAnimator = newTurnOrderQueueAnimator;
    }
}

void BattleSystem::CreateTargets() {
    auto enemyList{ GetEnemies() };
    if (!targetCircleList.empty()) {
        DestroyTargets();
    }
    int count = 0;
    for (CharacterStats* enemy : enemyList) {
        Entity targetcircle{ EntityFactory::entityFactory().ClonePrefab("targetcircle.prefab") };
        ECS::ecs().GetComponent<Transform>(targetcircle).position = ECS::ecs().GetComponent<Transform>(enemy->entity).position;
        if (activeCharacter->action.selectedSkill.attacktype == AttackType::AOE) {
            ECS::ecs().GetComponent<Model>(targetcircle).SetColor(1.f, 0.f, 0.f);
        }
        ECS::ecs().GetComponent<Button>(targetcircle).eventInput = std::to_string(count);
        targetCircleList.push_back(targetcircle);
        count++;
    }
}

void BattleSystem::DestroyTargets() {
    for (Entity target : targetCircleList) {
        EntityFactory::entityFactory().DeleteCloneModel(target);
    }
    targetCircleList.clear();
}

void BattleSystem::UpdateTargets() {
    static auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
    static auto& buttonArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Button>() };
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static vmath::Vector2 mousePos{ RESET_VEC2 };

    if (targetCircleList.empty()) {
        return;
    }

    // get cursorPos, compare with pos in Transform, return if no match
    for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
        switch (msg.type) {
        case(TYPE::MOUSE_MOVE):
            mousePos = { msg.posX, msg.posY };
            break;
        }
    }

    bool aoe{ activeCharacter->action.selectedSkill.attacktype == AttackType::AOE };
    int selected{ -1 };

    for (int i = 0; i < targetCircleList.size(); i++) {
        Entity currentTarget{ targetCircleList[i] };
        Model& targetModel{ modelArray.GetData(currentTarget) };
        Button& targetButton{ buttonArray.GetData(currentTarget) };
        targetButton.defaultColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
        targetButton.hoveredColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
        if (IsWithinObject(targetModel, mousePos)) {
            selected = i;
        }
    }

    for (int i = 0; i < targetCircleList.size(); i++) {
        Entity currentTarget{ targetCircleList[i] };
        Button& targetButton{ buttonArray.GetData(currentTarget) };
        Entity hpIcon{ enemyAnimators[i].healthbarIcon };
        AnimationSet& iconAnimation{ animationArray.GetData(hpIcon) };
        Entity turnIcon{ enemyAnimators[i].turnorderIcon };
        AnimationSet& turniconAnimation{ animationArray.GetData(turnIcon)};
        Entity turn{ enemyAnimators[i].turnorder };
        AnimationSet& turnAnimation{ animationArray.GetData(turn) };
        Entity hpBase{ enemyAnimators[i].healthbarBase };
        AnimationSet& hpbaseAnimation{ animationArray.GetData(hpBase) };
        

        if ((aoe && selected >= 0) || (selected == i)) {
            targetButton.defaultColor.buttonColor = glm::vec4{ 1.f,0.f,0.f,1.f };
            targetButton.hoveredColor.buttonColor = glm::vec4{ 1.f,0.f,0.f,1.f };

            if (iconAnimation.activeAnimation == nullptr) {
                iconAnimation.Start("Glow", hpIcon);
            }

            if (turniconAnimation.activeAnimation == nullptr) {
                turniconAnimation.Start("Glow", turnIcon);
            }

            if (turnAnimation.activeAnimation == nullptr) {
                turnAnimation.Start("Glow", turn);
            }

            if (hpbaseAnimation.activeAnimation == nullptr) {
                hpbaseAnimation.Start("Glow", hpBase);
            }
        }
        else {
            if (iconAnimation.activeAnimation != nullptr) {
                iconAnimation.Stop();
                modelArray.GetData(hpIcon).SetColor(1.f, 1.f, 1.f);
            }
            if (turniconAnimation.activeAnimation != nullptr) {
                turniconAnimation.Stop();
                modelArray.GetData(turnIcon).SetColor(1.f, 1.f, 1.f);
            }
            if (turnAnimation.activeAnimation != nullptr) {
                const float darkred{ 173.f / 255.f };
                turnAnimation.Stop();
                modelArray.GetData(turn).SetColor(darkred, 0.f, 0.f);
            }
            if (hpbaseAnimation.activeAnimation != nullptr) {
                hpbaseAnimation.Stop();
                modelArray.GetData(hpBase).SetColor(0.f, 0.f, 0.f);
            }
        }
    }
}

void BattleSystem::MoveOutUIAnimation() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
    if (m_Entities.size() == 0) {
        return;
    }
    if (attackingAnimation == true) {
        return;
    }
    for (Entity& e : skillButtons) {
        animationArray.GetData(e).Start("Pop Out", e);
    }
    for (CharacterAnimator& ca : enemyAnimators) {
        const float darkred{ 178.f / 255.f };
        animationArray.GetData(ca.healthbarIcon).Stop();
        modelArray.GetData(ca.healthbarIcon).SetColor(1.f, 1.f, 1.f);

        animationArray.GetData(ca.healthbarBase).Stop();
        modelArray.GetData(ca.healthbarBase).SetColor(0.f, 0.f, 0.f);

        animationArray.GetData(ca.turnorderIcon).Stop();
        modelArray.GetData(ca.turnorderIcon).SetColor(1.f, 1.f, 1.f);

        animationArray.GetData(ca.turnorder).Stop();
        modelArray.GetData(ca.turnorder).SetColor(darkred, 0.f, 0.f);
    }
    attackingAnimation = true;
}

void BattleSystem::MoveInUIAnimation() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    if (m_Entities.size() == 0) {
        return;
    }

    UpdateSkillIcons();
    for (Entity& e : skillButtons) {
        animationArray.GetData(e).Start("Pop In", e);
    }
    attackingAnimation = false;
}

void BattleSystem::AnimateRemoveHealthBar(Entity entity) {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& healthbarArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<HealthBar>() };
    static auto& enemyHUDArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<EnemyHUD>() };
    if (m_Entities.size() == 0) {
        return;
    }
    for (Entity& e : enemyHealthBars) {
        Entity healthbarEntity{ healthbarArray.GetData(e).entity };
        if (healthbarEntity == entity) {
            animationArray.GetData(e).Start("Pop Out", e);
            return;
        }
    }
    for (Entity& e : allyHealthBars) {
        Entity healthbarEntity{ healthbarArray.GetData(e).entity };
        if (healthbarEntity == entity) {
            animationArray.GetData(e).Start("Pop Out", e);
            return;
        }
    }
}

void BattleSystem::UpdateSkillIcons() {
    static auto& textureArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Tex>() };
    static auto& buttonArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Button>() };
    if (m_Entities.size() == 0) {
        return;
    }

    if (activeCharacter == nullptr) {
        activeCharacter = turnManage.turnOrderList.front();
    }

    for (int i = 0; i < 3; i++) {
        if (activeCharacter->action.skills.size() < 3) {
            break;
        }
        std::string skillTexture = activeCharacter->action.skills[i].skillTexture;
        textureArray.GetData(skillButtons[i]).tex = assetmanager.texture.Get(skillTexture.c_str());
        buttonArray.GetData(skillButtons[i]).hoveredColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
        buttonArray.GetData(skillButtons[i]).defaultColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
    }
}
