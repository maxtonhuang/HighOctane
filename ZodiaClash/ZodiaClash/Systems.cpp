/******************************************************************************
* 
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
* 
* *****************************************************************************
* 
*	@file		Systems.cpp
* 
*	@author		Maxton Huang Xinghua
* 
*	@email		m.huang\@digipen.edu
* 
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
* 
*	@section	Section A
* 
*	@date		22 September 2023
* 
* *****************************************************************************
* 
*	@brief		Systems running on underlying ECS Architecture
*
*	This file contains the system functions running on the underlying ECS
*	Architecture. It will call other functions in their respective source
*	files, and pass the required Entity information to them.
* 
******************************************************************************/


#include "ECS.h"
#include "Components.h"
#include "Movement.h"
#include "graphics.h"
#include "model.h"
#include "message.h"
#include "physics.h"
#include "MultiThreading.h"
#include "Global.h"
#include "GUIManager.h"
#include "CollisionResolution.h"
#include "Serialization.h"


/******************************************************************************
*
*	@brief Handles the physics-related updates of entities
*
*	This function handles:
*   1. Reading and processing physics-related mailbox messages.
*   2. Updating physics attributes based on entity size.
*   3. Debug drawing and physics calculations based on stepping mode.
*   4. Clearing the mailbox after processing.
*
******************************************************************************/
void PhysicsSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	bool reqStep{ false };
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::PHYSICS]) {
		switch (msg.type) {
		case TYPE::KEY_TRIGGERED:
			if (msg.info == INFO::KEY_8) {
				reqStep = true;
			}
			if (msg.info == INFO::KEY_9) {
				physics::PHYSICS->ToggleStepMode();
			}
			if (msg.info == INFO::KEY_0) {
				physics::PHYSICS->ToggleDebugMode();
			}
			break;
		}
	}

	//std::cout << m_Entities.size() << std::endl;
	// Access component arrays through the ComponentManager
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& bodyArray = componentManager.GetComponentArrayRef<physics::Body>();
	
	//assumes that there is size component
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	//auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();

	for (Entity const& entity : m_Entities) {
		physics::Body bodyData{ bodyArray.GetData(entity) };
		Size sizeData{ sizeArray.GetData(entity) };
		Transform transformData{ transformArray.GetData(entity) };
		bodyArray.GetData(entity).halfDimensions = {
			sizeData.width / 2.f * transformData.scale.x, sizeData.height / 2.f * transformData.scale.y
		};

	}

	if (physics::PHYSICS->GetStepModeActive()) {
		for (Entity const& entity : m_Entities) {
			Transform* transData = &transformArray.GetData(entity);
			physics::Body* bodyData = &bodyArray.GetData(entity);
			physics::PHYSICS->DebugDraw(*bodyData, *transData);
			transData->velocity *= .95f;
		}
		if (reqStep)
			for (Entity const& entity : m_Entities) {
				Transform* transData = &transformArray.GetData(entity);
				physics::Body* bodyData = &bodyArray.GetData(entity);
				physics::PHYSICS->Integrate(*bodyData, *transData);
			}
	}
	else {
		for (Entity const& entity : m_Entities) {
			Transform* transData = &transformArray.GetData(entity);
			physics::Body* bodyData = &bodyArray.GetData(entity);
			//Collider* collideData = &colliderArray.GetData(entity);

			//bodyData->velocity = transData->velocity;

			physics::PHYSICS->Integrate(*bodyData, *transData);
			physics::PHYSICS->DebugDraw(*bodyData, *transData);
		}
	}
	Mail::mail().mailbox[ADDRESS::PHYSICS].clear();
}

/******************************************************************************
*
*	@brief Checks and handles collisions between entities.
*
*	In this function:
*	1. It fetches required components (like Transform and Body).
*	2. Checks for collisions involving the `MainCharacter`.
*	3. Updates entities' positions and velocities after collisions.
*	4. Clears the collision mailbox.
*
******************************************************************************/
void CollisionSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	
	// Access component arrays through the ComponentManager
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& bodyArray = componentManager.GetComponentArrayRef<physics::Body>();
	//auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();
	

	//std::cout << m_Entities.size() << std::endl;
	for (Entity const& entity1 : m_Entities) {
		if (ECS::ecs().HasComponent<MainCharacter>(entity1)) {
			Transform* transData1 = &transformArray.GetData(entity1);
			physics::Body* bodyData1 = &bodyArray.GetData(entity1);
			//Collider* collideData1 = &colliderArray.GetData(entity1);

			for (Entity const& entity2 : m_Entities) {
				if (entity1 != entity2) {
					Transform* transData2 = &transformArray.GetData(entity2);
					physics::Body* bodyData2 = &bodyArray.GetData(entity2);
					//Collider* collideData2 = &colliderArray.GetData(entity2);

					bool collided{};
					collided = physics::COLLISION->CheckBodyCollision(*bodyData1, *bodyData2);
					if (collided) {
						physics::HandleCollisionResponse(*bodyData1, *bodyData2);
						
					}
					transData2->velocity = bodyData2->velocity;
					transData2->position = bodyData2->position;
				}
			}
			bodyData1->velocity = { 0.f, 0.f };
			transData1->velocity = bodyData1->velocity;
			transData1->position = bodyData1->position;
		}
	}
	Mail::mail().mailbox[ADDRESS::COLLISION].clear();
}

/******************************************************************************
*
*	@brief Handles movement of the main character
*
*	Calculates the velocity according to key presses and stores it for the
*	Physics System to handle the update of the position.
*
******************************************************************************/
// For MainCharacter only
void MovementSystem::Update() {
	//std::cout << "MovementSystem's m_Entities Size(): " << m_Entities.size() <<std::endl;

	//// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	//// Access component arrays through the ComponentManager
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& animationArray = componentManager.GetComponentArrayRef<Animation>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Transform* transformData = &transformArray.GetData(entity);
		Model* modelData = &modelArray.GetData(entity);
		Animation* aniData = &animationArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);

		UpdateMovement(*transformData);
		
		modelData->UpdateAnimationMC(*aniData, *texData, *sizeData);
		//modelData->DrawOutline();
	}
	Mail::mail().mailbox[ADDRESS::MOVEMENT].clear();
}

/******************************************************************************
*
*	@brief Handles animation for each entity, if any
*
*	Handles the animation for each entity, such as sprite cycling, if an entity
*	has any animation.
*
******************************************************************************/
void ModelSystem::Update() {

	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& animationArray = componentManager.GetComponentArrayRef<Animation>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	//auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Model* modelData = &modelArray.GetData(entity);
		Animation* aniData = &animationArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		//Size* sizeData = &sizeArray.GetData(entity);

		modelData->UpdateAnimation(*aniData, *texData);
	}
	Mail::mail().mailbox[ADDRESS::MODEL].clear();
}

/******************************************************************************
*
*	@brief Initializes Graphics System
*
*	-
*
******************************************************************************/
void GraphicsSystem::Initialize() {
	for (Entity const& entity : m_Entities) {
		Model* m = &ECS::ecs().GetComponent<Model>(entity);
		m->Update(ECS::ecs().GetComponent<Transform>(entity), ECS::ecs().GetComponent<Size>(entity));
	}
}

/******************************************************************************
*
*	@brief Sends Entity data to Graphics Manager
*
*	Sends the corresponding Entity data to the Graphics Manager (Graphics.cpp)
*	for rendering and display on screen.
*
******************************************************************************/
void GraphicsSystem::Update() {
	//std::cout << "GraphicsSystem's m_Entities Size(): " << m_Entities.size() << std::endl;
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& animationArray = componentManager.GetComponentArrayRef<Animation>();

	for (Entity const& entity : m_Entities) {
		Model* m = &modelArray.GetData(entity);
		Size* size = &sizeArray.GetData(entity);
		Transform* transform = &transformArray.GetData(entity);
		Tex* tex = &texArray.GetData(entity);
		Animation* anim = &animationArray.GetData(entity);
		if (m->CheckTransformUpdated(*transform, *size)) {
			m->Update(*transform, *size);
		}
		m->Draw(*tex, *anim);
	}
	graphics.Draw();
}

/******************************************************************************
*
*	@brief Sends Entity array data to Serializer
*
*	Sends the corresponding Entity array data to the Serializer
*	(Serialization.cpp) for saving into	file.
*
******************************************************************************/
void SerializationSystem::Update() {
	
	Serializer::SaveEntityToJson("../Assets/Scenes/SceneEntities.json", m_Entities);
	
}
