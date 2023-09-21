#include "ECS.h"
#include "Components.h"
#include "Movement.h"
#include "graphics.h"
#include "model.h"


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
		// std::cout << "MovementSystem's m_Entities Size(): " << m_Entities.size() << std::endl;
		for (Entity const& entity : m_Entities) {
			MainCharacter mc = ecs.GetComponent<MainCharacter>(entity);
			if (mc.isMainCharacter) {
				UpdateMovement(ecs.GetComponent<Transform>(entity));
			}
		}
	}

	void ModelSystem::Update() {
		for (Entity const& entity : m_Entities) {
			Animation animate = ecs.GetComponent<Animation>(entity);
			//update animation
			switch (animate.animationType) {
			case(Animation::ANIMATION_NONE):
				break;
			case(Animation::ANIMATION_TIME_BASED):
				ecs.GetComponent<Model>(entity).AnimateOnInterval(animate);
				break;
			case(Animation::ANIMATION_EVENT_BASED):
				ecs.GetComponent<Model>(entity).AnimateOnKeyPress(animate);
				break;
			default:
				break;
			}
		}
	}

	void GraphicsSystem::Update() {
		// std::cout << "GraphicsSystem's m_Entities Size(): " << m_Entities.size() << std::endl;
		for (Entity const& entity : m_Entities) {
			MainCharacter mc = ecs.GetComponent<MainCharacter>(entity);
			if (mc.isMainCharacter) {
			    
				
				//UpdateMovement(ecs.GetComponent<Transform>(entity));
			}
			Model m = ecs.GetComponent<Model>(entity);
			m.Update(entity);
			m.Draw(entity);
			//ecs.GetComponent<Model>(entity).Update(entity);
			//ecs.GetComponent<Model>(entity).Draw(entity);
		}
		graphics.Draw();
	}


