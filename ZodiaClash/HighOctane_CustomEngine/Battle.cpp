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
#include "Layering.h"
#include "CheatCode.h"

//For animating skill buttons
const float skillButtonOffset{ 160.f };

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
        if (input.speedupCharacter != nullptr && c.entity == input.speedupCharacter->entity) {
            speedupCharacter = &c;
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
    speedup = input.speedup;
    aiMultiplier = input.aiMultiplier;
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
    emperorDead = false;
}

/**
 * @brief Starts the battle, initializes event connections, and determines the turn order.
 */
void BattleSystem::StartBattle() {
    events.ConnectBattleSystem(this);
    roundInProgress = false;
    roundManage.characterCount = 0;
    roundManage.roundCounter = 0;
    chi = 3;
    speedup = false;
    aiMultiplier = 0;
    skillTooltipCalled = false;

    if (DetermineTurnOrder() == false) {
        return;
    }

    for (auto& c : turnManage.characterList) {
        std::string name = ECS::ecs().GetComponent<Name>(c.entity).name;
    }

   // triggers any dialogue set with PRE_BATTLE trigger condition
    events.Call("Start Dialogue", "PRE_BATTLE");

    //Initialise turn order animator
    InitialiseBattleUI();

    battlestarted = true;
    dialogueCalled = false;
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
        if (tutorialLock) {
            return;
        }

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
            case(TYPE::DIALOGUE_ACTIVE):
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

        if (!battlestarted) {
            StartBattle();
            return;
        }
        
        if (!AnimateInitialiseTurnOrder()) {
            break;
        }

        InitialiseUIAnimation();

        // trigger tutorial only for battle.scn
        if (sceneName == "battle.scn") {
            events.Call("Start Tutorial", "");
        }

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
            battleState = WIN;
            CompleteBattle();
        }
        //then check if enemy has won
        else if (!playerAmount) //no players left
        {
            battleState = LOSE;
            CompleteBattle();
        }
        //continue battle
        else
        {
            if (roundManage.characterCount < static_cast<int>(turnManage.characterList.size()))
            {
                activeCharacter = turnManage.turnOrderList.front();
                while (activeCharacter->untargetable) {
                    turnManage.turnOrderList.splice(turnManage.turnOrderList.end(), turnManage.turnOrderList, turnManage.turnOrderList.begin()); //SEND TO BACK OF TURN ORDER LIST
                    activeCharacter = turnManage.turnOrderList.front();
                }

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
                roundManage.characterCount++;
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
        if (activeCharacter->action.entityState == WAITING && activeCharacter->debuffs.stunStack == 0) {
            gameAI.Search(this);
        }
        [[fallthrough]];

    case PLAYERTURN:
        //Check if turn/health conditions are met, trigger dialogue
        if (m_Entities.size() > 0 && !dialogueCalled && emperorDead) {
            for (CharacterStats* c : turnManage.turnOrderList) {
                // dialogue call trigger specific to round 1, Monkey is the boss
                if (ECS::ecs().GetComponent<Name>(c->entity).name == "Monkey") {
                    if (activeCharacter == c) {
                        events.Call("Start Dialogue", "EVENT");
                        emperorDead = false;
                        // to confirm indexing?
                    }
                }
            }
        }

        if (m_Entities.size() > 0 && !dialogueCalled && (roundManage.roundCounter == 3 || roundManage.roundCounter == 4)) {
            for (CharacterStats* c : turnManage.turnOrderList) {
                // dialogue call trigger specific to round 3 and 4, OX_Enemy is the boss
                if (ECS::ecs().GetComponent<Name>(c->entity).name == "Ox_Enemy") {
                    if (activeCharacter == c) {
                        events.Call("Start Dialogue", "TURN");
                        if (dialogueCalled == 3) {

                            //Do player healing and VFX first
                            for (CharacterStats* cc : turnManage.turnOrderList) {
                                if (cc->tag == CharacterType::PLAYER) {
                                    cc->stats.health = cc->stats.maxHealth;
                                    cc->damage = cc->stats.maxHealth;
                                }
                            }
                            damagePrefab = "Goat_Skill_VFX.prefab";
                            ProcessDamage();

                            //Do for the rest of the enemies afterwards
                            for (CharacterStats* cc : turnManage.turnOrderList) {
                                cc->stats.health = cc->stats.maxHealth;
                                cc->damage = cc->stats.maxHealth;
                            }
                            ProcessDamage();

                            events.Call("Restart Music", "ZodiaClash_Boss.ogg");
                            AddCharacter(EntityFactory::entityFactory().ClonePrefab("Player_Goat.prefab"));
                        }
                    }
                }
            }
        }
        if (dialogueCalled) {
            break;
        }

        activeCharacter->action.UpdateState();

        if (activeCharacter->action.entityState == EntityState::END || activeCharacter->action.entityState == EntityState::DYING) {

            //Update turn order animator
            if (locked) {
                return;
            }

            if (!AnimateUpdateTurnOrder()) {
                return;
            }

            if (speedup && speedupCharacter->entity == turnManage.turnOrderList.front()->entity && speedupAnimationPlayed) {
                turnManage.turnOrderList.pop_front();
                speedup = false;
                roundManage.characterCount--;
                AnimateReturnTurnOrder();
            }
            else {
                if (speedup) {
                    AnimateSpeedupTurnOrder();
                }
                turnManage.turnOrderList.splice(turnManage.turnOrderList.end(), turnManage.turnOrderList, turnManage.turnOrderList.begin()); //SEND TO BACK OF TURN ORDER LIST
            }

            //Process dead characters
            std::vector<CharacterStats*> deadchars{};
            deadchars.reserve(turnManage.turnOrderList.size());
            for (CharacterStats* c : turnManage.turnOrderList) {
                if (c->stats.health <= 0) {
                    deadchars.push_back(c);
                }
            }
            for (CharacterStats* c : deadchars) {
                //Handle boss goat death
                if (ECS::ecs().GetComponent<Name>(c->entity).name == "Goat_Enemy") {
                    for (auto& character : turnManage.characterList) {
                        if (character.tag == CharacterType::PLAYER) {
                            character.stats.health += 0.6f * c->stats.maxHealth;
                            character.damage = 0.6f * c->stats.maxHealth;
                            if (character.stats.health > character.stats.maxHealth) {
                                character.stats.health = character.stats.maxHealth;
                            }
                        }
                        if (character.tag == CharacterType::ENEMY) {
                            character.buffs.attackBuff = 0.5f;
                            character.buffs.attackStack = 9;
                        }
                    }
                    if (m_Entities.size() > 0) {
                        damagePrefab = "Goat_Skill_VFX.prefab";
                    }
                }

                //Handle ox death
                else if (ECS::ecs().GetComponent<Name>(c->entity).name == "Ox_Enemy") {
                    for (auto& character : turnManage.turnOrderList) {
                        if (character->tag == CharacterType::ENEMY && character->stats.health != 0.f) {
                            character->damage = character->stats.health;
                            character->stats.health = 0.f;
                            deadchars.push_back(character);
                        }
                    }
                }

                //Handle emperor death
                else if (ECS::ecs().GetComponent<Name>(c->entity).name == "Emperor") {
                    for (auto& character : turnManage.turnOrderList) {
                        if (character->tag == CharacterType::ENEMY && character->stats.health != 0.f) {
                            character->damage = character->stats.health;
                            character->stats.health = 0.f;
                            deadchars.push_back(character);
                        }
                    }
                    ProcessDamage();
                    camera.SetShake(0.f);
                    if (m_Entities.size() > 0) {
                        events.Call("Start Dialogue", "HEALTH");
                        emperorDead = true;
                    }
                }

                //Handle shield death
                else if (c->untargetable) {
                    for (auto& character : turnManage.turnOrderList) {
                        if (character->buffs.shieldEntity == c->entity) {
                            character->buffs.shieldStack = 0;
                            character->buffs.shieldEntity = 0;
                            character->debuffs.stunStack = 1;
                        }
                    }
                }

                ProcessDamage();
            }
            for (CharacterStats* c : deadchars) {
                AnimateRemoveTurnOrder(c->entity);
                AnimateRemoveHealthBar(c->entity);
                turnManage.turnOrderList.remove(c);
                turnManage.originalTurnOrderList.remove(c);
                turnManage.characterList.remove(*c);
            }
            deadchars.clear();

            battleState = NEXTTURN;
        }
        break;
    }
}

/**
* @brief Determines the turn order of characters based on their stats and adds them to the turn management lists.
**/
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
        if (ECS::ecs().GetComponent<Name>(m->entity).name == "Ox_Enemy") {
            m->buffs.reflectStack = 1;
        }
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
    //sort the turn order list by speed
    turnManage.turnOrderList.sort([](const CharacterStats* a, const CharacterStats* b) {return a->stats.speed > b->stats.speed; });

    //originalTurnOrderList
    turnManage.originalTurnOrderList = turnManage.turnOrderList;
    return true;
}

void BattleSystem::AddCharacter(Entity addition) {
    if (m_Entities.size() == 0) {
        return;
    }

    CharacterStats* m = &ECS::ecs().GetComponent<CharacterStats>(addition);
    m->entity = addition;
    m->parent = this;
    m->action.battleManager = this;
    turnManage.characterList.push_back(*m);
    CharacterStats* newAddition{ &turnManage.characterList.back() };

    turnManage.turnOrderList.push_back(newAddition);
    turnManage.originalTurnOrderList = turnManage.turnOrderList;
    
    static auto& turnorderArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<TurnIndicator>() };
    static auto& texArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Tex>() };
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
    static auto& sizeArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Size>() };
    static auto& transformArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>() };
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& healthbarArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<HealthBar>() };

    CharacterAnimator animator{};
    Entity healthbar{};
    float hp_offset{ 0.f };
    Entity turnUI{};

    animator.character = addition;

    //Create UI for new character
    if (m->tag == CharacterType::PLAYER) {
        healthbar = EntityFactory::entityFactory().ClonePrefab("ally_healthbar.prefab");
        hp_offset = healthBarOffset * static_cast<float>(GetPlayers().size() - 1);
        turnUI = EntityFactory::entityFactory().ClonePrefab("turn_ally.prefab");
    }
    else {
        healthbar = EntityFactory::entityFactory().ClonePrefab("enemy_healthbar.prefab");
        hp_offset = healthBarOffset * static_cast<float>(GetEnemies().size() - 1);
        turnUI = EntityFactory::entityFactory().ClonePrefab("turn_enemy.prefab");
    }
    
    //Add healthbar
    transformArray.GetData(healthbar).position.y += hp_offset;
    animationArray.GetData(healthbar).Start("Pop In", healthbar);
    if (dialogueCalled) {
        animationArray.GetData(healthbar).Queue("Pop Out", healthbar);
    }
    animator.healthbar = healthbar;
    animator.turnorder = turnUI;
    animator.healthbarIcon = parentArray.GetData(healthbar).GetChildByName("hpBarIcon");
    texArray.GetData(animator.healthbarIcon).tex = assetmanager.texture.Get(m->icon.c_str());
    modelArray.GetData(animator.healthbarIcon).SetMirror(modelArray.GetData(addition).GetMirror());
    animator.healthbarBase = parentArray.GetData(healthbar).GetChildByName("hpBarBase");
    healthbarArray.GetData(healthbar).charaStatsRef = m;
    healthbarArray.GetData(healthbar).entity = addition;
    
    allBattleUI.push_back(healthbar);

    //Add Turn UI
    animator.turnorderIcon = parentArray.GetData(turnUI).GetChildByName("turnOrderIcon");
    turnorderArray.GetData(turnUI).character = addition;
    if (!m->untargetable) {
        animationArray.GetData(turnOrderAnimator).Queue("Add", turnOrderAnimator);
        animationArray.GetData(turnOrderAnimator).InstantFinish(turnOrderAnimator);
        animationArray.GetData(turnUI).Start("Shift In", turnUI);
        if (dialogueCalled) {
            animationArray.GetData(turnUI).Queue("Move Out", turnUI);
        }
        if (emperorDead) {
            animationArray.GetData(turnUI).Queue("Move Out", turnUI);
        }
        animationArray.GetData(turnUI).InstantFinish(turnUI);
        texArray.GetData(animator.turnorderIcon).tex = assetmanager.texture.Get(m->icon.c_str());
        turnOrderQueueAnimator.push_back(turnUI);
    }
    
    if (m->tag == CharacterType::PLAYER) {
        modelArray.GetData(animator.turnorderIcon).SetMirror(!modelArray.GetData(addition).GetMirror());
        allyHealthBars.push_back(healthbar);
        allyAnimators.push_back(animator);
    }
    else {
        modelArray.GetData(animator.turnorderIcon).SetMirror(modelArray.GetData(addition).GetMirror());
        enemyHealthBars.push_back(healthbar);
        enemyAnimators.push_back(animator);
    }
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
    std::deque<CharacterStats*> tmpDeque;
    std::vector<CharacterStats*> output;
    for (auto& c : turnManage.characterList) {
        if (c.tag == CharacterType::ENEMY) {
            if (c.boss) {
                tmpDeque.push_front(&c);
            }
            else {
                tmpDeque.push_back(&c);
            }
        }
    }
    for (auto& c : tmpDeque) {
        output.push_back(c);
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
    auto iterator = turnManage.turnOrderList.begin();
    iterator++;
    turnManage.turnOrderList.insert(iterator, target);
    speedup = true;
    speedupCharacter = target;
    speedupAnimationPlayed = false;
}

/**
 * @brief Reverts the turn order change for a character to its original position.
 *
 * @param target A pointer to the CharacterStats of the target character.
 */
void BattleSystem::RevertTurnOrder(CharacterStats* target)
{
    UNREFERENCED_PARAMETER(target);
}

/*!***********************************************************************
 \brief
  Finalizes the battle process, handling both win and lose scenarios. It pauses the background music, animates UI elements to indicate the battle's end, and, based on the battle outcome, spawns appropriate win or lose text. It also plays a sound to signify the battle's completion.
 \param
  This function does not take any parameters.
 \return
  This function does not return a value. It affects the game state by altering UI elements, playing audio, and potentially spawning new entities based on the battle outcome.
 *************************************************************************/
void BattleSystem::CompleteBattle() {
    static ComponentManager& componentManager = ECS::ecs().GetComponentManager();
    static ComponentArray<AnimationSet>* animationArray = &componentManager.GetComponentArrayRef<AnimationSet>();
    if (m_Entities.size() > 0) {
        assetmanager.audio.PauseGroup("BGM");
        for (int i = 0; i < 2; i++) {
            for (Entity& e : allBattleUI) {
                animationArray->GetData(e).Queue("Pop Out", e);
            }
            for (Entity& e : turnOrderQueueAnimator) {
                animationArray->GetData(e).Queue("Pop Out", e);
            }
        }
        if (battleState == WIN) {
            EntityFactory::entityFactory().ClonePrefab("wintext.prefab");
            events.Call("Start Dialogue", "WIN");
        }
        else if (battleState == LOSE) {
            EntityFactory::entityFactory().ClonePrefab("losetext.prefab");
        }
        events.Call("Play Sound", "Battle End_Edited.wav");
    }
}

void BattleSystem::ProcessDamage() {
    if (m_Entities.size() > 0) {
        ComponentManager& componentManager = ECS::ecs().GetComponentManager();
        ComponentArray<CharacterStats>* statsArray = &componentManager.GetComponentArrayRef<CharacterStats>();
        ComponentArray<Model>* modelArray = &componentManager.GetComponentArrayRef<Model>();
        ComponentArray<AnimationSet>* animationArray = &componentManager.GetComponentArrayRef<AnimationSet>();
        ComponentArray<Transform>* transformArray = &componentManager.GetComponentArrayRef<Transform>();
        ComponentArray<TextLabel>* textArray = &componentManager.GetComponentArrayRef<TextLabel>();
        ComponentArray<Name>* nameArray = &componentManager.GetComponentArrayRef<Name>();
        ComponentArray<Parent>* parentArray = &componentManager.GetComponentArrayRef<Parent>();

        float totalDamage{ 0.f };
        for (Entity entity : m_Entities) {
            CharacterStats* cs = &statsArray->GetData(entity);
            if (cs->action.entityState == DEAD) {
                continue;
            }
            bool found = false;
            for (CharacterStats& c : turnManage.characterList) {
                if (c.entity == entity) {
                    if (speedup && entity == speedupCharacter->entity) {
                        Entity damageEffect{ EntityFactory::entityFactory().ClonePrefab(damagePrefab) };
                        if (damageEffect) {
                            transformArray->GetData(damageEffect).position = transformArray->GetData(entity).position;
                        }
                        found = true;
                    }
                    if (c.stats.health != cs->stats.health || c.debuffs != cs->debuffs) {
                        Entity damageEffect{ EntityFactory::entityFactory().ClonePrefab(damagePrefab) };
                        if (damageEffect) {
                            transformArray->GetData(damageEffect).position = transformArray->GetData(entity).position;
                        }

                        float damage = cs->stats.health - c.stats.health;
                        totalDamage += damage;
                        Entity damagelabel{ EntityFactory::entityFactory().ClonePrefab("damagelabel.prefab") };
                        transformArray->GetData(damagelabel).position = transformArray->GetData(entity).position;
                        textArray->GetData(damagelabel).textString = std::to_string(abs((int)(c.damage)));

                        if (c.stats.health > 0) {
                            if (damage > 0.f) {
                                animationArray->GetData(entity).Queue("Damaged", entity);

                                //For shield, play the respective shield animations for the children also
                                if (c.untargetable) {
                                    for (CharacterStats* enemy : GetEnemies()) {
                                        if (!parentArray->HasComponent(enemy->entity)) {
                                            continue;
                                        }
                                        Entity enemyshield{ parentArray->GetData(enemy->entity).GetChildByName("Monkey Shield") };
                                        if (animationArray->HasComponent(enemyshield)) {
                                            animationArray->GetData(enemyshield).Queue("Damaged", enemyshield);
                                        }
                                    }
                                }

                                if (c.crit == true) {
                                    textArray->GetData(damagelabel).SetTextColor(glm::vec4{ 1.f,0.f,0.f,1.f });
                                    textArray->GetData(damagelabel).textString = "CRIT\n" + textArray->GetData(damagelabel).textString;
                                    c.crit = false;
                                }
                            }
                            else if (damage != 0.f) {
                                textArray->GetData(damagelabel).SetTextColor(glm::vec4{ 0.f,1.f,0.f,1.f });
                            }
                            
                            //Delete damage label if damage is 0
                            if (c.damage == 0.f) {
                                ECS::ecs().DestroyEntity(damagelabel);
                            }

                            //Create boss label
                            if (c.boss == true && c.stats.health < 0.5 * c.stats.maxHealth) {
                                Entity aura{ parentArray->GetData(c.entity).GetChildByName("VFX_Corrupted") };
                                if (modelArray->GetData(aura).GetAlpha() <= 0.1f && animationArray->HasComponent(aura)) {
                                    animationArray->GetData(aura).Start("Appear",aura);
                                }
                            }
                            else if (c.boss == true && c.stats.health > 0.5 * c.stats.maxHealth) {
                                Entity aura{ parentArray->GetData(c.entity).GetChildByName("VFX_Corrupted") };
                                if (modelArray->GetData(aura).GetAlpha() >= 0.9f && animationArray->HasComponent(aura)) {
                                    animationArray->GetData(aura).Start("Disappear", aura);
                                }
                            }
                        }
                        else {
                            animationArray->GetData(entity).Start("Death", entity);
                            c.buffs = CharacterStats::buff{};
                            c.debuffs = CharacterStats::debuff{};

                            // Handle boss ox death
                            if (nameArray->GetData(c.entity).name == "Ox_Enemy") {
                                events.Call("Start Dialogue", "HEALTH");
                            }

                            // Handle boss ox death
                            else if (nameArray->GetData(c.entity).name == "Monkey") {
                                modelArray->GetData(c.entity).SetMirror(!modelArray->GetData(c.entity).GetMirror());
                            }

                            //Handle shield death
                            else if (c.untargetable) {
                                for (CharacterStats* enemy : GetEnemies()) {
                                    if (!parentArray->HasComponent(enemy->entity)) {
                                        continue;
                                    }
                                    animationArray->GetData(enemy->entity).Start("Damaged",enemy->entity);
                                    Entity enemyshield{ parentArray->GetData(enemy->entity).GetChildByName("Monkey Shield") };
                                    if (animationArray->HasComponent(enemyshield)) {
                                        animationArray->GetData(enemyshield).Queue("Death", enemyshield);
                                    }
                                }
                                for (CharacterStats* ally : GetPlayers()) {
                                    ally->debuffs.igniteStack = 0;
                                }
                                chi = 5;
                                animationArray->GetData(chiLabel).Queue("Refresh", chiLabel);
                            }
                        }
                    }
                    *cs = c;
                    found = true;
                    break;
                }
            }
            //Death animation and logic
            if (found == false) {
                cs->action.entityState = DEAD;
                cs->debuffs = CharacterStats::debuff{};
                cs->buffs = CharacterStats::buff{};
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
        if (totalDamage > 0.f && !godModeOn && !endGameOn) {
            camera.SetShake(totalDamage / MAGNITUDE_PER_HEALTH);
        }
        damagePrefab.clear();
    }
    
}

void BattleSystem::InitialiseBattleUI() {
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static auto& turnorderArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<TurnIndicator>() };
    static auto& texArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Tex>() };
    static auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
    static auto& buttonArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Button>() };
    static auto& cloneArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Clone>() };
    if (m_Entities.size() > 0) {
        skillButtons.clear();
        enemyAnimators.clear();
        allyAnimators.clear();
        allyHealthBars.clear();
        enemyHealthBars.clear();
        allBattleUI.clear();
        battleInfoButton = 0;
        battleUIMovedOut = false;
        speedUpAndDeath = false;

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
            ECS::ecs().GetComponent<HealthBar>(healthbar).entity = c->entity;
            ECS::ecs().GetComponent<HealthBar>(healthbar).charaStatsRef = &ECS::ecs().GetComponent<CharacterStats>(c->entity);
            allyHealthBars.push_back(healthbar);
            allBattleUI.push_back(healthbar);
            ally_hp_offset += healthBarOffset;

            CharacterAnimator animator{};
            animator.character = c->entity;
            animator.healthbar = healthbar;
            animator.healthbarIcon = parentArray.GetData(healthbar).GetChildByName("hpBarIcon");
            texArray.GetData(animator.healthbarIcon).tex = assetmanager.texture.Get(c->icon.c_str());
            modelArray.GetData(animator.healthbarIcon).SetMirror(modelArray.GetData(c->entity).GetMirror());
            animator.healthbarBase = parentArray.GetData(healthbar).GetChildByName("hpBarBase");
            for (Entity& turnUI : turnOrderQueueInitializer) {
                if (turnorderArray.GetData(turnUI).character == animator.character) {
                    animator.turnorder = turnUI;
                    animator.turnorderIcon = parentArray.GetData(turnUI).GetChildByName("turnOrderIcon");
                    texArray.GetData(animator.turnorderIcon).tex = assetmanager.texture.Get(c->icon.c_str());
                    modelArray.GetData(animator.turnorderIcon).SetMirror(!modelArray.GetData(c->entity).GetMirror());
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
            ECS::ecs().GetComponent<HealthBar>(healthbar).entity = c->entity;
            ECS::ecs().GetComponent<HealthBar>(healthbar).charaStatsRef = &ECS::ecs().GetComponent<CharacterStats>(c->entity);
            enemyHealthBars.push_back(healthbar);
            allBattleUI.push_back(healthbar);
            enemy_hp_offset += healthBarOffset;

            CharacterAnimator animator{};
            animator.character = c->entity;
            animator.healthbar = healthbar;
            animator.healthbarIcon = parentArray.GetData(healthbar).GetChildByName("hpBarIcon");
            texArray.GetData(animator.healthbarIcon).tex = assetmanager.texture.Get(c->icon.c_str());
            modelArray.GetData(animator.healthbarIcon).SetMirror(modelArray.GetData(c->entity).GetMirror());
            animator.healthbarBase = parentArray.GetData(healthbar).GetChildByName("hpBarBase");
            for (Entity& turnUI : turnOrderQueueInitializer) {
                if (turnorderArray.GetData(turnUI).character == animator.character) {
                    animator.turnorder = turnUI;
                    animator.turnorderIcon = parentArray.GetData(turnUI).GetChildByName("turnOrderIcon");
                    texArray.GetData(animator.turnorderIcon).tex = assetmanager.texture.Get(c->icon.c_str());
                    modelArray.GetData(animator.turnorderIcon).SetMirror(modelArray.GetData(c->entity).GetMirror());
                    break;
                }
            }
            enemyAnimators.push_back(animator);
        }

        //Initialise battle info button
        for (auto& pair : buttonArray.GetPairArray()) {
            if (cloneArray.HasComponent(pair.first)) {
                if (pair.second->eventName == "Toggle Battle Info") {
                    battleInfoButton = pair.first;
                    allBattleUI.push_back(pair.first);
                    break;
                }
            }
        }
    }
}

void BattleSystem::InitialiseTurnOrderAnimator() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    if (m_Entities.size() > 0 && !ECS::ecs().EntityExists(turnOrderAnimator)) {
        turnOrderAnimator = EntityFactory::entityFactory().ClonePrefab("turnorderattach.prefab");
        for (auto& c : turnManage.turnOrderList) {
            if (c->tag == CharacterType::PLAYER) {
                Entity turnUI{ EntityFactory::entityFactory().ClonePrefab("turn_ally.prefab") };
                turnOrderQueueInitializer.push_back(turnUI);
                ECS::ecs().GetComponent<TurnIndicator>(turnUI).character = c->entity;
            }
            else if (c->tag == CharacterType::ENEMY) {
                Entity turnUI{ EntityFactory::entityFactory().ClonePrefab("turn_enemy.prefab") };
                turnOrderQueueInitializer.push_back(turnUI);
                ECS::ecs().GetComponent<TurnIndicator>(turnUI).character = c->entity;
            }
            animationArray.GetData(turnOrderAnimator).Queue("Add", turnOrderAnimator);
            animationArray.GetData(turnOrderAnimator).InstantFinish(turnOrderAnimator);
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
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    if (m_Entities.size() > 0 && !speedup) {
        static bool turnanimated{ false };
        if (turnanimated == false) {
            Entity queueFront{ turnOrderQueueAnimator.front() };
            animationArray.GetData(turnOrderQueueAnimator.front()).Queue("Pop Out", turnOrderQueueAnimator.front());
            Entity retractingIcon{ parentArray.GetData(turnOrderQueueAnimator.front()).GetChildByName("turnOrderIcon") };
            animationArray.GetData(retractingIcon).Queue("Unexpand", retractingIcon);
            turnOrderQueueAnimator.pop_front();

            animationArray.GetData(turnOrderQueueAnimator.front()).Queue("Expand", turnOrderQueueAnimator.front());
            Entity expandingIcon = parentArray.GetData(turnOrderQueueAnimator.front()).GetChildByName("turnOrderIcon");
            animationArray.GetData(expandingIcon).Queue("Expand", expandingIcon);
            for (Entity& e : turnOrderQueueAnimator) {
                if (e != turnOrderQueueAnimator.front()) {
                    animationArray.GetData(e).Queue("Next Turn", e);
                }
            }
            turnOrderQueueAnimator.push_back(queueFront);
            turnanimated = true;
            return false;
        }
        else {
            animationArray.GetData(turnOrderQueueAnimator.back()).Queue("Shift In", turnOrderQueueAnimator.back());
            turnanimated = false;
        }
    }
    return true;
}

/*!***********************************************************************
 \brief
  Animates the speed-up effect in the turn order queue. This function identifies the position of a specific entity (the one to be sped up) within the turn order list. It animates the front of the turn order queue to simulate the effect of speeding up the specified entity's turn, adjusts the turn order visually, and applies an expanding animation to the icon representing the sped-up entity in the UI.
 \param
  This function does not take any parameters.
 \return
  This function does not return a value. Instead, it modifies the turn order queue's visual representation by animating UI elements to reflect a change in the turn order, specifically highlighting the effect of a "speed-up" action on a particular entity.
 *************************************************************************/
void BattleSystem::AnimateSpeedupTurnOrder() {
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    auto iterator = turnManage.turnOrderList.begin();
    int count{0};
    bool firstfound{ false };
    speedupAnimationPlayed = true;
    if (m_Entities.size() == 0) {
        return;
    }
    while (iterator != turnManage.turnOrderList.end() && ((*iterator)->entity != speedupCharacter->entity || !firstfound)) {
        if ((*iterator)->untargetable) {
            iterator++;
            continue;
        }
        if ((*iterator)->entity == speedupCharacter->entity) {
            firstfound = true;
        }
        iterator++;
        count++;
    }
    animationArray.GetData(turnOrderQueueAnimator.front()).Queue("Pop Out", turnOrderQueueAnimator.front());
    animationArray.GetData(turnOrderQueueAnimator.front()).Queue("Shift In", turnOrderQueueAnimator.front());
    Entity retractingIcon{ parentArray.GetData(turnOrderQueueAnimator.front()).GetChildByName("turnOrderIcon") };
    animationArray.GetData(retractingIcon).Start("Unexpand", retractingIcon);
    Entity queueFront{ turnOrderQueueAnimator.front() };
    turnOrderQueueAnimator.pop_front();
    turnOrderQueueAnimator.push_back(queueFront);
    bool found{false};
    for (Entity e : turnOrderQueueAnimator) {
        if (ECS::ecs().GetComponent<TurnIndicator>(e).character == speedupCharacter->entity) {
            AnimationSet& animation{ animationArray.GetData(e) };
            Entity turnIcon{ parentArray.GetData(e).GetChildByName("turnOrderIcon") };
            for (int i = 0; i < count - 2; i++) {
                animation.Queue("Next Turn", e);
            }
            animation.Queue("Expand", e);
            animationArray.GetData(turnIcon).Queue("Expand",turnIcon);
            found = true;
        }
        else if (found && e != turnOrderQueueAnimator.front() && e != turnOrderQueueAnimator.back()) {
            AnimationSet& animation{ animationArray.GetData(e) };
            animation.Queue("Next Turn", e);
        }
    }
}

void BattleSystem::AnimateReturnTurnOrder() {
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    auto iterator = turnManage.turnOrderList.rbegin();
    int count{0};
    if (m_Entities.size() == 0) {
        return;
    }

    bool iteratorfound{ false };
    while (iterator != turnManage.turnOrderList.rend()) {
        if ((*iterator)->untargetable) {
            iterator++;
            continue;
        }
        if (!iteratorfound && (*iterator)->entity != speedupCharacter->entity) {
            count++;
        }
        else {
            iteratorfound = true;
        }
        if ((*iterator)->stats.health == 0) {
            speedUpAndDeath = true;
            count--;
        }
        iterator++;
    }
    Entity entity {};
    for (Entity e : turnOrderQueueAnimator) {
        if (ECS::ecs().GetComponent<TurnIndicator>(e).character == speedupCharacter->entity) {
            entity = e;
            break;
        }
    }
    if (turnOrderQueueAnimator.front() == entity) {
        count--;
    }
    AnimationSet& animation{ animationArray.GetData(entity) };
    Entity turnIcon{ parentArray.GetData(entity).GetChildByName("turnOrderIcon") };
    animation.Queue("Pop Out", entity);
    animationArray.GetData(turnIcon).Queue("Unexpand", turnIcon);
    animation.Queue("Shift In", entity);
    for (int i = 0; i < count; i++) {
        animation.Queue("Next Turn", entity);
    }
    
    animationArray.GetData(turnOrderQueueAnimator.front()).Queue("Expand", turnOrderQueueAnimator.front());
    Entity expandingIcon = parentArray.GetData(turnOrderQueueAnimator.front()).GetChildByName("turnOrderIcon");
    animationArray.GetData(expandingIcon).Queue("Expand", expandingIcon);
    for (Entity& e : turnOrderQueueAnimator) {
        if (ECS::ecs().GetComponent<TurnIndicator>(e).character == speedupCharacter->entity) {
            break;
        }
        else if (e != turnOrderQueueAnimator.front()) {
            animationArray.GetData(e).Queue("Next Turn", e);
        }
    }
}

void BattleSystem::InitialiseUIAnimation() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& transformArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>() };
    static auto& sizeArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Size>() };
    static auto& childArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Child>() };
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    if (m_Entities.size() > 0) {
        UpdateSkillIcons();
        for (Entity& e : allBattleUI) {
            animationArray.GetData(e).Start("Pop In", e);
        }
        if (activeCharacter->tag == CharacterType::ENEMY) {
            for (Entity& e : skillButtons) {
                animationArray.GetData(e).Stop();
            }
            attackingAnimation = true;
        }
        else {
            if (parentArray.HasComponent(activeCharacter->entity)) {
                for (Entity turnindicator : parentArray.GetData(activeCharacter->entity).children) {
                    turnIndicator = turnindicator;
                    animationArray.GetData(turnindicator).Queue("Show", turnindicator);
                }
            }
            transformArray.GetData(turnIndicator).position = transformArray.GetData(activeCharacter->entity).position;
            float sizeDifference{ sizeArray.GetData(turnIndicator).height * transformArray.GetData(turnIndicator).scale -
                sizeArray.GetData(activeCharacter->entity).height * transformArray.GetData(activeCharacter->entity).scale };
            transformArray.GetData(turnIndicator).position.y += sizeDifference / 2;
        }
    }
}

void BattleSystem::AnimateRemoveTurnOrder(Entity entity) {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    if (m_Entities.size() > 0) {
        std::deque<Entity> newTurnOrderQueueAnimator{};
        bool moveup{ false };
        bool deathAtStart{ false };
        for (size_t i = 0; i < turnOrderQueueAnimator.size(); i++) {
            Entity e{ turnOrderQueueAnimator[i] };
            if (ECS::ecs().GetComponent<TurnIndicator>(e).character == entity) {
                ECS::ecs().GetComponent<AnimationSet>(e).Queue("Pop Out", e);
                Entity icon{ parentArray.GetData(e).GetChildByName("turnOrderIcon") };
                ECS::ecs().GetComponent<AnimationSet>(icon).Queue("Unexpand", icon);
                moveup = true;
                if (e == turnOrderQueueAnimator.front()) {
                    deathAtStart = true;
                }
                continue;
            }
            newTurnOrderQueueAnimator.push_back(e);
            if (moveup) {
                if (deathAtStart) {
                    ECS::ecs().GetComponent<AnimationSet>(e).Queue("Expand", e);
                    Entity icon{ parentArray.GetData(e).GetChildByName("turnOrderIcon") };
                    ECS::ecs().GetComponent<AnimationSet>(icon).Queue("Expand", icon);
                    deathAtStart = false;
                }
                else if (speedUpAndDeath) {
                    ECS::ecs().GetComponent<AnimationSet>(e).Queue("Next Turn", e);
                }
                else if (!speedUpAndDeath && e != turnOrderQueueAnimator.back()) {
                    ECS::ecs().GetComponent<AnimationSet>(e).Queue("Next Turn", e);
                }
            }
        }

        //Cannot find turn indicator
        if (moveup == false) {
            return;
        }

        animationArray.GetData(turnOrderAnimator).Queue("Subtract", turnOrderAnimator);
        animationArray.GetData(turnOrderAnimator).InstantFinish(turnOrderAnimator);
        turnOrderQueueAnimator = newTurnOrderQueueAnimator;
    }
    speedUpAndDeath = false;
}

void BattleSystem::CreateTargets() {
    if (activeCharacter->action.selectedSkill.attacktype == AttackType::ALLY || activeCharacter->action.selectedSkill.attacktype == AttackType::ALLYSELF) {
        auto allyList{ GetPlayers() };
        if (!targetCircleList.empty()) {
            DestroyTargets();
        }
        int count = 0;
        for (CharacterStats* ally : allyList) {
            if (ally->entity == activeCharacter->entity && activeCharacter->action.selectedSkill.attacktype == AttackType::ALLY) {
                count++;
                continue;
            }
            Entity targetcircle{ EntityFactory::entityFactory().ClonePrefab("targetcircle.prefab") };
            ECS::ecs().GetComponent<Transform>(targetcircle).position = ECS::ecs().GetComponent<Transform>(ally->entity).position;
            ECS::ecs().GetComponent<Button>(targetcircle).eventInput = std::to_string(count);
            targetCircleList.push_back(targetcircle);
            count++;
        }
    }
    else {
        auto enemyList{ GetEnemies() };
        if (!targetCircleList.empty()) {
            DestroyTargets();
        }
        int count = 0;
        for (CharacterStats* enemy : enemyList) {
            if (enemy->untargetable) {
                count++;
                continue;
            }

            Entity targetcircle{ EntityFactory::entityFactory().ClonePrefab("targetcircle.prefab") };
            ECS::ecs().GetComponent<Transform>(targetcircle).position = ECS::ecs().GetComponent<Transform>(enemy->entity).position;
            if (activeCharacter->debuffs.tauntStack > 0 && activeCharacter->action.selectedSkill.attacktype != AttackType::AOE && activeCharacter->debuffs.tauntTarget != enemy->entity) {
                ECS::ecs().GetComponent<Transform>(targetcircle).position = Vec2{ -10000.f,-10000.f };
            }
            if (activeCharacter->action.selectedSkill.attacktype == AttackType::AOE) {
                ECS::ecs().GetComponent<Model>(targetcircle).SetColor(1.f, 0.f, 0.f);
            }
            ECS::ecs().GetComponent<Button>(targetcircle).eventInput = std::to_string(count);
            targetCircleList.push_back(targetcircle);
            count++;
        }
    }
}

void BattleSystem::DestroyTargets() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    for (Entity target : targetCircleList) {
        animationArray.GetData(target).Queue("Destroy",target);
    }
    targetCircleList.clear();
}

void BattleSystem::UpdateTargets() {
    static auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
    static auto& buttonArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Button>() };
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    static vmath::Vector2 mousePos{ RESET_VEC2 };
    const float darkred{ 117.f / 255.f };

    if (activeCharacter == nullptr) {
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
    if (targetCircleList.size() > 0) {
        for (int i = 0; i < static_cast<int>(targetCircleList.size()); i++) {
            Entity currentTarget{ targetCircleList[i] };
            Model& targetModel{ modelArray.GetData(currentTarget) };
            Button& targetButton{ buttonArray.GetData(currentTarget) };
            targetButton.defaultColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,targetButton.defaultColor.buttonColor.a };
            targetButton.hoveredColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,targetButton.defaultColor.buttonColor.a };
            if (IsWithinObject(targetModel, mousePos)) {
                selected = i;
            }
        }
        for (int i = 0; i < static_cast<int>(targetCircleList.size()); i++) {
            Entity currentTarget{ targetCircleList[i] };
            Button& targetButton{ buttonArray.GetData(currentTarget) };
            Entity hpIcon{};
            Entity turnIcon{};
            Entity turn{};
            Entity hpBase{};

            if (activeCharacter->action.selectedSkill.attacktype == AttackType::ALLY || activeCharacter->action.selectedSkill.attacktype == AttackType::ALLYSELF) {
                hpIcon = allyAnimators[i].healthbarIcon;
                turnIcon = allyAnimators[i].turnorderIcon;
                turn = allyAnimators[i].turnorder;
                hpBase = allyAnimators[i].healthbarBase;
            }
            else {
                hpIcon = enemyAnimators[i].healthbarIcon;
                turnIcon = enemyAnimators[i].turnorderIcon;
                turn = enemyAnimators[i].turnorder;
                hpBase = enemyAnimators[i].healthbarBase;
            }

            AnimationSet& iconAnimation{ animationArray.GetData(hpIcon) };
            AnimationSet& turniconAnimation{ animationArray.GetData(turnIcon) };
            AnimationSet& turnAnimation{ animationArray.GetData(turn) };
            AnimationSet& hpbaseAnimation{ animationArray.GetData(hpBase) };

            if ((aoe && selected >= 0) || (selected == i)) {
                targetButton.defaultColor.buttonColor = glm::vec4{ 1.f,0.f,0.f,targetButton.defaultColor.buttonColor.a };
                targetButton.hoveredColor.buttonColor = glm::vec4{ 1.f,0.f,0.f,targetButton.defaultColor.buttonColor.a };

                if (activeCharacter->action.selectedSkill.attacktype == AttackType::ALLY || activeCharacter->action.selectedSkill.attacktype == AttackType::ALLYSELF) {
                    continue;
                }

                if (iconAnimation.activeAnimation == nullptr) {
                    iconAnimation.Start("Glow_Dark", hpIcon);
                }

                if (hpbaseAnimation.activeAnimation == nullptr) {
                    hpbaseAnimation.Start("Glow_Dark", hpBase);
                }

                if (turniconAnimation.activeAnimation == nullptr) {
                    turniconAnimation.Start("Glow", turnIcon);
                }

                if (turnAnimation.activeAnimation == nullptr) {
                    turnAnimation.Start("Glow", turn);
                }
            }
            else {
                if (activeCharacter->action.selectedSkill.attacktype == AttackType::ALLY || activeCharacter->action.selectedSkill.attacktype == AttackType::ALLYSELF) {
                    continue;
                }

                if (iconAnimation.activeAnimation != nullptr) {
                    iconAnimation.Stop();
                    modelArray.GetData(hpIcon).SetColor(1.f, 1.f, 1.f);
                }
                if (hpbaseAnimation.activeAnimation != nullptr) {
                    hpbaseAnimation.Stop();
                    modelArray.GetData(hpBase).SetColor(darkred, 0.f, 0.f);
                }
                if (turniconAnimation.activeAnimation != nullptr) {
                    turniconAnimation.Stop();
                    modelArray.GetData(turnIcon).SetColor(1.f, 1.f, 1.f);
                }
                if (turnAnimation.activeAnimation != nullptr) {
                    turnAnimation.Stop();
                    modelArray.GetData(turn).SetColor(darkred, 0.f, 0.f);
                }
            }
        }
    }

    //TOOLTIPS
    static Entity tooltipPrefab{};
    bool isHovered{ false };
    for (int i = 0; i < static_cast<int>(skillButtons.size()); i++) {
        Model& skillModel{ modelArray.GetData(skillButtons[i])};
        if (IsWithinObject(skillModel, mousePos)) {
            isHovered = true;
            if (!skillTooltipCalled) {
                if (ECS::ecs().EntityExists(tooltipPrefab)) {
                    EntityFactory::entityFactory().DeleteCloneModel(tooltipPrefab);
                }
                tooltipPrefab = EntityFactory::entityFactory().ClonePrefab(tooltips[i]);
                skillTooltipCalled = true;
            }
        }
    }
    if (!isHovered && ECS::ecs().EntityExists(tooltipPrefab)) {
        EntityFactory::entityFactory().DeleteCloneModel(tooltipPrefab);
        skillTooltipCalled = false;
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
    if (turnIndicator) {
        animationArray.GetData(turnIndicator).Queue("Hide", turnIndicator);
        turnIndicator = 0;
    }
    for (Entity& e : skillButtons) {
        animationArray.GetData(e).Start("Pop Out", e);
    }
    for (CharacterAnimator& ca : enemyAnimators) {
        const float darkred{ 117.f / 255.f };
        animationArray.GetData(ca.healthbarIcon).Stop();
        modelArray.GetData(ca.healthbarIcon).SetColor(1.f, 1.f, 1.f);

        animationArray.GetData(ca.healthbarBase).Stop();
        modelArray.GetData(ca.healthbarBase).SetColor(darkred, 0.f, 0.f);

        animationArray.GetData(ca.turnorderIcon).Stop();
        modelArray.GetData(ca.turnorderIcon).SetColor(1.f, 1.f, 1.f);

        animationArray.GetData(ca.turnorder).Stop();
        modelArray.GetData(ca.turnorder).SetColor(darkred, 0.f, 0.f);
    }
    attackingAnimation = true;
}

void BattleSystem::MoveInUIAnimation() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& transformArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>() };
    static auto& sizeArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Size>() };
    static auto& childArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Child>() };
    static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
    if (m_Entities.size() == 0) {
        return;
    }

    UpdateSkillIcons();

    if (parentArray.HasComponent(activeCharacter->entity)) {
        Entity turnindicator = parentArray.GetData(activeCharacter->entity).GetChildByName("Turn Indicator");
        turnIndicator = turnindicator;
        animationArray.GetData(turnindicator).Queue("Show", turnindicator);
    }

    for (Entity& e : skillButtons) {
        animationArray.GetData(e).Start("Pop In", e);
    }
    attackingAnimation = false;
}

//Calls "Pop Out" animation for all UI, to remove UI during attacking animations
void BattleSystem::MoveOutAllUIAnimation() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    for (Entity& e : allBattleUI) {
        if (!animationArray.HasComponent(e)) {
            continue;
        }
        animationArray.GetData(e).Start("Pop Out", e);
    }
    for (Entity& e : turnOrderQueueAnimator) {
        if (!animationArray.HasComponent(e)) {
            continue;
        }
        animationArray.GetData(e).Queue("Move Out", e);
    }
    battleUIMovedOut = true;
}
//Calls "Pop In" animation for all UI, to add back UI after attacking
void BattleSystem::MoveInAllUIAnimation() {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    if (!battleUIMovedOut) {
        return;
    }
    for (Entity& e : allBattleUI) {
        if (!animationArray.HasComponent(e)) {
            continue;
        }
        animationArray.GetData(e).Start("Pop In", e);
    }
    for (Entity& e : turnOrderQueueAnimator) {
        if (!animationArray.HasComponent(e)) {
            continue;
        }
        animationArray.GetData(e).Queue("Move In", e);
    }
    battleUIMovedOut = false;
}

void BattleSystem::AnimateRemoveHealthBar(Entity entity) {
    static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
    static auto& healthbarArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<HealthBar>() };
    static auto& enemyHUDArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<EnemyHUD>() };
    if (m_Entities.size() == 0) {
        return;
    }
    bool found{ false };
    for (Entity& e : enemyHealthBars) {
        Entity healthbarEntity{ healthbarArray.GetData(e).entity };
        if (healthbarEntity == entity) {
            animationArray.GetData(e).Start("Pop Out", e);
            found = true;

            std::vector<Entity> newAllBattleUI{};
            for (Entity& ui : allBattleUI) {
                if (ui != e) {
                    newAllBattleUI.push_back(ui);
                }
            }
            allBattleUI = newAllBattleUI;
        }
        else if (found) {
            animationArray.GetData(e).Queue("Move Up", e);
        }
    }
    found = false;
    for (Entity& e : allyHealthBars) {
        Entity healthbarEntity{ healthbarArray.GetData(e).entity };
        if (healthbarEntity == entity) {
            animationArray.GetData(e).Start("Pop Out", e);
            found = true;

            std::vector<Entity> newAllBattleUI{};
            for (Entity& ui : allBattleUI) {
                if (ui != e) {
                    newAllBattleUI.push_back(ui);
                }
            }
            allBattleUI = newAllBattleUI;
        }
        else if (found) {
            animationArray.GetData(e).Queue("Move Up", e);
        }
    }
}

void BattleSystem::UpdateSkillIcons() {
    static auto& textureArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Tex>() };
    static auto& buttonArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Button>() };
    if (m_Entities.size() == 0) {
        return;
    }

    tooltips.clear();

    if (activeCharacter == nullptr) {
        activeCharacter = turnManage.turnOrderList.front();
    }

    for (int i = 0; i < 3; i++) {
        if (activeCharacter->action.skills.size() < 3) {
            break;
        }
        std::string skillTexture = activeCharacter->action.skills[i].skillTexture;
        tooltips.push_back(activeCharacter->action.skills[i].skillTooltip);
        textureArray.GetData(skillButtons[i]).tex = assetmanager.texture.Get(skillTexture.c_str());
        buttonArray.GetData(skillButtons[i]).hoveredColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
        buttonArray.GetData(skillButtons[i]).defaultColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
    }
}
