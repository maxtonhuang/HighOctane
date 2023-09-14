#include "physics.h"
#include "vmath.h"
#include <math.h>

namespace physics {

    PhysicsManager* PHYSICS = nullptr;

    PhysicsManager::PhysicsManager()
    {
        PHYSICS = this;
        gravity = vmath::Vector2(0, -400);
        maxVelocity = 1000;
        maxVelocitySq = maxVelocity * maxVelocity;
        advanceStep = false;
        stepModeActive = false;
        timeAccumulation = 0.0f;
        penetrationEpsilon = 0.2f;
        penetrationResolvePercentage = 0.8f;
    }

    void PhysicsManager::Initialize() 
    {
        PHYSICS = this;
        gravity = vmath::Vector2(0, -400);
        maxVelocity = 1000;
        maxVelocitySq = maxVelocity * maxVelocity;
    }

    void PhysicsManager::Update(float deltaTime)
    {
        // Define a constant time step (fixed interval at which physics calculations will be performed)
        const float timeStep = 1.0f / 60.0f; // 60 updates per second

        // When physics simulation is not in step mode (eg.running continuously)
        if (!stepModeActive) {

            // Keeps track of the passed time
            timeAccumulation += deltaTime;

            // If the accumulated time has reached or exceeded the time step
            if (timeAccumulation > timeStep) {

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
        if (advanceStep) {

            // Perform the physics calculations for this time step
            Step(timeStep);

            // Reset the step request flag
            advanceStep = false;
        }
    }

    void PhysicsManager::IntegrateBodies(float deltaTime) 
    {
        for (Body& body : bodies)
        {
            body.Integrate(deltaTime);
        }
    }

    void PhysicsManager::DetectCollision(float deltaTime)
    {

    }

    void PhysicsManager::Step(float deltaTime)
    {
        // Integrate forces and velocities to update positions
        IntegrateBodies(deltaTime);
        // Detect and resolve collisions.
        DetectCollision(deltaTime);
    }

}

//void PhysicsManager::ExternalForces(PhysicsBody& body, float deltaTime) {
//    // Apply forces like gravity, user input, etc.
//    // Modify body's acceleration, velocity, or position.
//}
