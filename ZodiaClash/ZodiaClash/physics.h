<<<<<<< Updated upstream
//#pragma once
//#include <vector>
//#include "VMath.h"
//#include "Collision.h"
//#include "Body.h"
//
///*
//* TODO:
//* - init
//* - update body position
//* - update forces
//* - consider debug drawing step
//*/
//namespace physics {
//	class PhysicsManager {
//	public:
//		PhysicsManager(); //constructor
//
//		// new ver. arrangment
//	public:
//		std::vector<Body> bodies; // store all the bodies
//		void Initialize(); //initializes 1x instance of physics and relevant (global?) variables
//		void Update(float deltaTime);
//		void Integrate(Body& body, float deltaTime);
//		void DetectCollision(float deltaTime);
//		CollisionManager collision;
//
//		//attributes (some overlap with body, need further clarif.)
//	public:
//		vmath::Vector2 gravity;
//		float maxVelocity;
//		float maxVelocitySq;
//
//		//note: PenetrationEpsilon and PenetrationResolvePercentage in sample, to confirm further usage
//	};
//
//	// global pointer
//	extern PhysicsManager* PHYSICS;
//}
=======
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
		vmath::Vector2 gravity;
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
>>>>>>> Stashed changes
