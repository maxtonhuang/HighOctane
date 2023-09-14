#include "body.h"
#include "physics.h"

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

	Body::~Body()
	{
		/*delete bodyShape;
		PHYSICS->bodies.erase(this);*/
		//auto it = std::find(PHYSICS->bodies.begin(), PHYSICS->bodies.end(), someBodyObject);
		//// if (it != PHYSICS->bodies.end()) {
		////     PHYSICS->bodies.erase(it);
	}

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

	/*void Body::Serialize(ISerializer& stream)
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

	}*/