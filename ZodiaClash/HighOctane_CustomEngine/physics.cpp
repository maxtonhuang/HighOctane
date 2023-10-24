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
        @brief Updates the physics manager.
        @param deltaTime Elapsed time since the last frame in seconds.

        This method is responsible for updating the physics system.
        If in continuous mode, it'll perform physics calculations as long
        as the accumulated time exceeds a defined time step. In step mode,
        it'll update one step at a time when a step update is requested.
     */
     /**************************************************************************/
    /*void PhysicsManager::Update(float deltaTime)
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
        else // When physics simulation is in step mode (it updates one step at a time)
        {
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
        }
    }*/

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
    void PhysicsManager::Integrate(Transform& transformData) 
    {

        // calculate acceleration due to force
        transformData.acceleration = transformData.force * transformData.inverseMass;
        // add gravitational acceleration
        transformData.acceleration += {GRAVITY_X, GRAVITY_Y};
        // update velocity with acceleration and apply friction
        transformData.velocity += transformData.acceleration * g_dt;
        // update position with velocity
        transformData.position += transformData.velocity * g_dt;

        // reset force for the next frame
        transformData.velocity *= FRICTION;
        transformData.force = { 0, 0 };

        std::cout << transformData.velocity.x << " , " << transformData.velocity.y << std::endl;
        std::cout << transformData.position.x << " , " << transformData.position.y << std::endl;

        /*

        // If the body is static, we don't want to update its position or velocity.
        //if (body.isStatic) return;

        // Store the current position as the previous position
        //body.prevPosition = transform.position;

        // Update the position based on deltaTime
        //std::cout << transform.velocity.x << " , " << transform.velocity.y << std::endl;
        //transform.position += transform.velocity;
        //std::cout << transform.position.x << " , " << transform.position.y << std::endl;
        /*
        // Update the acceleration based on the global gravity and any accumulated forces on the body.
        Vector2 newAcceleration = body.accumulatedForce + body.acceleration * 0.1f;

        // Update the velocity using the newly computed acceleration.
        transform.velocity += newAcceleration * deltaTime;
        

        //// Ensure the velocity doesn't exceed a maximum value for numerical stability.
        if (Vector2::dot(transform.velocity, transform.velocity) > maxVelocitySq)
        {
            transform.velocity.normalize();  // Make the velocity a unit vector
            transform.velocity *= maxVelocity;  // Scale it to the maximum allowed velocity
        }
        transform.velocity *= .95f;

        // Reset the accumulated force to zero for the next frame
        body.accumulatedForce = Vector2(0, 0);*/
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
        (void)deltaTime;
        for (const auto& entity : m_Entities) 
        {
            auto& transform = m_ecs.GetComponent<Transform>(entity);
            Integrate(transform); // Using Integrate function to avoid code duplication
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
    void PhysicsManager::DebugDraw(Transform& transform)
    {
        if (!DebugDrawingActive) return;
        //draw the position/center of the body as a point
        graphics.DrawPoint(transform.position.x, transform.position.y, 0.f, 1.f, 0.f);

        //draw velocity as a line
        Vector2 endPosition = transform.position + (transform.velocity * 50.f);
        graphics.DrawLine(transform.position.x, transform.position.y, endPosition.x, endPosition.y, 0.f, 0.f, 1.f);

        //draw AABB box
        Vector2 bottomLeft = transform.position - transform.halfDimensions;
        Vector2 topRight = transform.position + transform.halfDimensions;
        graphics.DrawOutline(bottomLeft.x, bottomLeft.y, topRight.x , topRight.y, 0.f, 0.f, 1.f);
    }
}
