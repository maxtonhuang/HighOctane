#pragma once
#include <vector>
#include "vmath.h"
#include "collision.h"

namespace physics {

	/*
	* TODO:
	* - init
	* - update body position
	* - update forces
	* - consider debug drawing step
	*/

	class PhysicsManager {

	// new ver. arrangment
	public:
		void Init(); //initializes 1x instance of physics and relevant (global?) variables
		void Update(float deltaTime);
		void Update(PhysicsBody& body, float deltaTime);
		void DetectCollision(float deltaTime);
		CollisionManager collision;

	//attributes (some overlap with body, need further clarif.)
	public:
		vmath::Vector2 gravity;
		float maxVelocity;
		float maxVelocitySq;

		//note: PenetrationEpsilon and PenetrationResolvePercentage in sample, to confirm further usage


	//// old ver. arrangement
	//public:
	//	std::vector<PhysicsBody> bodies; // store all the bodies
	//	void Update(float deltaTime);
	//	void Update(PhysicsBody& body, float deltaTime);
	//	void ExternalForces(PhysicsBody& body, float deltaTime);
	//	void CollisionDetectionAndResponse();
	//	static bool CheckCollision(const PhysicsBody& bodyA, const PhysicsBody& bodyB); // check collision with 2 bodies
	//	void CollisionResponse(PhysicsBody& bodyA, PhysicsBody& bodyB);

	};

	// global pointer
	extern PhysicsManager* PHYSICS;
}
