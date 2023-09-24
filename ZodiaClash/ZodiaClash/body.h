#pragma once
#include "vmath.h"
#include "Collision.h"
#include "Physics.h"

using namespace vmath;

//class Shape; //forward declaration

namespace physics {

	struct Body {
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
		bool isStatic{};
		Shape* bodyShape;

		// Constructors, if needed, can remain
		Body();

		// These setters might stay if you find them useful, but in pure ECS they might not be here.
		void SetPosition(Vector2 pos);
		void SetVelocity(Vector2 vel);

		// Getters can also be useful
		Vector2 GetPosition() const { return position; }
		Vector2 GetVelocity() const { return velocity; }
	};
}
