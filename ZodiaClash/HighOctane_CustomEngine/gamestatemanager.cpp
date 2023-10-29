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
*	@file		GameStateManager.h
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
*	@date		22 October 2023
*
* *****************************************************************************
*
*	@brief		Game State Manager
*
*	This file contains the main update function for the Game State Manager. It
*	loads the corresponding function pointers when the state of the game is
*	changed. 
*
******************************************************************************/

#include "GameStateManager.h"
#include "Battle.h"
#include "CharacterStats.h"
#include "enginecore.h"

// Constructors and member functions for GameState
GameState::GameState() :
                        idx{ -1 }, // indicates whether state is in use or not. -ve for not in use
                        loadFunc{ nullptr },
                        initFunc{ nullptr },
                        updateFunc{ nullptr },
                        drawFunc{ nullptr },
                        freeFunc{ nullptr },
                        unloadFunc{ nullptr } {};

void GameState::SetGameState(int gsIdx, LoadFunc load, InitFunc init, UpdateFunc update, DrawFunc draw, FreeFunc free, UnloadFunc unload) 
{
    idx = gsIdx;
    loadFunc = load;
    initFunc = init;
    updateFunc = update;
    drawFunc = draw;
    freeFunc = free;
    unloadFunc = unload;
}

void GameState::GSM_Load() 
{
    if (loadFunc) loadFunc();
}

void GameState::GSM_Init() 
{
    if (initFunc) initFunc();
}

void GameState::GSM_Update(float dt) 
{
    if (updateFunc) updateFunc(dt);
}

void GameState::GSM_Draw(float dt) 
{
    if (drawFunc) drawFunc(dt);
}

void GameState::GSM_Free() 
{
    if (freeFunc) freeFunc();
}

void GameState::GSM_Unload() 
{
    if (unloadFunc) unloadFunc();
}

int GameState::GetIdx() 
{ 
    return idx; 
}

// Constructors, destructors, and member functions for GameStateMgr
GameStateMgr* GameStateMgr::gamestatemgr_ = nullptr;

GameStateMgr::GameStateMgr(unsigned gsCount) :
                                                gsmState{ GSLOAD },
                                                gameStateCount{ gsCount },
                                                stateArray{ new GameState[gameStateCount]{} },
                                                state{ 0 }, //gsm will run the first state in the array when initialized
                                                nextState{ -1 },
                                                isRunning{ true },
                                                continueNextState{ true } {}

GameStateMgr::~GameStateMgr() 
{
    // Assuming these managers are present elsewhere in your code
   /* PhysicsSystem::removeInstance();
    CollisionMgr::removeInstance();
    EntityMgr::removeInstance();*/
    delete[] stateArray;
}

GameStateMgr* GameStateMgr::GetInstance(unsigned gsCount) 
{
    if (!gamestatemgr_) gamestatemgr_ = new GameStateMgr(gsCount);
    return gamestatemgr_;
}

GameStateMgr* GameStateMgr::GetInstance() 
{ 
    return gamestatemgr_;
}

void GameStateMgr::RemoveInstance() {
    if (gamestatemgr_) delete gamestatemgr_;
    gamestatemgr_ = nullptr;
}

void GameStateMgr::InsertGameState(int gsIdx, LoadFunc load, InitFunc init, UpdateFunc update, DrawFunc draw, FreeFunc free, UnloadFunc unload) {
    std::cout << "inserting game state at idx " << gsIdx << std::endl;
    stateArray[gsIdx].SetGameState(gsIdx, load, init, update, draw, free, unload);
}

void GameStateMgr::UpdateGameStateMgr()
{
    if (!isRunning) return;

    // Systems and manager access
    ECS& ecs = ECS::ecs();
    //EntityManager* entityManager = ecs.m_EntityManager.get();
    // Assuming GraphicsSystem, PhysicsSystem, and CollisionSystem are part of ECS:
    std::shared_ptr<GraphicsSystem> graphics = ecs.RegisterSystem<GraphicsSystem>();
    std::shared_ptr<PhysicsSystem> physics = ecs.RegisterSystem<PhysicsSystem>();
    std::shared_ptr<CollisionSystem> collision = ecs.RegisterSystem<CollisionSystem>();

    float dt = GetTime();

    switch (gsmState) {
    case GSLOAD:
        stateArray[state].GSM_Load();
        gsmState = GSINIT;
        break;

    case GSINIT:
        stateArray[state].GSM_Init();
        gsmState = GSRUNNING;
        break;

    case GSRUNNING:
        if (nextState < 0) {
            stateArray[state].GSM_Update(dt);

            // Update the systems
            physics->Update();  // will use fixed time-step and handle entities' physics
            collision->Update();

            // Draw or render logic
            stateArray[state].GSM_Draw(dt);
            graphics->Draw();
        }
        else {
            gsmState = GSFREE;
        }
        break;

    case GSFREE:
        stateArray[state].GSM_Free();

        // Assuming you want to delete all entities
        for (Entity entity = 0; entity < ecs.GetEntityCount(); ++entity) {
            ecs.DestroyEntity(entity);
        }

        gsmState = GSUNLOAD;
        break;

    case GSUNLOAD:
        stateArray[state].GSM_Unload();
        gsmState = GSLOAD;
        state = nextState;
        (continueNextState) ? nextState = -1 : isRunning = false;
        break;
    }
}


void GameStateMgr::ChangeGameState(int gsIdx) 
{
    nextState = gsIdx;
}

void GameStateMgr::QuitGame() 
{
    continueNextState = false; nextState = 0;
}
