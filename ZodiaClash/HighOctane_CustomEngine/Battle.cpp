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
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
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
#include "CharacterStats.h"
#include "GameStateManager.h"
#include "debuglog.h"
#include "Events.h"
#include "GameAITree.h"
#include "ECS.h"
#include <algorithm>

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

void BattleSystem::Initialize() 
{
    LOG_WARNING("Initializing battle system");
    events.ConnectBattleSystem(this);
    roundInProgress = false;
    roundManage.characterCount = 0;
    roundManage.roundCounter = 0;

    DetermineTurnOrder();

    printf("\nBeginning battle:\n");
    for (auto& c : turnManage.characterList) {
        std::string name = ECS::ecs().GetComponent<Name>(c.entity).name;
        DEBUG_PRINT("%s remaining health: %f", name.c_str(), c.stats.health);
        printf("Character loaded: %s, remaining health: %f\n", name.c_str(), c.stats.health);
    }

    battleState = NEWROUND;
    //StartCoroutine(NewGameDelay(0.5f, 1.f)); //delay at start
}

void BattleSystem::Update() 
{
    int enemyAmount = 0;
    int playerAmount = 0;
    TreeManager gameAI{};
    
    // Access component arrays through the ComponentManager
    ComponentArray<CharacterStats>* statsArray{};
    ComponentArray<Model>* modelArray{};
    if (m_Entities.size() > 0) {
        ComponentManager& componentManager = ECS::ecs().GetComponentManager();
        statsArray = &componentManager.GetComponentArrayRef<CharacterStats>();
        modelArray = &componentManager.GetComponentArrayRef<Model>();

    }

    switch (battleState) {
    case NEWGAME:
        //LOG_WARNING("Initializing battle system");
        Initialize();
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
        LOG_WARNING("State: Next Turn");

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
                printf("\nYOU WIN!\n");
            }
            LOG_WARNING("State: Win");
            battleState = WIN;
        }
        //then check if enemy has won
        else if (!playerAmount) //no players left
        {
            if (m_Entities.size() > 0) {
                printf("\nYOU LOSE!\n");
            }
            LOG_WARNING("State: Lose");
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

                    LOG_WARNING("State: Enemy Turn");

                    battleState = ENEMYTURN;
                }
                else if (activeCharacter->tag == CharacterType::PLAYER)
                {
                    turnManage.activePlayer = activeCharacter->gameObject.name;
                    turnManage.activeEnemy = "";

                    LOG_WARNING("State: Player Turn");

                    battleState = PLAYERTURN;
                }
                if (activeCharacter->action.entityState != EntityState::DYING) {
                    activeCharacter->action.entityState = EntityState::START;
                }
                ++roundManage.characterCount;
            }
            else
            {
                LOG_WARNING("State: End Round");
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
    case PLAYERTURN:
        activeCharacter->action.UpdateState();
        if (activeCharacter->action.entityState == EntityState::ENDING) {
            if (m_Entities.size() > 0) {
                for (auto& c : turnManage.characterList) {
                    std::string name = ECS::ecs().GetComponent<Name>(c.entity).name;
                    printf("%s remaining health: %f\n", name.c_str(), c.stats.health);
                    DEBUG_PRINT("%s remaining health: %f", name.c_str(), c.stats.health);
                }
            }
            turnManage.turnOrderList.splice(turnManage.turnOrderList.end(), turnManage.turnOrderList, turnManage.turnOrderList.begin()); //SEND TO BACK OF TURN ORDER LIST
            battleState = NEXTTURN;
        }
        else if (activeCharacter->action.entityState == EntityState::DYING) {
            if (m_Entities.size() > 0) {
                std::string name = ECS::ecs().GetComponent<Name>(activeCharacter->entity).name;
                printf("%s died\n", name.c_str());
                DEBUG_PRINT("%s died", name.c_str());
            }
            turnManage.turnOrderList.remove(activeCharacter);
            turnManage.originalTurnOrderList.remove(activeCharacter);
            turnManage.characterList.remove(*activeCharacter);
            battleState = NEXTTURN;
            //turnManage.characterList.remove(*activeCharacter);
        }
        break;
    }
    for (Entity entity : m_Entities) {
        CharacterStats* cs = &statsArray->GetData(entity);
        if (cs->action.entityState == DEAD) {
            continue;
        }
        bool found = false;
        for (CharacterStats const& c : turnManage.characterList) {
            if (c.entity == entity) {
                *cs = c;
                found = true;
                break;
            }
        }
        if (found == false) {
            Model* model = &modelArray->GetData(entity);
            cs->action.entityState = DEAD;
            model->SetAlpha(0.2f);
        }
    }
}

void BattleSystem::DetermineTurnOrder()
{
    //charactersList
    //getting an array of CharacterStats component 
    ComponentArray<CharacterStats>& characters =  ECS::ecs().GetComponentManager().GetComponentArrayRef<CharacterStats>(); // GameObject::FindObjectsOfType();

    turnManage.characterList.clear();

    for(auto& chara : m_Entities)
    {
        CharacterStats* m = &characters.GetData(chara);
        m->entity = chara;
        m->parent = this;
        m->action.battleManager = this;
        turnManage.characterList.push_back(*m);
    }

    //turnOrderList
    turnManage.turnOrderList.clear();

    for(CharacterStats& chara : turnManage.characterList) //add the sorted charactersList to turnOrderList
    {
        turnManage.turnOrderList.push_back(&chara);
    }

    //originalTurnOrderList
    turnManage.originalTurnOrderList = turnManage.turnOrderList;
}

std::vector<CharacterStats*> BattleSystem::GetPlayers() {
    std::vector<CharacterStats*> output;
    for (auto& c : turnManage.characterList) {
        if (c.tag == CharacterType::PLAYER) {
            output.push_back(&c);
        }
    }
    return output;
}
std::vector<CharacterStats*> BattleSystem::GetEnemies() {
    std::vector<CharacterStats*> output;
    for (auto& c : turnManage.characterList) {
        if (c.tag == CharacterType::ENEMY) {
            output.push_back(&c);
        }
    }
    return output;
}

void BattleSystem::SwitchTurnOrder(CharacterStats* target)
{
    turnManage.turnOrderList.remove(target);
    auto iterator = turnManage.turnOrderList.begin();
    iterator++;
    turnManage.turnOrderList.insert(iterator, target);
}

void BattleSystem::RevertTurnOrder(CharacterStats* target)
{
    auto& ogTurnList = turnManage.originalTurnOrderList;
    auto originalIndex = std::find(ogTurnList.begin(), ogTurnList.end(), target);
    turnManage.turnOrderList.remove(target);
    turnManage.turnOrderList.insert(originalIndex, target);
}
