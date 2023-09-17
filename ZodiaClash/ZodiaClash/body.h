#include "vmath.h"
#include "collision.h"
#pragma once

using vmath::Vector2;

///Body Component provides basic point physics dynamics including mass, 
///velocity, forces, acceleration, and collision resolution.
///Component will modify transform component attributes every frame.

namespace physics {
	class Body{
	public:
		Body(); //constructor
		~Body(); //destructor

		void AddForce(Vector2 force);
		void Integrate(float deltaTime);
		void SetPosition(Vector2);
		void SetVelocity(Vector2);
		bool CheckBodyCollision(const Body& otherBody);
		bool CheckBorderCollision();
		//void DebugDraw();//Draw the object using the debug drawer
		//void PublishResults();
		//virtual void Initialize();
		//virtual void Serialize(ISerializer& stream);

		Body* next;
		Body* prev;
		Vector2 position; 
		Vector2 prevPosition;
		Vector2 velocity;
		Vector2 acceleration;
		Vector2 accumulatedForce;
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
