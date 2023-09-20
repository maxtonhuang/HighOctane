#include "Body.h"
#include "Physics.h"

namespace physics {

	Body::Body(): 
				position(0, 0),
				prevPosition(0, 0),
				velocity(0, 0),
				acceleration(0, 0),
				accumulatedForce(0, 0),
				mass(0.0f),
				restitution(0.0f),
				friction(0.2f),
				isStatic(false),
				bodyShape(nullptr)
	{}

	void Body::SetPosition(Vector2 pos) 
	{
		position = pos;
	}

	void Body::SetVelocity(Vector2 vel) 
	{
		velocity = vel;
	}
}
