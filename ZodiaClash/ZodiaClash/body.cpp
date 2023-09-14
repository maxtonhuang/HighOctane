#include "body.h"
#include "physics.h"

namespace physics {
	Body::Body()
	{
		position = Vec2(0, 0);
		prevPosition = Vec2(0, 0);
		velocity = Vec2(0, 0);
		acceleration = Vec2(0, 0);
		accumulatedForce = Vector2(0, 0);
		mass = 0.0f;
		restitution = 0.0f;
		friction = 0.0f;
		isStatic = false;
	}

	Body::~Body()
	{
		delete bodyShape;
		PHYSICS->Bodies.erase(this);
	}

	void Body::Integrate(float deltaTime)
	{
		// If the body is static, we don't want to update its position or velocity.
		if (isStatic) return;

		// Store the current position as the previous position
		prevPosition = position;

		// Update the position base on deltatime
		position += velocity * dt;

		// Update the acceleration based on the global gravity and any accumulated forces on the body.
		acceleration = PHYSICS->gravity;
		vmath::Vector2 newAcceleration = accumulatedForce + acceleration;

		// Update the velocity using the newly computed acceleration.
		//velocity is speed with direction
		velocity += newAcceleration * dt;

		// Ensure the velocity doesn't exceed a maximum value for numerical stability.
		if (velocity.dot(velocity) > PHYSICS->maxVelocitySq) // Use dot product to get the squared magnitude
		{
			velocity.normalize(); // Make the velocity a unit vector
			velocity *= PHYSICS->maxVelocity; // Scale it to the maximum allowed velocity
		}

		// Reset the accumulated force to zero for the next frame
		accumulatedForce = vmath::Vector2(0, 0);
	}

	void Body::AddForce(vmath::Vector2 force)
	{
		accumulatedForce += force;
	}

	void Body::SetPosition(vmath::Vector2 pos)
	{
		position = pos;
	}

	void Body::SetVelocity(vmath::Vector2 vel)
	{
		velocity = vel;
	}

	bool Body::CheckCollision(const Body& otherBody) 
	{
		return CollisionManager::CheckCollision(*this, otherBody);
	};


//void Body::PublishResults()
//{
//	tx->Position = Position;
//}

//void Body::DebugDraw()
//{
//	if (IsStatic)
//	{
//		//White
//		Drawer::Instance.SetColor(Vec4(1, 1, 1, 1));

//		//Draw the shape of the object
//		BodyShape->Draw();
//	}
//	else
//	{
//		//Red
//		Drawer::Instance.SetColor(Vec4(1, 0, 0, 1));

//		//Draw the shape of the object
//		BodyShape->Draw();

//		//Draw the velocity of the object
//		Drawer::Instance.SetColor(Vec4(1, 1, 1, 1));
//		Drawer::Instance.MoveTo(Position);
//		Drawer::Instance.LineTo(Position + Velocity * 0.25f);
//	}

//}

//void Body::Initialize()
//{
//	//Get the transform to write results to
//	tx = GetOwner()->has(Transform);

//	//Get the starting position
//	Position = tx->Position;

//	//Add this body to the body list
//	PHYSICS->Bodies.push_back(this);

//	//If mass is zero object is interpreted
//	//to be static
//	if (Mass > 0.0f)
//	{
//		IsStatic = false;
//		InvMass = 1.0f / Mass;
//	}
//	else
//	{
//		IsStatic = true;
//		InvMass = 0.0f;
//	}

//	BodyShape->body = this;
//}

	void Body::Serialize(ISerializer& stream)
	{
		StreamRead(stream, Mass);
		StreamRead(stream, Friction);
		StreamRead(stream, Restitution);

		std::string shapeName;
		StreamRead(stream, shapeName);

		if (shapeName == "Circle")
		{
			ShapeCircle* shape = new ShapeCircle();
			StreamRead(stream, shape->Radius);
			this->BodyShape = shape;
		}

		if (shapeName == "Box")
		{
			ShapeAAB* shape = new ShapeAAB();
			StreamRead(stream, shape->Extents);
			this->BodyShape = shape;
		}

	}