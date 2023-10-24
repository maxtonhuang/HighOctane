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
//#include "GUIManager.h"
#include "CollisionResolution.h"
#include "Serialization.h"
#include "Animator.h"
#include "Scripting.h"
#include "Editing.h"
#include "ScriptEngine.h"
#include "Battle.h"
#include "UIComponents.h"


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

	//assumes that there is size component
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	//auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();

	for (Entity const& entity : m_Entities) {
		Size sizeData{ sizeArray.GetData(entity) };
		Transform transformData{ transformArray.GetData(entity) };
		transformArray.GetData(entity).halfDimensions = {
			sizeData.width / 2.f * transformData.scale, sizeData.height / 2.f * transformData.scale
		};

	}

	if (physics::PHYSICS->GetStepModeActive()) {
		for (Entity const& entity : m_Entities) {
			Transform* transData = &transformArray.GetData(entity);
			physics::PHYSICS->DebugDraw(*transData);
			transData->velocity *= .95f;
		}
		if (reqStep)
			for (Entity const& entity : m_Entities) {
				Transform* transData = &transformArray.GetData(entity);
				physics::PHYSICS->Integrate(*transData);
			}
	}
	else {
		for (Entity const& entity : m_Entities) {
			Transform* transData = &transformArray.GetData(entity);
			//Collider* collideData = &colliderArray.GetData(entity);

			//bodyData->velocity = transData->velocity;

			physics::PHYSICS->Integrate(*transData);
			physics::PHYSICS->DebugDraw(*transData);
		}
	}
	//Mail::mail().mailbox[ADDRESS::PHYSICS].clear();
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
	auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();
	

	//std::cout << m_Entities.size() << std::endl;
	for (Entity const& entity1 : m_Entities) {
		if (ECS::ecs().HasComponent<MainCharacter>(entity1)) {
			Transform* transData1 = &transformArray.GetData(entity1);
			Collider* collideData1 = &colliderArray.GetData(entity1);

			for (Entity const& entity2 : m_Entities) {
				if (entity1 != entity2) {
					Transform* transData2 = &transformArray.GetData(entity2);
					Collider* collideData2 = &colliderArray.GetData(entity2);

					bool collided{};
					if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_BOX) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_BOX)) {
						collided = physics::CheckCollisionBoxBox(*transData1, *transData2);
					}
					else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE)) {
						collided = physics::CheckCollisionCircleCircle(*transData1, *transData2);
					}
					else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_BOX)) {
						collided = physics::CheckCollisionCircleBox(*transData1, *transData2);
					}
					else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_BOX) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE)) {
						collided = physics::CheckCollisionBoxCircle(*transData1, *transData2);
					}
					else
						collided = false;
					
					if (collided == true) { physics::DynamicStaticResponse(*transData1); }
				}
			}
			transData1->velocity = { RESET_VEC2 };
		}
	}
	//Mail::mail().mailbox[ADDRESS::COLLISION].clear();
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
	auto& animatorArray = componentManager.GetComponentArrayRef<Animator>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Transform* transformData = &transformArray.GetData(entity);
		Model* modelData = &modelArray.GetData(entity);

		Animator* animatorData = &animatorArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);

		UpdateMovement(*transformData, *modelData);
		
		animatorData->UpdateAnimationMC(*texData, *sizeData);
		//modelData->DrawOutline();
		graphics.backgroundsystem.SetFocusEntity(entity);
	}
	//Mail::mail().mailbox[ADDRESS::MOVEMENT].clear();
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
	auto& animatorArray = componentManager.GetComponentArrayRef<Animator>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	//auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Animator* animatorData = &animatorArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		//Size* sizeData = &sizeArray.GetData(entity);

		animatorData->UpdateAnimation(*texData);
	}
	//Mail::mail().mailbox[ADDRESS::ANIMATOR].clear();
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
	auto& animatorArray = componentManager.GetComponentArrayRef<Animator>();

	for (Entity const& entity : m_Entities) {
		Model* m = &modelArray.GetData(entity);
		Size* size = &sizeArray.GetData(entity);
		Transform* transform = &transformArray.GetData(entity);
		Tex* tex = nullptr;
		Animator* anim = nullptr;
		if (m->CheckTransformUpdated(*transform, *size)) {
			m->Update(*transform, *size);
		}
		if (texArray.HasComponent(entity)) {
			tex = &texArray.GetData(entity);
		}
		if (animatorArray.HasComponent(entity)) {
			anim = &animatorArray.GetData(entity);
		}
		m->Draw(tex, anim);
		
	}
	camera.Update();
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


void ScriptSystem::Initialize() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	std::cout << "System.cpp::ScriptSystem::Initialize" << std::endl;
	std::cout << "System.cpp::ScriptSystem::Initialize::size: " << m_Entities.size() << std::endl;
	for (Entity const& entity : m_Entities) {
		Name* name = &nameArray.GetData(entity);
		ScriptEngine::OnCreateEntity(entity);
		std::cout << "System.cpp::ScriptSystem::Initialize: " << name->name << std::endl;
	}

}

// Scripting
void ScriptSystem::Update() {
	//// Access the ComponentManager through the ECS class
	//ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	//// Access component arrays through the ComponentManager
	//auto& screenArray = componentManager.GetComponentArrayRef<Screen>();
	//std::cout << "ScriptSystem's m_Entities Size(): " << m_Entities.size() << std::endl;


	//for (Entity const& entity : m_Entities) {
	//	Screen* s = &screenArray.GetData(entity);
	//	script.RunScript(s);
	//}
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	//std::cout << "System.cpp::ScriptSystem::Update::size: " << m_Entities.size() << std::endl;
	for (Entity const& entity : m_Entities) {
		Name* name = &nameArray.GetData(entity);
		ScriptEngine::OnUpdateEntity(entity);
		//std::cout << "System.cpp::ScriptSystem::Update: " << name->name << std::endl;
	}
	// scripts
	ScriptEngine::OnRuntimeStart();

	// Instantiate all script entities
}



void EditingSystem::Update() {

	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();

	for (Entity entity : m_Entities) {
		Name* n = &nameArray.GetData(entity);
		Transform* t = &transformArray.GetData(entity);
		Model* m = &modelArray.GetData(entity);
		
		// update position
		UpdateProperties(*n, *t, *m);

		if (n->selected) {
			m->DrawOutline();
		}
	}
	//Mail::mail().mailbox[ADDRESS::EDITING].clear();
}

void GameplaySystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	// Access component arrays through the ComponentManager
	auto& entityArray = componentManager.GetComponentArrayRef<Entity>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& animatorArray = componentManager.GetComponentArrayRef<Animator>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();

	for (Entity const& entity : m_Entities) {
		Model* m = &modelArray.GetData(entity);
		Transform* transform = &transformArray.GetData(entity);
		Tex* tex = &texArray.GetData(entity);
		Animator* anim = &animatorArray.GetData(entity);
	}
}

/******************************************************************************
*
*	@brief Battle System is located in Battle.cpp
*
******************************************************************************/



void UITextLabelSystem::Update() {
	//std::cout << "Hi this is UI update\n";
	//// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	//// Access component arrays through the ComponentManager
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	//auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();

	for (Entity const& entity : m_Entities) {
		Transform* transformData = &transformArray.GetData(entity);
		//Tex* texData = &texArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);
		Model* modelData = &modelArray.GetData(entity);
		Name* nameData = &nameArray.GetData(entity);
		TextLabel* textLabelData = &textLabelArray.GetData(entity);

		if (textLabelData->CheckStringUpdated(*textLabelData)) {
			textLabelData->UpdateOffset(textLabelData->relTransform, *transformData);
		}

		textLabelData->IsClickedOrHovered(*transformData, *modelData, *nameData);
		if (nameData->selected) {
			textLabelData->OnFocus();
		}
		
		sizeData->width = textLabelData->posOffset.x;
		sizeData->height = textLabelData->posOffset.y;
		
		//DEBUG_PRINT("SIZE: %f %f", sizeData->width, sizeData->height);
		//DEBUG_PRINT("relTrans: %f %f", textLabelData->relTransform.x, textLabelData->relTransform.y);
		//DEBUG_PRINT("SCALE: %f", transformData->scale);
		//DEBUG_PRINT("MIN %f %f", modelData->GetMin().x, modelData->GetMin().y);
		//DEBUG_PRINT("MAX %f %f", modelData->GetMax().x, modelData->GetMax().y);

		
		//call graphics drawLabel here?
		modelData->SetAlpha(1.f);
		graphics.DrawLabel(*textLabelData, textLabelData->relTransform, modelData->GetColor());
		modelData->SetAlpha(0.2f);

		//note: find a way to update size!!
	}
}

//void UIButtonSystem::Update() {
//	std::cout << "Hi this is UI update\n";
//}