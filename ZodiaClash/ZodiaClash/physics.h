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
        PhysicsManager(Architecture::ECS& ecs);
        void Update(float deltaTime);
        void AddEntity(Architecture::Entity entity);
        void Integrate(Body& body, float deltaTime);
        void AddForce(Body& body, Vector2 force);

    private:
        void IntegrateBodies(float deltaTime);
        void Step(float deltaTime);
        void DebugDraw();

        Architecture::ECS& m_ecs; // Reference to the ECS instance
        std::vector<Architecture::Entity> m_Entities;

        Vector2 gravity{};
        float maxVelocity{};
        float maxVelocitySq{};
        bool advanceStep{};
        bool stepModeActive{};
        float timeAccumulation{};
        float penetrationEpsilon{};
        float penetrationResolvePercentage{};
        bool DebugDrawingActive{};
    };

	// global pointer declaration
	extern PhysicsManager* PHYSICS;
}

