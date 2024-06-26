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
*	@brief		Basic 2D iterative impulse physics engine.
*
*	This file contains functions used in the main physics system of the engine
*
******************************************************************************/

#include "Physics.h"
#include <math.h>

#define FIXED_DT 1.0f/60.f
#define UNREFERENCED_PARAMETER(P) (P)

namespace physics {

    PhysicsManager* PHYSICS = nullptr;

    //Initializer list
    PhysicsManager::PhysicsManager(ECS& ecs, GraphicsManager& graphicsSystem) : m_ecs(ecs), graphics(graphicsSystem)
    {
        PHYSICS = this;
        maxVelocity = 1000;
        maxVelocitySq = maxVelocity * maxVelocity;
        advanceStep = false;
        stepModeActive = false;
    }

    /**************************************************************************/
    /*!
        @brief Adds an entity to the physics manager.
        @param entity The entity to be added.

        This method appends a given entity to the list of entities the
        physics manager is responsible for.
     */
     /**************************************************************************/
    void PhysicsManager::AddEntity(Entity entity)
    {
        m_Entities.push_back(entity);
    }

    /**************************************************************************/
    /*!
        @brief Integrates motion equations for a given body.
        @param body Reference to the body whose physics will be updated.
        @param deltaTime Elapsed time since the last frame in seconds.
        @param transform Reference to the transform component of the body.

        This method updates the position, velocity, and acceleration of
        the body based on the elapsed time. If the body is static, its
        properties won't be updated.
     */
     /**************************************************************************/
    void PhysicsManager::Integrate(Transform& transformData, Collider& colliderData)
    {
        // calculate acceleration due to force
        transformData.acceleration = transformData.force * transformData.inverseMass;
        // update velocity with acceleration and apply friction
        transformData.velocity += transformData.acceleration * FIXED_DT;

        // update position with velocity
        transformData.position += transformData.velocity * FIXED_DT;
        colliderData.position += transformData.velocity * FIXED_DT;

        // apply friction (assuming FRICTION is < 1 and represents a damping factor)
        transformData.velocity *= FRICTION;
        // reset force for the next frame
        transformData.force = { 0, 0 };
    }



    /**************************************************************************/
    /*!
        @brief Integrates motion equations for all bodies in the physics manager.
        @param deltaTime Elapsed time since the last frame in seconds.

        This method loops through each entity in the physics manager
        and updates its physics using the Integrate function.
     */
     /**************************************************************************/
    void PhysicsManager::IntegrateBodies(float deltaTime)
    {
        UNREFERENCED_PARAMETER(deltaTime);
        for (const auto& entity : m_Entities)
        {
            auto& transform = m_ecs.GetComponent<Transform>(entity);
            auto& collider = m_ecs.GetComponent<Collider>(entity);
            Integrate(transform, collider); // Using Integrate function to avoid code duplication
        }
    }

    /**************************************************************************/
    /*!
        @brief Performs physics calculations for a given time step.
        @param deltaTime Elapsed time since the last frame in seconds.

        This method is called to carry out the main physics calculations
        for all bodies during the given time interval.
     */
     /**************************************************************************/
    void PhysicsManager::Step(float deltaTime)
    {
        // Integrate forces and velocities to update positions
        IntegrateBodies(deltaTime);
    }

    /**************************************************************************/
    /*!
        @brief Toggles the step mode in the physics manager.

        This method switches the physics simulation between continuous
        mode and step-by-step mode.
     */
     /**************************************************************************/
    void PhysicsManager::ToggleStepMode()
    {
        stepModeActive = !stepModeActive;
    }

    /**************************************************************************/
    /*!
        @brief Toggles the debug mode in the physics manager.

        This method switches the physics simulation between continuous
        mode and step-by-step mode.
     */
     /**************************************************************************/
    void PhysicsManager::ToggleDebugMode() {
        DebugDrawingActive = !DebugDrawingActive;

    }
    /**************************************************************************/
    /*!
        @brief Requests a single step update in the physics simulation.

        This method is used to trigger a single physics update when
        the physics manager is in step mode.
     */
     /**************************************************************************/
    void PhysicsManager::RequestStep()
    {
        advanceStep = true;
    }

    /**************************************************************************/
    /*!
        @brief Draws debugging visuals for a given body and its transform.
        @param body Reference to the body that needs its debug visuals drawn.
        @param transform Reference to the transform component of the body.

        This method renders various debug visuals such as the body's
        position, velocity vector, and bounding box. The visuals will
        only be rendered if debug drawing is active.
     */
     /**************************************************************************/
    void PhysicsManager::DebugDraw(Transform& transform, Collider& colliderData)
    {
        if (!DebugDrawingActive) return;
        //draw the position/center of the body as a point
        graphics.DrawPoint(transform.position.x, transform.position.y, 0.f, 1.f, 0.f);

        //draw velocity as a line
        Vector2 endPosition = transform.position + (transform.velocity * 50.f);
        graphics.DrawLine(transform.position.x, transform.position.y, endPosition.x, endPosition.y, 0.f, 0.f, 1.f);

        //draw AABB box
        Vector2 bottomLeft = colliderData.position - colliderData.halfDimensions;
        Vector2 topRight = colliderData.position + colliderData.halfDimensions;
        graphics.DrawOutline(bottomLeft.x, bottomLeft.y, topRight.x, topRight.y, 0.f, 0.f, 1.f);
    }
}
