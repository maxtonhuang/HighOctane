#include "Body.h"
#include "Physics.h"

namespace physics {
	Body::Body()
	{
		position = Vector2(0, 0);
		prevPosition = Vector2(0, 0);
		velocity = Vector2(0, 0);
		acceleration = Vector2(0, 0);
		accumulatedForce = Vector2(0, 0);
		mass = 0.0f;
		restitution = 0.0f;
		friction = 0.0f;
		isStatic = false;
	}

	/*Body::~Body()
	{
		delete BodyShape;
		PHYSICS->bodies.erase(this);
	}*/

	void Body::Integrate(float deltaTime)
	{
		// If the body is static, we don't want to update its position or velocity.
		if (isStatic) return;

		// Store the current position as the previous position
		prevPosition = position;

		// Update the position base on deltatime
		position += velocity * deltaTime;

		// Update the acceleration based on the global gravity and any accumulated forces on the body.
		acceleration = PHYSICS->gravity;
		Vector2 newAcceleration = accumulatedForce + acceleration;

		// Update the velocity using the newly computed acceleration.
		//velocity is speed with direction
		velocity += newAcceleration * deltaTime;

		// Ensure the velocity doesn't exceed a maximum value for numerical stability.
		if (Vector2::dot(velocity, velocity) > PHYSICS->maxVelocitySq) // Use dot product to get the squared magnitude
		{
			velocity.normalize(); // Make the velocity a unit vector
			velocity *= PHYSICS->maxVelocity; // Scale it to the maximum allowed velocity
		}

		// Reset the accumulated force to zero for the next frame
		accumulatedForce = Vector2(0, 0);
	}

	void Body::AddForce(Vector2 force)
	{
		accumulatedForce += force;
	}

	void Body::SetPosition(Vector2 pos)
	{
		position = pos;
	}

	void Body::SetVelocity(Vector2 vel)
	{
		velocity = vel;
	}

	/*bool Body::CheckCollision(const Body& otherBody)
	{
		return CollisionManager::CheckCollision(*this, otherBody);
	};*/
}
