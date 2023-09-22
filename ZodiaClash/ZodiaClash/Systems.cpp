#include "ECS.h"
#include "Components.h"
#include "Movement.h"
#include "graphics.h"
#include "model.h"
#include "message.h"


	extern ECS ecs;

	extern Mail mail;
	
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
		//std::cout << "MovementSystem's m_Entities Size(): " << m_Entities.size() << std::endl;
		for (Entity const& entity : m_Entities) {
				UpdateMovement(ecs.GetComponent<Transform>(entity));
		}
		mail.mailbox[ADDRESS::MOVEMENT].clear();
	}

	void ModelSystem::Update() {
		// Access the ComponentManager through the ECS class
		ComponentManager& componentManager = ecs.GetComponentManager();
		// Access component arrays through the ComponentManager
		auto& modelArray = componentManager.GetComponentArrayRef<Model>();
		auto& animationArray = componentManager.GetComponentArrayRef<Animation>();
		auto& texArray = componentManager.GetComponentArrayRef<Tex>();

		for (Entity const& entity : m_Entities) {
			Animation* animate = &animationArray.GetData(entity);
			//update animation
			switch (animate->animationType) {
			case(Animation::ANIMATION_NONE):
				break;
			case(Animation::ANIMATION_TIME_BASED):
				modelArray.GetData(entity).AnimateOnInterval(*animate, texArray.GetData(entity));
				break;
			case(Animation::ANIMATION_EVENT_BASED):
				modelArray.GetData(entity).AnimateOnKeyPress(*animate, texArray.GetData(entity));
				break;
			default:
				break;
			}
		}
	}

	void GraphicsSystem::Initialize() {
		for (Entity const& entity : m_Entities) {
			//MainCharacter mc = ecs.GetComponent<MainCharacter>(entity);
			//if (mc.isMainCharacter) {


				//UpdateMovement(ecs.GetComponent<Transform>(entity));
			//}
			Model* m = &ecs.GetComponent<Model>(entity);
			m->Update(ecs.GetComponent<Transform>(entity), ecs.GetComponent<Size>(entity));
		}
	}

	void GraphicsSystem::Update() {
		//std::cout << "GraphicsSystem's m_Entities Size(): " << m_Entities.size() << std::endl;
		
		// Access the ComponentManager through the ECS class
		ComponentManager& componentManager = ecs.GetComponentManager();

		// Access component arrays through the ComponentManager
		auto& modelArray = componentManager.GetComponentArrayRef<Model>();
		auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
		auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
		auto& texArray = componentManager.GetComponentArrayRef<Tex>();
		auto& animationArray = componentManager.GetComponentArrayRef<Animation>();

		for (Entity const& entity : m_Entities) {
			Model m = modelArray.GetData(entity);
			m.Update(transformArray.GetData(entity), sizeArray.GetData(entity));	// ecs.GetComponent<Transform>(entity), ecs.GetComponent<Size>(entity));
			m.Draw(texArray.GetData(entity), animationArray.GetData(entity));	// ecs.GetComponent<Tex>(entity), ecs.GetComponent<Animation>(entity));
		}
		graphics.Draw();
	}


