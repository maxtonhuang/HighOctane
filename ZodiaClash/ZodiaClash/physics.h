#pragma once
#include <vector>
#include "vmath.h"
#include "collision.h"
#include "body.h"

/*
* TODO:
* - init
* - update body position
* - update forces
* - consider debug drawing step
*/
using namespace vmath; 
namespace physics {
	class PhysicsManager {
	public:
		PhysicsManager(); 

	public:
		void Initialize();
		void Update(float deltaTime);

	private:
		void IntegrateBodies(float deltaTime);
		void DetectCollision(float deltaTime);
		void Step(float deltaTime);
		//void DebugDraw(float deltaTime);
		//void PublishResult();
		//CollisionManager collision;

	public:
		std::vector<Body> bodies; // store all the bodies
		Vector2 gravity;
		float maxVelocity;
		float maxVelocitySq;
		bool advanceStep;
		bool stepModeActive;
		float timeAccumulation;
		float penetrationEpsilon;
		//Position correction resolve percentage
		float penetrationResolvePercentage;
	};
	// global pointer
	extern PhysicsManager* PHYSICS;
}
