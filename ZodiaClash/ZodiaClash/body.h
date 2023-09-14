#include "vmath.h"
#include "collision.h"
#pragma once

///Body Component provides basic point physics dynamics including mass, 
///velocity, forces, acceleration, and collision resolution.
///Component will modify transform component attributes every frame.

namespace physics {
	class Body{
	public:
		Body(); //constructor
		~Body(); //destructor

		void AddForce(vmath::Vector2 force);
		void Integrate(float deltaTime);
		void SetPosition(vmath::Vector2);
		void SetVelocity(vmath::Vector2);
		bool CheckCollision(const Body& otherBody);
		//void DebugDraw();//Draw the object using the debug drawer
		//void PublishResults();
		//virtual void Initialize();
		//virtual void Serialize(ISerializer& stream);

		Body* next;
		Body* prev;
		vmath::Vector2 position; 
		vmath::Vector2 prevPosition;
		vmath::Vector2 velocity;
		vmath::Vector2 acceleration;
		vmath::Vector2 accumulatedForce;
		float mass;
		float restitution;
		float friction{ 0.2f };

		// Indicates if the body is movable
		bool isStatic;
		Shape* bodyShape;

		class Collider {

		};
	};
}
