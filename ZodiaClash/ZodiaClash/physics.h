#pragma once

#include <vector>
#include "vmath.h"
#include "ECS.h" // will include the Body component definition
#include "collision.h"
#include "body.h"

using namespace vmath;

struct Body; //forward declaration

namespace physics {
    class PhysicsManager {
    public:
        PhysicsManager(ECS& ecs, GraphicsManager& graphicsSystem);
        void Update(float deltaTime);
        void AddEntity(Entity entity);
        void Integrate(Body& body, float deltaTime);
        void AddForce(Body& body, Vector2 force);

    private:
        void IntegrateBodies(float deltaTime);
        void Step(float deltaTime);
        void DebugDraw();

        ECS& m_ecs; // Reference to the ECS instance
        std::vector<Entity> m_Entities;
        float maxVelocity{};
        float maxVelocitySq{};
        bool advanceStep{};
        bool stepModeActive{};
        float timeAccumulation{};
        float penetrationEpsilon{};
        float penetrationResolvePercentage{};
        bool DebugDrawingActive{};
        GraphicsManager& graphics; 
    };

	// global pointer declaration
	extern PhysicsManager* PHYSICS;
}

