#pragma once
#include <vector>
#include "VMath.h"
//#include "Body.h"

namespace physics {

	/*
	* TODO:
	* - collision detection
	* - basic shape data
	* - bring in stuff from resolution
	*/

	// bring in Body class
	struct Body;

	
	// basic line, shape data

	/*struct Linesegment {
		vmath::Vector2 point0;
		vmath::Vector2 point1;
		vmath::Vector2 normal;
	};*/

	/*enum SHAPE_ID {
		SHAPE_CIRCLE,
		SHAPE_BOX,
		NUM_OF_SHAPES
	};*/

	//class Shape {
	//public:
	//	
	//	SHAPE_ID id;
	//	Body* body;
	//	Shape(SHAPE_ID id) : id{ id } , body(nullptr) {};
	//	~Shape() {}
	//	//virtual void Draw() = 0;
	//	//virtual bool TestPoint(vmath::Vector2) = 0;
	//};

	//// Circle shape.
	//class Circle : public Body
	//{
	//public:
	//	Circle() : Shape(SHAPE_CIRCLE), radius{ 0.1f }, isStatic{}, isVulnerable{} {};
	//	float radius;
	//	bool isStatic;
	//	bool isVulnerable;
	//	//virtual void Draw();
	//};

	//// Axis Aligned Bounding Box Shape
	//class AABB : public Body
	//{
	//public:
	//	AABB() : Shape(SHAPE_BOX), min{ 0.f, 0.f }, max{ 0.f, 0.f }, extents{ 0.f, 0.f }, isStatic{}, isVulnerable{} {};
	//	vmath::Vector2 min;
	//	vmath::Vector2 max;
	//	vmath::Vector2 extents;
	//	bool isStatic;
	//	bool isVulnerable;
	//	//virtual void Draw();
	//};

	/**
	// --- to check more on physics side!
	// data to use for contact between two bodies
	struct BodyContact {
		Body* bodies[2];
		vmath::Vector2 movement[2];
		vmath::Vector2 contactNormal;
		//float Penetration;
		//float Restitution;
		//float FrictionCof;

		float seperatingVelocity;
		//float contactImpulse;
		float CalculateSeparatingVelocity();
	};

	// --- to check more on physics side!
	// identified contacts to resolve collisions for
	class ContactSet
	{
	public:
		BodyContact* GetNextContact();
		void ResolveContacts(float dt);
		void Reset();
	private:
		friend class Physics;
		static const int MaxContacts = 1024;
		BodyContact contactArray[MaxContacts];
		unsigned NumberOfContacts;
		void ResolveVelocities(float dt);
		void ResolvePositions(float dt);
	};

	**/

	// function pointer for different collision tests
	//typedef bool (*CollisionTest)
	//	(Shape* alpha,
	//		vmath::Vector2 alphaPos,
	//		Shape* beta,
	//		vmath::Vector2 betaPos);
	//		//ContactSet* c);

	class CollisionManager
	{
	public:
		CollisionManager();
		
		/*
		* CollisionTest Collision[Shape::NUM_OF_SHAPES][Shape::NUM_OF_SHAPES];
		bool GetContacts(Shape* alpha,
								vmath::Vector2 alphaPos,
								Shape* beta,
								vmath::Vector2 betaPos,
								ContactSet* c);*/
		static bool CheckBodyCollision(const Body& alpha, const Body& beta);

		static bool CheckBorderCollision(const Body& alpha);

		//void CollisionDetectionAndResponse();
		//bool CheckCollision(const Body& otherBody);// check collision with another body
		//static bool CheckCollision(const Body& bodyA, const Body& bodyB); // check collision with 2 bodies
		//void CollisionResponse(Body& bodyA, Body& bodyB); 
	};

	extern CollisionManager* COLLISION;
}