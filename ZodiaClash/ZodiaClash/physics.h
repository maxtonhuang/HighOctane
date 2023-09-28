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
#include "body.h"
#include "graphics.h"

using namespace vmath;

class GraphicsManager;

namespace physics {
    class PhysicsManager {
    public:
        PhysicsManager(ECS& ecs, GraphicsManager& graphicsSystem);
        void Update(float deltaTime);
        void AddEntity(Entity entity);
        void Integrate(Body& body, float deltaTime, Transform& transform);
        void AddForce(Body& body, Vector2 force);
        void DebugDraw(physics::Body& body, Transform& transform);
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
        float timeAccumulation{};
        float penetrationEpsilon{};
        float penetrationResolvePercentage{};
        bool DebugDrawingActive{};
        bool advanceStep{};
        bool stepModeActive{};
        GraphicsManager& graphics; 
    };

	// global pointer declaration
	extern PhysicsManager* PHYSICS;
}

//for ECS
namespace physics {
    struct Body;
}

struct Collider {
    physics::Body* body;
    //if true, don't perform physical response, just notify that theres collision
    bool                    isTrigger;
    //list of entities this collider is in contact with
    std::vector<Entity*>    collidedEntities;
};
