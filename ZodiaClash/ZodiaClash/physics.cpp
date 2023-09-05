#include "physics.h"
#include "vmath.h"
#include <math.h>

namespace physics {

    PhysicsManager* PHYSICS = nullptr;

    void PhysicsManager::Init() {
        //a/n: consider duplicate guard like sample ?
        PHYSICS = this;
        gravity = vmath::Vector2(0, -400);
        maxVelocity = 1000;
        maxVelocitySq = maxVelocity * maxVelocity;
    }
    
    void PhysicsManager::Update(float deltaTime) {
        auto bodies = collision.bodies;
        for (auto& body : bodies) {
            Update(body, deltaTime); 
            //ExternalForces(body, deltaTime); //every frame, update the following for each body 
        }
        collision.CollisionDetectionAndResponse(); //call this function every frame
    }

    void PhysicsManager::Update(PhysicsBody& body, float deltaTime) {
        body.velocity += body.acceleration * deltaTime; // Update velocity based on acceleration
        body.position += body.velocity * deltaTime; // Update position based on velocity(speed with direction)
    }

    void PhysicsManager::DetectCollision(float deltaTime) {
        //expand scope frm PhysicsBody??
        //or use this to call CollisionManager?
    }


    bool PhysicsBody::CheckCollision(const PhysicsBody& otherBody) {
        return CollisionManager::CheckCollision(*this, otherBody);
    };

    //void PhysicsManager::ExternalForces(PhysicsBody& body, float deltaTime) {
    //    // Apply forces like gravity, user input, etc.
    //    // Modify body's acceleration, velocity, or position.
    //}

}