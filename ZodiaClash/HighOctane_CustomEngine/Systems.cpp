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
#include "CollisionResolution.h"
#include "Serialization.h"
#include "Animator.h"
#include "Scripting.h"
#include "Editing.h"
#include "ScriptEngine.h"
#include "Battle.h"
#include "EntityFactory.h"
#include "EngineCore.h"
#include "CharacterStats.h"
#include "UIComponents.h"
#include "AssetManager.h"
#include "Layering.h"

#define FIXED_DT 1.0f/60.f
#define MAX_ACCUMULATED_TIME 5.f // to avoid the "spiral of death" if the system cannot keep up

// Extern for the vector to contain the full name for ImGui for scripting system
extern std::vector<std::string> fullNameVecImGUI;

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

	//process mesaage here
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
	Mail::mail().mailbox[ADDRESS::PHYSICS].clear(); // Clear the mailbox after processing.

		// Access component arrays through the ComponentManager
		// Access the ComponentManager through the ECS class
		ComponentManager& componentManager = ECS::ecs().GetComponentManager();
		auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
		auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

		//update entity half-dimensions
		for (Entity const& entity : m_Entities) {
			Size sizeData{ sizeArray.GetData(entity) };
			Transform transformData{ transformArray.GetData(entity) };
			transformArray.GetData(entity).halfDimensions = {
				sizeData.width / 2.f * transformData.scale, sizeData.height / 2.f * transformData.scale
			};
		}

		// Check step mode and integrate physics
		if (physics::PHYSICS->GetStepModeActive()) {
			// Debug draw all entities
			// If step is required, integrate physics for all entities
			if (reqStep) {
				for (Entity const& entity : m_Entities) {
					Transform& transData = transformArray.GetData(entity);
					physics::PHYSICS->Integrate(transData);
				}
			}
		}
		else {
			// Regular physics integration and debug drawing
			for (Entity const& entity : m_Entities) {
				Transform& transData = transformArray.GetData(entity);
				physics::PHYSICS->Integrate(transData);
			}
		}
}

void PhysicsSystem::Draw() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();

	for (Entity const& entity : m_Entities) {
		Transform& transData = transformArray.GetData(entity);
		physics::PHYSICS->DebugDraw(transData);
	}
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

		for (Entity const& entity1 : m_Entities) {
			if (ECS::ecs().HasComponent<MainCharacter>(entity1)) {
				Transform* transData1 = &transformArray.GetData(entity1);
				Collider* collideData1 = &colliderArray.GetData(entity1);

				bool hasCollided = false;

				for (Entity const& entity2 : m_Entities) {
					if (entity1 != entity2) {
						Transform* transData2 = &transformArray.GetData(entity2);
						Collider* collideData2 = &colliderArray.GetData(entity2);

						if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_BOX) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_BOX)) {
							hasCollided = physics::CheckCollisionBoxBox(*transData1, *transData2);
						}
						else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE)) {
							hasCollided = physics::CheckCollisionCircleCircle(*transData1, *transData2);
						}
						else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_BOX)) {
							hasCollided = physics::CheckCollisionCircleBox(*transData1, *transData2);
						}
						else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_BOX) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE)) {
							hasCollided = physics::CheckCollisionBoxCircle(*transData1, *transData2);
						}
						if (hasCollided == true) 
						{ 
							physics::DynamicStaticResponse(*transData1);
							break;
						}
					}
				}
				// Update the character's position if no collision occurred
				if (!hasCollided) {
					physics::PHYSICS->Integrate(*transData1);
				}
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

	if (!inEditing || viewportWindowHovered) {

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
			camera.SetPos(-transformData->position.x, -transformData->position.y);
		}
	}
}

/******************************************************************************
*
*	@brief Handles animation for each entity, if any
*
*	Handles the animation for each entity, such as sprite cycling, if an entity
*	has any animation.
*
******************************************************************************/
void AnimatorSystem::Update() {

	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& animatorArray = componentManager.GetComponentArrayRef<Animator>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	//auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Animator* animatorData = &animatorArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		animatorData->UpdateAnimation(*texData);
	}
}


/******************************************************************************
*
*	@brief Handles model data for each entity, if any
*
*	Handles ...
*
******************************************************************************/
void ModelSystem::Update() {
	// Empty
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
		if (ECS::ecs().HasComponent<TextLabel>(entity)) {
			m->type = UI;
		}
		m->Update(ECS::ecs().GetComponent<Transform>(entity), ECS::ecs().GetComponent<Size>(entity));
	}
	camera.SetPos(0.f, 0.f);
	camera.SetZoom(1.f);
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
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Model* m = &modelArray.GetData(entity);
		Size* size = &sizeArray.GetData(entity);
		Transform* transform = &transformArray.GetData(entity);
		/*if (m->CheckTransformUpdated(*transform, *size)) {
			
		}*/
		m->Update(*transform, *size);
	}

	//UPDATE FREE CAMERA MOVEMENT
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
		if (msg.type == TYPE::KEY_DOWN) {
			switch (msg.info) {
			case INFO::KEY_Y:   camera.AddZoom(0.1f * FIXED_DT);        break;
			case INFO::KEY_U:   camera.AddZoom(-0.1f * FIXED_DT);       break;
			case INFO::KEY_I:   camera.AddPos(0.f, 200.f * FIXED_DT);   break;
			case INFO::KEY_J:   camera.AddPos(-200.f * FIXED_DT, 0.f);  break;
			case INFO::KEY_K:   camera.AddPos(0, -200.f * FIXED_DT);    break;
			case INFO::KEY_L:   camera.AddPos(200.f * FIXED_DT, 0.f);   break;
			default: break;
			}
		}
	}
	camera.Update();
}

void GraphicsSystem::Draw() {
// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& animatorArray = componentManager.GetComponentArrayRef<Animator>();
	auto& textlabelArray = componentManager.GetComponentArrayRef<TextLabel>();

	graphics.viewport.Unuse();
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] || !edit_mode) {
			for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
				Entity entity = layering[layer_it][entity_it];
				if (entitiesToSkip[entity] || !edit_mode) {
					Tex* tex{};
					Animator* anim{};
					Model* m{};
					if (modelArray.HasComponent(entity)) {
						m = &modelArray.GetData(entity);
						if (texArray.HasComponent(entity)) {
							tex = &texArray.GetData(entity);
						}
						if (animatorArray.HasComponent(entity)) {
							anim = &animatorArray.GetData(entity);
						}
						m->Draw(tex, anim);
						if (textlabelArray.HasComponent(entity)) {
							TextLabel* textLabelData = &textlabelArray.GetData(entity);
							graphics.DrawLabel(*textLabelData, textLabelData->relTransform, textLabelData->textColor);
						}

					}
				}
			}
		}
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
	if (saveFile) {
		PrepareLayeringForSerialization();
		//EmbedSkipLockForSerialization();
		std::string scenePath{ SaveFileDialog("*.scn","Scene File") };
		if (scenePath != "") {
			std::ofstream sceneFile{ scenePath.c_str() };

			std::string jsonPath{ scenePath.substr(0,scenePath.find(".scn")) + ".json" };
			Serializer::SaveEntityToJson(jsonPath, m_Entities);

			auto files = assetmanager.GetFiles();
			for (auto& f : files) {
				sceneFile << f << "\n";
			}
			sceneFile << jsonPath.substr(jsonPath.find_last_of("\\") + 1);
			sceneFile.close();
		}
		saveFile = false;
	}
	
	if (destroyAll) {
		std::vector<Entity> entitylist{};
		EntityFactory::entityFactory().masterEntitiesList.clear();
		for (Entity e : m_Entities) {
			entitylist.push_back(e);
		}
		for (Entity e : entitylist) {
			//ECS::ecs().DestroyEntity(e);
			EntityFactory::entityFactory().DeleteCloneModel(e);
		}
		destroyAll = false;
	}

	if (newScene) {
		assetmanager.UnloadAll();
		if (newSceneName != "") {
			assetmanager.LoadAssets(newSceneName);
		}
		initLevel = true;
		newScene = false;
	}

	if (playButton) {

		std::string scenePath{ assetmanager.GetDefaultPath() + "Scenes/tmp.scn"};
		if (scenePath != "") {
			std::ofstream sceneFile{ scenePath.c_str() };

			std::string jsonPath{ assetmanager.GetDefaultPath() + "Scenes/tmp.json" };
			Serializer::SaveEntityToJson(jsonPath, m_Entities);

			auto files = assetmanager.GetFiles();
			for (auto& f : files) {
				sceneFile << f << "\n";
			}
			sceneFile << "tmp.json";
			sceneFile.close();
		}

		initLevel = true;
		playButton = false;
	}

	
}


/******************************************************************************
*
*	@brief Initialies the Script System
* 
*   This function initializes the Script System by calling the ScriptEngine's
*   ScriptInit function for each entity with a script component.
*
******************************************************************************/
bool FirstInitScriptSystem = true;
void ScriptSystem::Initialize() {
	std::unordered_map<Entity, std::vector<std::string>> scriptMap;

	// Iterate through all entities with a script component
	for (const Entity& entity : m_Entities) {

		// Get the script component
		Script* s = &ECS::ecs().GetComponent<Script>(entity);
		if (!s) {
			// If there's no script associated with the entity, just continue to the next iteration.
			continue;
		}

		// Only allows the scrpit system to init once
		if (FirstInitScriptSystem) {
			ScriptEngine::ScriptInit(entity);
			FirstInitScriptSystem = false;
		}
	}
}


/******************************************************************************
*
*   @brief Updates the Script System
* 
*   This function updates the Script System by calling the ScriptEngine's
*   ScriptUpdate function for each entity with a script component.
* 
******************************************************************************/

void ScriptSystem::Update() {

	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& scriptArray = componentManager.GetComponentArrayRef<Script>();

	for (Entity const& entity : m_Entities) {

		Script* scriptData = &scriptArray.GetData(entity);

		for (size_t i = 0; i < scriptData->scriptNameVec.size(); ++i) {
			ScriptEngine::ScriptUpdate(entity);
		}

	}

}


/******************************************************************************
*
*	@brief Update Function for the Editing System
*
*	Sends the corresponding Entity data to the Editing System (Editing.cpp)
*
******************************************************************************/
void EditingSystem::Update() {
	anyObjectSelected = false;
	selectedEntities.clear();
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();

	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (Entity & entity : layering[layer_it]) {
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)]) {
					Name* n = &nameArray.GetData(entity);
					Transform* t = &transformArray.GetData(entity);
					Model* m = &modelArray.GetData(entity);

					// edit entity's properties
					UpdateProperties(entity, *n, *t, *m, layer_it);
				}
			}
		}
	}

	selectedEntities.clear();
	for (Entity entity : m_Entities) {
		if (nameArray.GetData(entity).selected) {
			anyObjectSelected = true;
			selectedEntities.emplace_back(entity);
		}
	}

	if (toCopy || toDestroy) {
		for (Entity entity : selectedEntities) {
			printf("%d ", static_cast<int>(entity));
		}
	}

	if (toCopy) {
		for (Entity entity : selectedEntities) {
			EntityFactory::entityFactory().CloneMaster(entity);
		}
		toCopy = false;
		selectedEntities.clear();
		anyObjectSelected = false;
		UnselectAll();
	}


	if (toDestroy) {
		for (Entity entity : selectedEntities) {
			EntityFactory::entityFactory().DeleteCloneModel(entity);
			RemoveEntityFromLayering(entity);
		}
		toDestroy = false;
		selectedEntities.clear();
		UnselectAll();
		anyObjectSelected = false;
		currentLayer = selectedLayer = std::numeric_limits<size_t>::max();
	}
}


/******************************************************************************
*
*	@brief Draws the green outline for selected entities
*
******************************************************************************/
void EditingSystem::Draw() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();

	for (Entity entity : m_Entities) {
		Name* n = &nameArray.GetData(entity);
		Model* m = &modelArray.GetData(entity);

		if (n->selected) {
			m->DrawOutline();
		}
	}
}

/******************************************************************************
*
*	@brief Battle System is located in Battle.cpp
*
******************************************************************************/



void UITextLabelSystem::Update() {
	//// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	//// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& buttonArray = componentManager.GetComponentArrayRef<Button>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();

	for (Entity const& entity : m_Entities) {
		Model* modelData = &modelArray.GetData(entity);
		Name* nameData = &nameArray.GetData(entity);
		TextLabel* textLabelData = &textLabelArray.GetData(entity);
		Transform* transformData = &transformArray.GetData(entity);

		//if entity has button component, state handling managed by button
		if (!buttonArray.HasComponent(entity)) {
			textLabelData->Update(*modelData, *nameData);
		}

		textLabelData->UpdateOffset(*transformData);

		//note: find a way to update size!!
	}
}

void UITextLabelSystem::Draw() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	//// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& buttonArray = componentManager.GetComponentArrayRef<Button>();
	
	for (Entity const& entity : m_Entities) {
		Model* modelData = &modelArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);
		Tex* texData = nullptr;
		TextLabel* textLabelData = &textLabelArray.GetData(entity);
		Button* buttonData = nullptr;
		
		
		//if entity has button component, drawing managed by button
		if (buttonArray.HasComponent(entity)) {
			buttonData = &buttonArray.GetData(entity);
		}
		else {
			sizeData->width = textLabelData->textWidth;
			sizeData->height = textLabelData->textHeight;
		}

		if (texArray.HasComponent(entity)) {
			texData = &texArray.GetData(entity);
		}

		if (!buttonData && !texData) {
			if (edit_mode) {
				(textLabelData->currentState == STATE::NONE) ? modelData->SetAlpha(0.0f) : modelData->SetAlpha(0.2f);
			}
			else {
				modelData->SetAlpha(0.0f);
			}
		}
		else if (!buttonData && !texData) {
			modelData->SetAlpha(0.0f);
		}
	}
	
}

void UIButtonSystem::Update() {
	//// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	//// Access component arrays through the ComponentManager
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& buttonArray = componentManager.GetComponentArrayRef<Button>();

	for (Entity const& entity : m_Entities) {
		Size* sizeData = &sizeArray.GetData(entity);
		Name* nameData = &nameArray.GetData(entity);
		Model* modelData = &modelArray.GetData(entity);
		TextLabel* textLabelData = &textLabelArray.GetData(entity);
		Button* buttonData = &buttonArray.GetData(entity);

		buttonData->Update(*modelData, *nameData, *textLabelData);

		if (!texArray.HasComponent(entity)) {
			glm::vec4 btnColor = (edit_mode) ? buttonData->GetDefaultButtonColor() : buttonData->GetButtonColor();
			modelData->SetColor(btnColor.r, btnColor.g, btnColor.b);
		}

		sizeData->width = buttonData->buttonWidth;
		sizeData->height = buttonData->buttonHeight;
	}
}

