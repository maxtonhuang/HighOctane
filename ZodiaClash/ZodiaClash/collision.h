#pragma once
#include <vector>
#include "VMath.h"
#include "Body.h"
namespace physics {

	/*
	* TODO:
	* - collision detection
	* - basic shape data
	* - bring in stuff from resolution
	*/

	// bring in Body class
	class Body;

	
	// basic line, shape data

	struct Linesegment {
		vmath::Vector2 point0;
		vmath::Vector2 point1;
		vmath::Vector2 normal;
	};


	class Shape {
	public:
		enum SHAPE_ID {
			SHAPE_CIRCLE,
			SHAPE_BOX,
			NUM_OF_SHAPES
		};
		SHAPE_ID id;
		Body* body;
		Shape(SHAPE_ID shapeId) : id(shapeId), body(nullptr) {};
		~Shape() {}
		//virtual void Draw() = 0;
		//virtual bool TestPoint(vmath::Vector2) = 0;
	};

	// Circle shape.
	class Circle : public Shape
	{
	public:
		Circle() : Shape(SHAPE_CIRCLE) {};
		vmath::Vector2 center;
		float radius;
		bool isStatic;
		bool isVulnerable;
		//virtual void Draw();
	};

	// Axis Aligned Bounding Box Shape
	class AABB : public Shape
	{
	public:
		AABB() : Shape(SHAPE_BOX) {};
		vmath::Vector2 min;
		vmath::Vector2 max;
		bool isStatic;
		bool isVulnerable;
		//virtual void Draw();
	};

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
	


	// function pointer for different collision tests
	typedef bool (*CollisionTest)
		(Shape* alpha, 
			vmath::Vector2 alphaPos, 
			Shape* beta,
			vmath::Vector2 betaPos,
			ContactSet* c);
	**/

	class CollisionManager
	{
	public:
		//std::vector<Body> bodies; // store all the bodies -- to reloc


		// SECTION: COLLISION DETECTION
		bool CheckCollisionBoxBox(const AABB& aabb1,
			const vmath::Vector2& vel1,
			const AABB& aabb2,
			const vmath::Vector2& vel2);

		bool CheckCollisionCircleCircle(const Circle& circle1,
			const vmath::Vector2& vel1,
			const Circle& circle2,
			const vmath::Vector2& vel2);

		bool CheckCollisionCircleBox(const Circle& circle,
			const vmath::Vector2& vel1,
			const AABB& aabb,
			const vmath::Vector2& vel2);

		bool CheckCollisionBoxCircle(const AABB& aabb,
			const vmath::Vector2& vel1,
			const Circle& circle,
			const vmath::Vector2& vel2);

		bool CheckCollisionBoxBorder(const AABB& aabb,
			const vmath::Vector2& vel);

		bool CheckCollisionBCircleBorder(const Circle& circle,
			const vmath::Vector2& vel);

		
		// SECTION: COLLISION RESPONSE
		void OnCollideSnap(Shape::SHAPE_ID alpha, Shape::SHAPE_ID beta);
		void OnCollideDestroy(Shape::SHAPE_ID alpha, Shape::SHAPE_ID beta);

		
		// SECTION: CORE COLLISION MANAGER
		CollisionManager();
		/*CollisionTest Collision[Shape::NUM_OF_SHAPES][Shape::NUM_OF_SHAPES];
		bool GetContacts(Shape* alpha,
								vmath::Vector2 alphaPos,
								Shape* beta,
								vmath::Vector2 betaPos,
								ContactSet* c);*/
		void CheckCollision(Shape::SHAPE_ID alpha, Shape::SHAPE_ID beta);

		//void CollisionDetectionAndResponse();
		//bool CheckCollision(const Body& otherBody);// check collision with another body
		//static bool CheckCollision(const Body& bodyA, const Body& bodyB); // check collision with 2 bodies
		//void CollisionResponse(Body& bodyA, Body& bodyB); 
	};

}