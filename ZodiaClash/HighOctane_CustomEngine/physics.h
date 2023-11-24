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
*	@file		Physics.h
*
*	@author		Liu Wan Ting
*
*	@email		wanting.liu@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		24 September 2023
*
* *****************************************************************************
*
*	@brief		Basic 2D iterative impulse physics engine.
*
*	This file contains functions used in the main physics system of the engine
*
******************************************************************************/

#pragma once
#include <vector>
#include "vmath.h"
#include "ECS.h" 
#include "collision.h"
#include "graphics.h"
#include "EngineCore.h"

using namespace vmath;
#define FRICTION 0.98f
#define GRAVITY_X 0.0f
#define GRAVITY_Y -9.81f


class GraphicsManager;

namespace physics {

    /**************************************************************************/
    /*!
        @class PhysicsManager
        @brief Manages physics simulation and provides core physics functionalities.

        This class is responsible for updating physics entities,
        integrating their motion, handling forces, and rendering debug information.
        It communicates with an Entity Component System (ECS) to access and
        modify components of entities. Additionally, it interfaces with the
        GraphicsManager to render debug visualizations.
    */
    /**************************************************************************/
    class PhysicsManager {
    public:
        PhysicsManager(ECS& ecs, GraphicsManager& graphicsSystem);
        void Update(float deltaTime);
        void AddEntity(Entity entity);
        void Integrate(Transform& transform, Collider& colliderData);
        void DebugDraw(Transform& transform, Collider& colliderData);
        void ToggleStepMode();
        void ToggleDebugMode();
        bool GetDebugDrawActive() { return DebugDrawingActive; }
        bool GetStepModeActive() { return stepModeActive; }
        void RequestStep();
    private:
        void IntegrateBodies(float deltaTime);
        void Step(float deltaTime);
        ECS& m_ecs; // Reference to the ECS instance
        std::vector<Entity> m_Entities;
        float maxVelocity{};
        float maxVelocitySq{};
        bool DebugDrawingActive{};
        bool advanceStep{};
        bool stepModeActive{};
        GraphicsManager& graphics;
    };

    // global pointer declaration
    extern PhysicsManager* PHYSICS;
}
