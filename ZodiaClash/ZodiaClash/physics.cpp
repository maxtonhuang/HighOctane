#include "Physics.h"
#include <math.h>

namespace physics {

    PhysicsManager* PHYSICS = nullptr;

    PhysicsManager::PhysicsManager(ECS& ecs) : m_ecs(ecs)
    {
        PHYSICS = this;
        gravity = Vector2(0, -400);
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
        body.acceleration = PHYSICS->gravity;
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
            if (!body.isStatic)
            {
                body.velocity += body.acceleration * deltaTime;
                body.position += body.velocity * deltaTime;
            }
        }
    }

    void PhysicsManager::Step(float deltaTime)
    {
        // Integrate forces and velocities to update positions
        IntegrateBodies(deltaTime);
        // Detect and resolve collisions.
        //DetectCollision(deltaTime);
    }

    void DebugDraw()
    {

    }


    //void CollisionManager::CollisionDetectionAndResponse() {
    //    //for each body, check for collision with the all other bodies in the container
    //    for (size_t i = 0; i < bodies.size(); ++i) {
    //        for (size_t j = i + 1; j < bodies.size(); ++j) {
    //            //if collied, execute collsion response
    //            if (CheckCollision(bodies[i], bodies[j])) {
    //                CollisionResponse(bodies[i], bodies[j]);
    //            }
    //        }
    //    }
    //}

    //bool CollisionManager::CheckCollision(const Body& bodyA, const Body& bodyB) {
    //    // Compare AABBs (Axis-Aligned Bounding Boxes) of the bodies
    //    float half_sumWidth = (bodyA.size.x + bodyB.size.x) * 0.5f, half_sumHeight = (bodyA.size.y + bodyB.size.y) * 0.5f;

    //    // Horizontal: CHeck the difference between the x coordinate of the body's center positions and bodies' width
    //    bool xOverlap = abs(bodyA.position.x - bodyB.position.x) <= half_sumWidth; //overlap if less than half width

    //    // Vertical: CHeck the difference between the y coordinate of the body's center positions and bodies' heigh
    //    bool yOverlap = abs(bodyA.position.y - bodyB.position.y) <= half_sumHeight; //overlap if less than half height

    //    // if both x and y axes overlap, a collision is detected, return true
    //    return xOverlap && yOverlap;
    //}


    //void CollisionManager::CollisionResponse(Body& bodyA, Body& bodyB) {
    //    // Calculate relative velocity , how the bodies are moving relative to each other after the collision
    //    vmath::Vector2 relativeVelocity = bodyB.velocity - bodyA.velocity;

    //    // Calculate the collision direction vector (direction from A to B), normalized so length = 1
    //    vmath::Vector2 collisionDirection = (bodyB.position - bodyA.position).normalize();

    //    //measure how fast the bodies are moving towards each other along the direction of collision
    //    float velocityAlongDirection = relativeVelocity ^ (collisionDirection);

    //    // checking if the relative motion is away from the collision point (bodies moving away from each other)
    //    if (velocityAlongDirection > 0) {
    //        return; // bodies are already separating, no need to resolve (not gonna colide)
    //    }

    //    // use the bounciness of both bodies to calculate the scalar value to be applied 
    //    // for collision response IN OPPOSITE DIRECTION hence -ve (0 = no bounce, 1 = full bounce)
    //    float bounceScalar = -(std::min(bodyA.bounciness, bodyB.bounciness)) * velocityAlongDirection;
    //    bounceScalar /= (1 / bodyA.mass + 1 / bodyB.mass); //adjusts the impulse scalar based on the masses of the bodies

    //    // Apply rebounce to each body based on mass and collision direction
    //    vmath::Vector2 bounce = collisionDirection * bounceScalar;
    //    bodyA.velocity -= bounce / bodyA.mass;
    //    bodyB.velocity += bounce / bodyB.mass;

    //}
}
