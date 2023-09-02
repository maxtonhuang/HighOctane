#pragma once
#include <vector>
#include "vmath.h"

struct PhysicsBody {
	//position will be the center of the object 
	vmath::Vector2 position, velocity{ 0,0 }, acceleration{ 0,0 }, size{};
	float bounciness{ 0.5f }; // after collisions
	float friction{ 0.2f };    // Slowing down due to surfaces
	float mass;
	bool isStatic;     // Indicates if the body is movable

	// Constructor
	PhysicsBody(float x, float y, float width, float height, float mass, bool isStatic = false)
		       : position({ x, y }), mass(mass), size({ width, height }), isStatic(isStatic) {};

	bool CheckCollision(const PhysicsBody& otherBody);// check collision with another body

};

class PhysicsEngine {

public:
	std::vector<PhysicsBody> bodies; // store all the bodies
	void update(float deltaTime);
	void update(PhysicsBody& body, float deltaTime);
	void externalForces(PhysicsBody& body, float deltaTime);
	void collisionDetectionAndResponse();
	static bool CheckCollision(const PhysicsBody& bodyA, const PhysicsBody& bodyB); // check collision with 2 bodies
	void CollisionResponse(PhysicsBody& bodyA, PhysicsBody& bodyB);
};
