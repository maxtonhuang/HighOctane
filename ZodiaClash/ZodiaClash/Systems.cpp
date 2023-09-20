#include "ECS.h"
#include "Components.h"
#include "Movement.h"


namespace Architecture {

	extern ECS ecs;
	
	void PhysicsSystem::Update() {
		//int i = 0;
		//for (Entity const & entity1 : m_Entities) { // this m_Entities belongs to the PhysicsSystem only.
		//	for (Entity const& entity2 : m_Entities) {
		//		if (entity1 == entity2) {
		//			continue;
		//		}
		//		else {
		//			BoundingBox bbox1 = ecs.GetComponent<BoundingBox>(entity1);
		//			BoundingBox bbox2 = ecs.GetComponent<BoundingBox>(entity2);
		//			Transform trans1 = ecs.GetComponent<Transform>(entity1);

		//			//Collision::CheckCollision(bbox1, bbox2, trans1, trans2);//check collision...
		//		}
		//	}
		//	//Vel movement = ecs.GetComponent<Vel>(entity);
		//	//Transform transform = ecs.GetComponent<Transform>(entity);
		//
		//	//UpdateMovement(movement);
		//	//UpdateModel(transform, movement);
		//	PhysicsManager ... (transform);
		//
		//}
	}
	
	// Movement System
	void MovementSystem::Update() {
		for (Entity const& entity : m_Entities) {
			MainCharacter mc = ecs.GetComponent<MainCharacter>(entity);
			if (mc.isMainCharacter) {
				UpdateMovement(ecs.GetComponent<Transform>(entity));
			}
		}
	}

	void ModelSystem::Update() {

	}


}

