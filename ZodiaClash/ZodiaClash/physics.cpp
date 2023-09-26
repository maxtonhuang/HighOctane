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
*	@file		Physics.cpp
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
*	@brief		Physics system of the engine
*
*	This file contains functions used in the main physics system of the engine
*
******************************************************************************/

#include "Physics.h"
#include <math.h>

namespace physics {

    PhysicsManager* PHYSICS = nullptr;

    PhysicsManager::PhysicsManager(ECS& ecs, GraphicsManager& graphicsSystem) : m_ecs(ecs), graphics(graphicsSystem)
    {
        PHYSICS = this;
        maxVelocity = 1000;
        maxVelocitySq = maxVelocity * maxVelocity;
        advanceStep = false;
        stepModeActive = false;
        timeAccumulation = 0.0f;
        penetrationEpsilon = 0.2f;
        penetrationResolvePercentage = 0.8f;
    }

    void PhysicsManager::Update(float deltaTime)
    {
        // Define a constant time step (fixed interval at which physics calculations will be performed)
        const float timeStep = 1.0f / 60.0f; // 60 updates per second

        // When physics simulation is not in step mode (eg.running continuously)
        if (!stepModeActive) 
        {
            // Keeps track of the passed time
            timeAccumulation += deltaTime;

            // If the accumulated time has reached or exceeded the time step
            if (timeAccumulation > timeStep) 
            {
                // Subtract the time step from the accumulated time
                timeAccumulation -= timeStep;

                // Perform the physics calculations for this time step
                Step(timeStep);
            }
        }

        // When physics simulation is in step mode (it updates one step at a time)
        // Reset the accumulated time 
        timeAccumulation = 0.0f;

        // If a step update is requested
        if (advanceStep) 
        {

            // Perform the physics calculations for this time step
            Step(timeStep);

            // Reset the step request flag
            advanceStep = false;
        }
        DebugDraw();
    }

    void PhysicsManager::AddEntity(Entity entity) 
    {
        m_Entities.push_back(entity);
    }

    void PhysicsManager::Integrate(Body& body, float deltaTime) 
    {
        // If the body is static, we don't want to update its position or velocity.
        if (body.isStatic) return;

        // Store the current position as the previous position
        body.prevPosition = body.position;

        // Update the position based on deltaTime
        body.position += body.velocity * deltaTime;

        // Update the acceleration based on the global gravity and any accumulated forces on the body.
        Vector2 newAcceleration = body.accumulatedForce + body.acceleration;

        // Update the velocity using the newly computed acceleration.
        body.velocity += newAcceleration * deltaTime;

        // Ensure the velocity doesn't exceed a maximum value for numerical stability.
        if (Vector2::dot(body.velocity, body.velocity) > PHYSICS->maxVelocitySq) 
        {
            body.velocity.normalize();  // Make the velocity a unit vector
            body.velocity *= PHYSICS->maxVelocity;  // Scale it to the maximum allowed velocity
        }

        // Reset the accumulated force to zero for the next frame
        body.accumulatedForce = Vector2(0, 0);
    }

    void PhysicsManager::AddForce(Body& body, Vector2 force) 
    {
        body.accumulatedForce += force;
    }


    void PhysicsManager::IntegrateBodies(float deltaTime)
    {
        for (const auto& entity : m_Entities) 
        {
            auto& body = m_ecs.GetComponent<physics::Body>(entity);

            // Integrate the body.
            Integrate(body, deltaTime); // Using Integrate function to avoid code duplication
        }
    }

    void PhysicsManager::Step(float deltaTime)
    {
        // Integrate forces and velocities to update positions
        IntegrateBodies(deltaTime);
        // Detect and resolve collisions.
        //DetectCollision(deltaTime);
    }

    void PhysicsManager::DebugDraw()
    {
        if (!DebugDrawingActive) return;
        for (const auto& entity : m_Entities)
        {
            auto& body = m_ecs.GetComponent<physics::Body>(entity);

            //draw the position/center of the body as a point
            graphics.DrawPoint(body.position.x, body.position.y);

            //draw velocity as a line
            Vector2 endPosition = body.position + body.velocity;
            graphics.DrawLine(body.position.x, body.position.y, endPosition.x, endPosition.y);

            //draw AABB box
            Vector2 bottomLeft = body.position - body.halfDimensions;
            Vector2 topRight = body.position + body.halfDimensions;
            graphics.DrawRect(bottomLeft.x, bottomLeft.y, topRight.x - bottomLeft.x, topRight.y - bottomLeft.y);
        }
    }
}
