#pragma once
#include "vmath.h"
#include "Collision.h"
#include "Physics.h"

using namespace vmath;

namespace physics {
	class Body{
	public:
		Body(); //constructor
		~Body(); //destructor
		void AddForce(Vector2 force);
		void Integrate(float deltaTime);
		void SetPosition(Vector2);
		void SetVelocity(Vector2);
		//bool CheckCollision(const Body& otherBody);
		//void DebugDraw();//Draw the object using the debug drawer
		//void PublishResults();
		//virtual void Initialize();
		//virtual void Serialize(ISerializer& stream);

		Body* next{};
		Body* prev{};
		Vector2 position{};
		Vector2 prevPosition{};
		Vector2 velocity{};
		Vector2 acceleration{};
		Vector2 accumulatedForce{};
		float mass{};
		float restitution{};
		float friction{ 0.2f };

		// Indicates if the body is movable
		bool isStatic{};
		Shape* bodyShape;

		/*class Collider {

		};*/
	};
}
