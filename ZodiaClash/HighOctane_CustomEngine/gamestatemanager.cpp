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
*	@file		GameStateManager.cpp
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
*	@date		26 October 2023
*
* *****************************************************************************
*
*	@brief		Game State Management
*
*	This file contains the implementation of the game state management system.
*	It manages the different states of the game such as loading, running, and
*	unloading game states. The system is used by the Game State Manager (GSM)
*	to transition between different phases of the game.
*
******************************************************************************/

#include "GameStateManager.h"
#include "Battle.h"
#include "CharacterStats.h"
#include "enginecore.h"

/**
 * @brief Constructor for GameState. Initializes state indices and function pointers.
 */
GameState::GameState() :
                        idx{ -1 }, // indicates whether state is in use or not. -ve for not in use
                        loadFunc{ nullptr },
                        initFunc{ nullptr },
                        updateFunc{ nullptr },
                        drawFunc{ nullptr },
                        freeFunc{ nullptr },
                        unloadFunc{ nullptr } {};

/**
 * @brief Sets the game state and associates the provided functions with the state's lifecycle.
 */
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

/**
 * @brief Calls the load function associated with this game state, if any.
 */
void GameState::GSM_Load() 
{
    if (loadFunc) loadFunc();
}

/**
 * @brief Calls the init function associated with this game state, if any.
 */
void GameState::GSM_Init() 
{
    if (initFunc) initFunc();
}

/**
 * @brief Calls the update function associated with this game state, passing in the delta time.
 */
void GameState::GSM_Update(double dt) 
{
    if (updateFunc) updateFunc(dt);
}

/**
 * @brief Calls the draw function associated with this game state, passing in the delta time.
 */
void GameState::GSM_Draw(double dt) 
{
    if (drawFunc) drawFunc(dt);
}

/**
 * @brief Calls the free function associated with this game state, used for resource deallocation.
 */
void GameState::GSM_Free() 
{
    if (freeFunc) freeFunc();
}

/**
 * @brief Calls the unload function associated with this game state, signaling the end of the state.
 */
void GameState::GSM_Unload() 
{
    if (unloadFunc) unloadFunc();
}

/**
 * @brief Returns the index of this game state.
 * @return int The index of the game state.
 */
int GameState::GetIdx() 
{ 
    return idx; 
}

// Constructors, destructors, and member functions for GameStateMgr
GameStateMgr* GameStateMgr::gamestatemgr_ = nullptr;

/**
 * @brief Constructor for GameStateMgr. Initializes the manager with the specified number of game states.
 */
GameStateMgr::GameStateMgr(unsigned gsCount, int totalLevels) :
                                                gsmState{ GSLOAD },
                                                gameStateCount{ gsCount },
                                                stateArray{ new GameState[gameStateCount]{} },
                                                gameLevel{ new GameLevel(totalLevels) },
                                                state{ 0 }, //gsm will run the first state in the array when initialized
                                                nextState{ -1 },
                                                isRunning{ true },
                                                continueNextState{ true } {}

/**
 * @brief Destructor for GameStateMgr. Cleans up dynamic memory used for storing game states.
 */
GameStateMgr::~GameStateMgr() 
{
    delete[] stateArray;
    delete gameLevel;
}

/**
 * @brief Retrieves the singleton instance of the GameStateMgr, creating it with the specified state count if necessary.
 */
GameStateMgr* GameStateMgr::GetInstance(unsigned gsCount, int totalCount) 
{
    if (!gamestatemgr_) gamestatemgr_ = new GameStateMgr(gsCount, totalCount);
    return gamestatemgr_;
}

/**
 * @brief Retrieves the singleton instance of the GameStateMgr.
 * @return GameStateMgr* The singleton instance of GameStateMgr.
 */
GameStateMgr* GameStateMgr::GetInstance() 
{ 
    return gamestatemgr_;
}

/**
 * @brief Deletes the singleton instance of the GameStateMgr, if it exists.
 */
void GameStateMgr::RemoveInstance() {
    if (gamestatemgr_) delete gamestatemgr_;
    gamestatemgr_ = nullptr;
}

/**
 * @brief Inserts a new game state into the manager at the specified index.
 */
void GameStateMgr::InsertGameState(int gsIdx, LoadFunc load, InitFunc init, UpdateFunc update, DrawFunc draw, FreeFunc free, UnloadFunc unload) {
    std::cout << "inserting game state at idx " << gsIdx << std::endl;
    stateArray[gsIdx].SetGameState(gsIdx, load, init, update, draw, free, unload);
}

/**
 * @brief Updates the state of the GameStateMgr, transitioning between game states as needed.
 */
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
    std::shared_ptr<ParticleSystem> particle = ecs.RegisterSystem<ParticleSystem>();
    double dt = static_cast<double>(GetTime());

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
            particle->Update();

            // Draw or render logic
            stateArray[state].GSM_Draw(dt);
            graphics->Draw();
            particle->Draw();
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

/**
 * @brief Signals the GameStateMgr to transition to the game state with the specified index.
 */
void GameStateMgr::ChangeGameState(int gsIdx) 
{
    nextState = gsIdx;
}

/**
 * @brief Signals the GameStateMgr to quit the game, setting the next state to 0 and stopping further transitions.
 */
void GameStateMgr::QuitGame() 
{
    continueNextState = false; nextState = 0;
}

void GameStateMgr::NextLevel() {
    gameLevel->IncreaseLevel();

    // Check if there is a specific game state associated with each level
    int newLevel = gameLevel->GetCurrentLevel();
    if (newLevel < static_cast<int>(gameStateCount)) {
        //each level has a corresponding game state
        ChangeGameState(newLevel);
    }
    else {
        // if the new level exceeds the number of game states, end the game
        ChangeGameState(GS_QUIT);
    }
}

void GameStateMgr::GoToEndLevel() {
    gameLevel->JumpToEndLevel();
    //last game state is the end state
    ChangeGameState(gameStateCount - 1);
}
