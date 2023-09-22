#include "ECS.h"
#include "Components.h"
#include "Movement.h"
#include "graphics.h"
#include "model.h"
//#include <future>


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
				UpdateMovement(ecs.GetComponent<Transform>(entity));
		}
	}

	void ModelSystem::Update() {
		for (Entity const& entity : m_Entities) {
			Animation* animate = &ecs.GetComponent<Animation>(entity);
			//update animation
			switch (animate->animationType) {
			case(Animation::ANIMATION_NONE):
				break;
			case(Animation::ANIMATION_TIME_BASED):
				ecs.GetComponent<Model>(entity).AnimateOnInterval(*animate, ecs.GetComponent<Tex>(entity));
				break;
			case(Animation::ANIMATION_EVENT_BASED):
				ecs.GetComponent<Model>(entity).AnimateOnKeyPress(*animate, ecs.GetComponent<Tex>(entity));
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
		//std::vector<std::future<void>> asyncResults;
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


			m.Update(transformArray.GetData(entity), sizeArray.GetData(entity));
			
			//auto future = std::async(std::launch::async, [&entity, &m, &texArray, &animationArray]() {
				m.Draw(texArray.GetData(entity), animationArray.GetData(entity));
			//	});

			//asyncResults.emplace_back(std::move(future));
		}

		//for (auto& future : asyncResults) {
		//	future.wait();
		//}

		graphics.Draw();
	}


