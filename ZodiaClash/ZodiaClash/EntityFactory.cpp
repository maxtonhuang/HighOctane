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




	extern ECS ecs;



	std::unordered_map<std::string, Entity> masterEntitiesList;

	void LoadMasterModel() {

		// REMOVE HARDCODING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		{
			Entity entity = ecs.CreateEntity();
			masterEntitiesList["CAT"] = entity;
			ecs.AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
			//ecs.AddComponent(entity, Transform{ glm::vec2{(rand_width(rng) - graphics.GetWidth() / 2), (rand_height(rng) - graphics.GetHeight() / 2)}, 0.f, glm::vec2{1, 1} });
			ecs.AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, Vec2{ 1.f, 1.f }, vmath::Vector2{ 0,0 } });
			ecs.AddComponent(entity, Tex{ texList.Add("cat.png") });
			ecs.AddComponent(entity, Visible{ false });
			Tex t = ecs.GetComponent<Tex>(entity);
			ecs.AddComponent(entity, Size{ static_cast<float>(t.tex->GetWidth()), static_cast<float>(t.tex->GetHeight()) });
			//ecs.AddComponent(entity, MainCharacter{});
			ecs.AddComponent(entity, Model{});
		}
	}

	void CloneMasterModel(float rW, float rH, bool isMainCharacter) {
		/*
		SUGGESTION:
		Maybe make a for loop that checks each component type.
		for (each component type) {
			if master entity contains component type
			add component to clone with master entity data
		}
		at end of for loop, change necessary parameters.
		*/
		Entity entity = ecs.CreateEntity();
		Entity masterEntity = (masterEntitiesList.find("CAT"))->second;
		ecs.AddComponent(entity, Color{ ecs.GetComponent<Color>(masterEntity) });
		ecs.AddComponent(entity, Transform{ ecs.GetComponent<Transform>(masterEntity) });
		ecs.GetComponent<Transform>(entity).position = {rW, rH};
		//tr.position = { rW, rH };
		ecs.AddComponent(entity, Tex{ ecs.GetComponent<Tex>(masterEntity) });
		ecs.AddComponent(entity, Visible{ true });
		ecs.AddComponent(entity, Size{ ecs.GetComponent<Size>(masterEntity) });
		if (isMainCharacter) {
			ecs.AddComponent(entity, MainCharacter{});
		}		
		ecs.AddComponent(entity, Model{ ecs.GetComponent<Model>(masterEntity) });
		ecs.AddComponent(entity, Animation{});
		ecs.AddComponent(entity, Clone{});
		
	}

	void LoadModels(uint32_t amount, bool isMainCharacter) {

		std::default_random_engine rng;
		std::uniform_real_distribution<float> rand_width(-GRAPHICS::w, GRAPHICS::w);
		std::uniform_real_distribution<float> rand_height(-GRAPHICS::h, GRAPHICS::h);
		
		for (uint32_t i = 0; i < amount; ++i) {
			CloneMasterModel(rand_width(rng), rand_height(rng), isMainCharacter);
		}
	}

	//FUNCTIONS CREATED BY NIGEL FOR TEMP / TESTING PURPOSES

	Entity CreateModel() {
		Entity entity = ecs.CreateEntity();
		Entity masterEntity = (masterEntitiesList.find("CAT"))->second;
		ecs.AddComponent(entity, Color{ ecs.GetComponent<Color>(masterEntity) });
		ecs.AddComponent(entity, Transform{ ecs.GetComponent<Transform>(masterEntity) });
		ecs.AddComponent(entity, Tex{ ecs.GetComponent<Tex>(masterEntity) });
		ecs.AddComponent(entity, Visible{ true });
		ecs.AddComponent(entity, Size{ ecs.GetComponent<Size>(masterEntity) });
		ecs.AddComponent(entity, Model{ ecs.GetComponent<Model>(masterEntity) });
		ecs.AddComponent(entity, Animation{});
		ecs.AddComponent(entity, Clone{});
		return entity;
	}
