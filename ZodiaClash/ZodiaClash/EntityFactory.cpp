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
*	@file		EntityFactory.cpp
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
*	@brief		The place where new entities are born
*
*	This file does the follwing:
*	[1]	Load master models which are not rendered by the Graphics Manager.
*	[2] Clone models from the master models. These clones will be the in-game
*		entities.
*
******************************************************************************/

#include <cstdint>
#include <random>
#include <unordered_map>
#include "EntityFactory.h"
#include "Components.h"
#include "ECS.h"
#include "Message.h"
#include "EngineCore.h"
#include "VMath.h"
#include "GraphLib.h"
#include "Texture.h"
#include "Graphics.h"
#include "physics.h"



std::unordered_map<std::string, Entity> masterEntitiesList;
std::vector<Entity> massRenderEntitiesList;

	void LoadMasterModel() {

		// REMOVE HARDCODING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		{
			Entity entity = ECS::ecs().CreateEntity();
			masterEntitiesList["CAT"] = entity;
			ECS::ecs().AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
			//ECS::ecs().AddComponent(entity, Transform{ glm::vec2{(rand_width(rng) - graphics.GetWidth() / 2), (rand_height(rng) - graphics.GetHeight() / 2)}, 0.f, glm::vec2{1, 1} });
			ECS::ecs().AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, Vec2{ 1.f, 1.f }, vmath::Vector2{ 0,0 } });
			ECS::ecs().AddComponent(entity, Visible{ false });
			//ECS::ecs().AddComponent(entity, Tex{ texList.Add("cat.png") });

			//add tex component, init tex with duck sprite
			ECS::ecs().AddComponent(entity, Tex{ texList.Add("duck.png") });
			Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
			t->texVariants.push_back(texList.Add("duck.png"));
			t->texVariants.push_back(texList.Add("duck2.png"));
			t->tex = t->texVariants.at(0);
			ECS::ecs().AddComponent(entity, Animation{});
			Animation* a = &ECS::ecs().GetComponent<Animation>(entity);
			a->animationType = Animation::ANIMATION_TIME_BASED;
			//a->animationType = Animation::ANIMATION_EVENT_BASED;
			a->frameDisplayDuration = 0.1f;
			ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
			//ECS::ecs().AddComponent(entity, MainCharacter{});
			ECS::ecs().AddComponent(entity, Model{});

			//add physics component
			ECS::ecs().AddComponent<physics::Body>(entity, physics::Body{});
			ECS::ecs().AddComponent<Collider>(entity, Collider{});
		}
	}

void CloneMasterModel(float rW, float rH, bool isMainCharacter, const std::vector<const char*>& spritesheets) {
	Entity entity = ECS::ecs().CreateEntity();
	Entity masterEntity = (masterEntitiesList.find("CAT"))->second;
	ECS::ecs().AddComponent(entity, Color{ ECS::ecs().GetComponent<Color>(masterEntity) });
	ECS::ecs().AddComponent(entity, Transform{ ECS::ecs().GetComponent<Transform>(masterEntity) });
	ECS::ecs().GetComponent<Transform>(entity).position = {rW, rH};
	ECS::ecs().AddComponent(entity, Tex{ ECS::ecs().GetComponent<Tex>(masterEntity) });
	ECS::ecs().AddComponent(entity, Visible{ true });
	ECS::ecs().AddComponent(entity, Size{ ECS::ecs().GetComponent<Size>(masterEntity) });
	if (isMainCharacter) {
		ECS::ecs().AddComponent(entity, MainCharacter{});
	}		
	ECS::ecs().AddComponent(entity, Model{ ECS::ecs().GetComponent<Model>(masterEntity) });
	ECS::ecs().AddComponent(entity, Animation{ ECS::ecs().GetComponent<Animation>(masterEntity)});
	ECS::ecs().AddComponent(entity, Clone{});
	ECS::ecs().AddComponent<physics::Body>(entity, ECS::ecs().GetComponent<physics::Body>(masterEntity));
	ECS::ecs().AddComponent<Collider>(entity, ECS::ecs().GetComponent<Collider>(masterEntity));

	// check if any spritesheets have been loaded
	if (spritesheets.size() > 0) {
		for (const char* filename : spritesheets) {
			// add a texVariant
			ECS::ecs().GetComponent<Tex>(entity).texVariants.push_back(texList.Add(filename));
		}
		// set default tex to first texVariant
		ECS::ecs().GetComponent<Tex>(entity).tex = ECS::ecs().GetComponent<Tex>(entity).texVariants[0];
		// set default aniType to event-based
		ECS::ecs().GetComponent<Animation>(entity).animationType = Animation::ANIMATION_TIME_BASED;
		ECS::ecs().GetComponent<Animation>(entity).frameDisplayDuration = 0.1f;
		// resize size to tex dimensions
		ECS::ecs().GetComponent<Size>(entity).width = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetWidth();
		ECS::ecs().GetComponent<Size>(entity).height = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetHeight();

		//// for mass rendering - add this entity to vector
		massRenderEntitiesList.push_back(entity);
	}
}

void LoadModels(uint32_t amount, bool isMainCharacter, const std::vector<const char*>& spritesheets) {
	// generate random positions to spawn models
	std::default_random_engine rng;
	std::uniform_real_distribution<float> rand_width(-GRAPHICS::w, GRAPHICS::w);
	std::uniform_real_distribution<float> rand_height(-GRAPHICS::h, GRAPHICS::h);
		
	for (uint32_t i = 0; i < amount; ++i) {
		CloneMasterModel(rand_width(rng), rand_height(rng), isMainCharacter, spritesheets);
	}
}

void RemoveMassRendering() {
	for (Entity const& entity : massRenderEntitiesList) {
		ECS::ecs().RemoveComponent<Clone>(entity);
	}
}

void ReapplyMassRendering() {
	for (Entity const& entity : massRenderEntitiesList) {
		ECS::ecs().AddComponent(entity, Clone{});
	}
}


//FUNCTIONS CREATED BY NIGEL FOR TEMP / TESTING PURPOSES
Entity CreateModel() {
	Entity entity = ECS::ecs().CreateEntity();
	Entity masterEntity = (masterEntitiesList.find("CAT"))->second;
	ECS::ecs().AddComponent(entity, Color{ ECS::ecs().GetComponent<Color>(masterEntity) });
	ECS::ecs().AddComponent(entity, Transform{ ECS::ecs().GetComponent<Transform>(masterEntity) });
	ECS::ecs().AddComponent(entity, Tex{ ECS::ecs().GetComponent<Tex>(masterEntity) });
	ECS::ecs().AddComponent(entity, Visible{ true });
	ECS::ecs().AddComponent(entity, Size{ ECS::ecs().GetComponent<Size>(masterEntity) });
	ECS::ecs().AddComponent(entity, Model{ ECS::ecs().GetComponent<Model>(masterEntity) });
	ECS::ecs().AddComponent(entity, Animation{});
	ECS::ecs().AddComponent(entity, Clone{});
	return entity;
}
