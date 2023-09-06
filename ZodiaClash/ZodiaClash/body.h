//#include "vmath.h"
//#pragma once
//
/////Body Component provides basic point physics dynamics including mass, 
/////velocity, forces, acceleration, and collision resolution.
/////Component will modify transform component attributes every frame.
//
//namespace physics {
//	class Body {
//	public:
//		Body(); //constructor
//		~Body(); //destructor
//
//		void AddForce(vmath::Vector2 force);
//		void Integrate(float dt);
//		void SetPosition(vmath::Vector2);
//		void SetVelocity(vmath::Vector2);
//		bool CheckCollision(const Body& otherBody);// check collision with another body
//		void PublishResults();
//		virtual void Initialize();
//		virtual void Serialize(ISerializer& stream);
//		//Draw the object using the debug drawer
//		void DebugDraw();
//
//		Body* next;
//		Body* prev;
//		vmath::Vector2 position, prevPosition, velocity{ 0,0 }, acceleration{ 0,0 }, accumulatedForce;
//		float mass, restitution, friction{ 0.2f };
//
//		//Transform for this body
//		Transform* tx;
//		//Shape used for collision with this body
//		Shape* bodyShape;
//		// Indicates if the body is movable
//		bool isStatic;
//	};
//}
