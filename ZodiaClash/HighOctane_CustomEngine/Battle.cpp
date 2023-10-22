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
#include "GameStateManager.h"
#include "debuglog.h"

void BattleSystem::Initialize() 
{
    battleState = NEWGAME;

    roundInProgress = false;
    roundManage.characterCount = 0;
    roundManage.roundCounter = 0;

    //StartCoroutine(NewGameDelay(0.5f, 1.f)); //delay at start
}

void BattleSystem::Update() 
{
    if (battleState == NEWROUND)
    {
        if (!roundInProgress)
        {
            roundInProgress = true;
            ++roundManage.roundCounter;

            battleState = NEXTTURN;
        }
    }
    else if (battleState == NEXTTURN)
    {
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
                activeCharacter = turnManage.turnOrderList[0];

                if (activeCharacter.gameObject.tag.tag == "Enemy")
                {
                    turnManage.activeEnemy = activeCharacter.gameObject.name;
                    turnManage.activePlayer = "";

                    LOG_WARNING("State: Enemy Turn");

                    battleState = ENEMYTURN;
                }
                else if (activeCharacter.tag.tag == "Player")
                {
                    turnManage.activePlayer = activeCharacter.gameObject.name;
                    turnManage.activeEnemy = "";

                    LOG_WARNING("State: Player Turn");

                    battleState = PLAYERTURN;
                }
                ++roundManage.characterCount;
            }
            else
            {
                LOG_WARNING("State: End Round");
                battleState = NEWROUND;

                activeCharacter.isnull = true;
                turnManage.activeEnemy = "";
                turnManage.activePlayer = "";

                roundManage.characterCount = 0;
                roundInProgress = false;
            }
        }
    }
}

BattleState BattleSystem::NewGameDelay(float startDelay, float nextDelay)
{
    //yield return new WaitForSeconds(startDelay);
    //return new WaitForSeconds(nextDelay);

    DetermineTurnOrder();
    battleState = NEWROUND;
    return battleState;
}


void BattleSystem::DetermineTurnOrder()
{
    //charactersList
    //getting an array of CharacterStats component 
    ComponentArray<CharacterStats> characters =  ECS::ecs().GetComponentManager().GetComponentArrayRef<CharacterStats>(); // GameObject::FindObjectsOfType();

    turnManage.characterList.clear();

    for(auto& chara : m_Entities)
    {
        CharacterStats* m = &characters.GetData(chara);
        turnManage.characterList.push_back(*m);
    }

    //turnOrderList
    turnManage.turnOrderList.clear();

    for(CharacterStats chara : turnManage.characterList) //add the sorted charactersList to turnOrderList
    {
        turnManage.turnOrderList.push_back(chara);
    }

    //originalTurnOrderList
    turnManage.originalTurnOrderList.clear();

    for(CharacterStats chara : turnManage.turnOrderList) //add turnOrderList to original turn order list
    {
        turnManage.originalTurnOrderList.push_back(chara);
    }
}
