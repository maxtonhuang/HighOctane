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

	
void PhysicsSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::PHYSICS]) {
		switch (msg.type) {
		case TYPE::KEY_TRIGGERED:
			if (msg.info == INFO::KEY_G) {
				Entity entity = ECS::ecs().CreateEntity();
				ECS::ecs().AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
				//ECS::ecs().AddComponent(entity, Transform{ glm::vec2{(rand_width(rng) - graphics.GetWidth() / 2), (rand_height(rng) - graphics.GetHeight() / 2)}, 0.f, glm::vec2{1, 1} });
				ECS::ecs().AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, Vec2{ 1.f, 1.f }, vmath::Vector2{ 0,0 } });
				ECS::ecs().AddComponent(entity, Visible{ true });
				//ECS::ecs().AddComponent(entity, Tex{ texList.Add("cat.png") });

				//add tex component, init tex with duck sprite (init tex with nullptr produces white square instead)
				ECS::ecs().AddComponent(entity, Tex{ texList.Add("duck.png") });
				Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
				t->texVariants.push_back(texList.Add("duck.png"));
				t->texVariants.push_back(texList.Add("duck2.png"));
				//setting tex to texVariants[1] (duck2) still shows duck tex but with duck2 dims?
				t->tex = t->texVariants.at(0);
				ECS::ecs().AddComponent(entity, Animation{});
				Animation* a = &ECS::ecs().GetComponent<Animation>(entity);
				a->animationType = Animation::ANIMATION_TIME_BASED;
				//a->animationType = Animation::ANIMATION_EVENT_BASED;
				a->frameDisplayDuration = 0.1f;
				ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
				ECS::ecs().AddComponent(entity, Model{});
				ECS::ecs().AddComponent(entity, Clone{});
				//ECS::ecs().AddComponent(entity, MainCharacter{});

				//add physics component
				ECS::ecs().AddComponent<physics::Body>(entity, physics::Body{});
				ECS::ecs().AddComponent<Collider>(entity, Collider{});
			}
			break;
		}

	}
	// Access component arrays through the ComponentManager
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& bodyArray = componentManager.GetComponentArrayRef<physics::Body>();
	auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();

	for (Entity const& entity : m_Entities) {
		Transform* transData = &transformArray.GetData(entity);
		physics::Body* bodyData = &bodyArray.GetData(entity);
		Collider* collideData = &colliderArray.GetData(entity);
		
		physics::PHYSICS->Integrate(*bodyData, g_dt, *transData);
	}
	Mail::mail().mailbox[ADDRESS::PHYSICS].clear();
}
	
// Movement System
// For MainCharacter
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

		//UpdateMovement(ECS::ecs().GetComponent<Transform>(entity));
		UpdateMovement(*transformData);

		//Model* modelData = &ECS::ecs().GetComponent<Model>(entity);
		//Animation* aniData = &ECS::ecs().GetComponent<Animation>(entity);
		//Tex* texData = &ECS::ecs().GetComponent<Tex>(entity);
		//Size* sizeData = &ECS::ecs().GetComponent<Size>(entity);

		modelData->UpdateAnimationMC(*aniData, *texData, *sizeData);
		modelData->DrawOutline();
	}
	Mail::mail().mailbox[ADDRESS::MOVEMENT].clear();
}

void ModelSystem::Update() {

	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& animationArray = componentManager.GetComponentArrayRef<Animation>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Model* modelData = &modelArray.GetData(entity);
		Animation* aniData = &animationArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);

		modelData->UpdateAnimation(*aniData, *texData, *sizeData);
	}
	Mail::mail().mailbox[ADDRESS::MODEL].clear();
}

void GraphicsSystem::Initialize() {
	for (Entity const& entity : m_Entities) {
		Model* m = &ECS::ecs().GetComponent<Model>(entity);
		m->Update(ECS::ecs().GetComponent<Transform>(entity), ECS::ecs().GetComponent<Size>(entity));
	}
}

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
		m->Update(transformArray.GetData(entity), sizeArray.GetData(entity));
		m->Draw(texArray.GetData(entity), animationArray.GetData(entity));
	}
	graphics.Draw();
}
