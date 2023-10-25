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
#include <algorithm>

BattleSystem::BattleSystem(BattleSystem const& input) {
    battleState = input.battleState;
    roundManage = input.roundManage;
    turnManage.characterList = input.turnManage.characterList;
    for (CharacterStats& c : turnManage.characterList) {
        c.parent = this;
        c.Start();
    }
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
    roundInProgress = false;
    roundManage.characterCount = 0;
    roundManage.roundCounter = 0;

    DetermineTurnOrder();

    battleState = NEWROUND;
    //StartCoroutine(NewGameDelay(0.5f, 1.f)); //delay at start
}

void BattleSystem::Update() 
{
    switch (battleState) {
    case NEWGAME:
        LOG_WARNING("Initializing battle system");
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
        if (GameObject::FindGameObjectsWithTag("Enemy").size() <= 0) //no enemies left
        {
            LOG_WARNING("State: Win");
            battleState = WIN;
        }
        //then check if enemy has won
        else if (GameObject::FindGameObjectsWithTag("Player").size() <= 0) //no players left
        {
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
    case PLAYERTURN:
    case ENEMYTURN:
        activeCharacter->action.UpdateState();
        if (activeCharacter->action.entityState == EntityState::ENDING) {
            turnManage.turnOrderList.splice(turnManage.turnOrderList.end(), turnManage.turnOrderList, turnManage.turnOrderList.begin()); //SEND TO BACK OF TURN ORDER LIST
            battleState = NEXTTURN;;
        }
        else if (activeCharacter->action.entityState == EntityState::DYING) {
            turnManage.turnOrderList.remove(activeCharacter);
            turnManage.originalTurnOrderList.remove(activeCharacter);
            turnManage.characterList.pop_front();
            //turnManage.characterList.remove(*activeCharacter);
        }
        break;
    }
    //if (battleState == NEWROUND)
    //{
    //    
    //}
    //else if (battleState == NEXTTURN)
    //{
    //    
    //}
    //else if (battleState == PLAYERTURN) {

    //}
}
//
//BattleState BattleSystem::NewGameDelay(float startDelay, float nextDelay)
//{
//    //yield return new WaitForSeconds(startDelay);
//    //return new WaitForSeconds(nextDelay);
//
//    DetermineTurnOrder();
//    battleState = NEWROUND;
//    return battleState;
//}


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
        turnManage.characterList.push_back(*m);
    }

    //turnOrderList
    turnManage.turnOrderList.clear();

    for(CharacterStats chara : turnManage.characterList) //add the sorted charactersList to turnOrderList
    {
        turnManage.turnOrderList.push_back(&chara);
    }

    //originalTurnOrderList
    turnManage.originalTurnOrderList = turnManage.turnOrderList;
    /*
    turnManage.originalTurnOrderList.clear();

    for(CharacterStats chara : turnManage.turnOrderList) //add turnOrderList to original turn order list
    {
        turnManage.originalTurnOrderList.push_back(chara);
    }
    */
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
