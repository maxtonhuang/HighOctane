#pragma once

#include <vector>
#include "vmath.h"
#include "collision.h"

using namespace vmath;

namespace physics {
	class PhysicsManager {
	public:
		PhysicsManager();
		void Update(float deltaTime);

	private:
		void IntegrateBodies(float deltaTime);
		void Step(float deltaTime);
		//CollisionManager collision;

	public:
		std::vector<Body> bodies{}; // store all the bodies
		Vector2 gravity{};
		float maxVelocity{};
		float maxVelocitySq{};
		bool advanceStep{};
		bool stepModeActive{};
		float timeAccumulation{};
		float penetrationEpsilon{};
		float penetrationResolvePercentage{};
	};

	// global pointer declaration
	extern PhysicsManager* PHYSICS;
}

