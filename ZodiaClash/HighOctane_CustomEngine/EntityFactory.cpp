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
* 	@co-author	Foong Pun Yuen Nigel (CloneMaster, ClonePrefab, UpdateDeletion functions)
*
*	@email		p.foong\@digipen.edu
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
*	This file does the following:
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
#include "collision.h"
#include "Model.h"
#include "AssetManager.h"
#include "Serialization.h"
#include "Global.h"
#include "Layering.h"
#include <deque>
#include <algorithm>
#include <limits>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "CharacterStats.h"
#include "UndoRedo.h"


/*

1. Create default entity
2. How to change position / size / etc?

1. Load asset into asset library (Change size to fit)
2. Click and drag from asset library (default properties)
3. Change properties
4. Need name for each entity

default name: entity_0001, entity_0002, etc

how to keep track of name?
Instead of not visible, show in asset library

*/


/******************************************************************************
*
*	@brief Loads Master Model
*
*	This loads a master model where new game objects will be cloned from.
*
******************************************************************************/
void EntityFactory::LoadMasterModel() {  ///////// MASTER
	Entity entity = ECS::ecs().CreateEntity();

	std::ostringstream oss;
	oss << "master_" << std::setfill('0') << std::setw(5) << masterCounter++;
	ECS::ecs().AddComponent(entity, Name{ oss.str() });
	masterEntitiesList[oss.str()] = entity;

	ECS::ecs().AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
	ECS::ecs().AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, 1.f, Vec2{ 0,0 }, 0.f, Vec2{}, TRUE });
	ECS::ecs().AddComponent(entity, Visible{ false });
	ECS::ecs().AddComponent(entity, Tex{}); //add tex component, init tex with duck sprite
	ECS::ecs().AddComponent(entity, Animator{ Animator::ANIMATION_TIME_BASED, 0.1f });
	ECS::ecs().AddComponent(entity, Model{});
	ECS::ecs().AddComponent(entity, Master{});
	Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
	t->texVariants.push_back(assetmanager.texture.Get("duck.png"));
	t->texVariants.push_back(assetmanager.texture.Get("duck2.png"));
	t->tex = t->texVariants.at(0);
	ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
	ECS::ecs().AddComponent(entity, Collider{ Collider::SHAPE_BOX, {static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight())}, {0.0f, 0.0f} }); //add physics component
	ECS::ecs().AddComponent(entity, Script{}); //add script component


	//ECS::ecs().AddComponent(entity, Static{});
}

/******************************************************************************
*
*	@brief Creates New Master Entities 1 (NEW!)
*
*	This function creates new Master Entities with Static Images.
*
******************************************************************************/
Entity EntityFactory::CreateMasterModel(const char* filename) {
	Entity entity = ECS::ecs().CreateEntity();

	ECS::ecs().AddComponent(entity, Name{ filename });
	masterEntitiesList[filename] = entity;

	ECS::ecs().AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
	ECS::ecs().AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, 1.f, Vec2{ 0,0 }, 0.f, Vec2{}, TRUE });
	ECS::ecs().AddComponent(entity, Visible{ false });
	ECS::ecs().AddComponent(entity, Tex{}); //add tex component, init tex with duck sprite
	ECS::ecs().AddComponent(entity, Model{});
	ECS::ecs().AddComponent(entity, Master{});
	ECS::ecs().AddComponent(entity, Movable{});
	Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
	assetmanager.LoadTexture(filename);
	t->texVariants.push_back(assetmanager.texture.Get(filename));
	t->tex = t->texVariants.at(0);
	ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
	++masterCounter;
	return entity;
}

/******************************************************************************
*
*	@brief Creates New Master Entities 2 (NEW!)
*
*	This function creates new Master Entities with Spritesheets.
*
******************************************************************************/
Entity EntityFactory::CreateMasterModel(const char* filename, int rows, int cols) {
	Entity entity = ECS::ecs().CreateEntity();

	ECS::ecs().AddComponent(entity, Name{ filename });
	masterEntitiesList[filename] = entity;

	ECS::ecs().AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
	ECS::ecs().AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, 1.f, Vec2{ 0,0 }, 0.f, Vec2{}, TRUE });
	ECS::ecs().AddComponent(entity, Visible{ false });
	ECS::ecs().AddComponent(entity, Tex{}); //add tex component, init tex with duck sprite
	//ECS::ecs().AddComponent(entity, Animator{ Animator::ANIMATION_TIME_BASED, 0.1f });
	ECS::ecs().AddComponent(entity, Model{});
	//ECS::ecs().AddComponent(entity, Collider{}); //add physics component
	ECS::ecs().AddComponent(entity, Master{});
	ECS::ecs().AddComponent(entity, Movable{});

	WriteSpriteConfig(filename, rows, cols);
	std::string filenameString = filename;
	std::ostringstream oss;
	oss << filenameString.substr(0, filenameString.find_last_of('.')) << ".spritesheet";

	Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
	assetmanager.LoadSpritesheet(oss.str());
	t->texVariants.push_back(assetmanager.texture.Get(filename));
	t->tex = t->texVariants.at(0);
	ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
	ECS::ecs().AddComponent(entity, Collider{ Collider::SHAPE_BOX, {static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight())}, {0.0f, 0.0f} }); //add physics component

	++masterCounter;
	return entity;
}

/******************************************************************************
*
*	@brief Clones Master Model 1
*
*	This function clones new game objects from Master Entities.
*
******************************************************************************/
Entity EntityFactory::CloneMasterModel(float rW, float rH, bool isMainCharacter, const std::vector<const char*>& spritesheets) {
	Entity entity = ECS::ecs().CreateEntity();

	std::ostringstream oss;
	oss << "entity_" << std::setfill('0') << std::setw(5) << cloneCounter++;
	ECS::ecs().AddComponent(entity, Name{ oss.str() });

	Entity masterEntity = (masterEntitiesList.find("master_00001"))->second;
	ECS::ecs().AddComponent(entity, Color{ ECS::ecs().GetComponent<Color>(masterEntity) });
	ECS::ecs().AddComponent(entity, Transform{ ECS::ecs().GetComponent<Transform>(masterEntity) });
	ECS::ecs().AddComponent(entity, Tex{ ECS::ecs().GetComponent<Tex>(masterEntity) });
	ECS::ecs().AddComponent(entity, Visible{ true });
	ECS::ecs().AddComponent(entity, Size{ ECS::ecs().GetComponent<Size>(masterEntity) });
	ECS::ecs().AddComponent(entity, Model{ ECS::ecs().GetComponent<Model>(masterEntity) });
	ECS::ecs().AddComponent(entity, Animator{ ECS::ecs().GetComponent<Animator>(masterEntity) });
	ECS::ecs().AddComponent(entity, Collider{});
	ECS::ecs().AddComponent(entity, Movable{});
	ECS::ecs().AddComponent(entity, Clone{});
	ECS::ecs().AddComponent(entity, Script{}); //add script component
	ECS::ecs().GetComponent<Transform>(entity).position = { rW, rH };
	ECS::ecs().GetComponent<Collider>(entity).bodyShape = Collider::SHAPE_BOX;
	ECS::ecs().GetComponent<Collider>(entity).position = ECS::ecs().GetComponent<Transform>(entity).position;
	ECS::ecs().GetComponent<Transform>(entity).isStatic = true;

	if (isMainCharacter) {
		ECS::ecs().AddComponent(entity, MainCharacter{});
	}
	// check if any spritesheets have been loaded
	if (spritesheets.size() > 0) {
		for (const char* filename : spritesheets) {
			ECS::ecs().GetComponent<Tex>(entity).texVariants.push_back(assetmanager.texture.Get(filename)); // add a texVariant
		}
		ECS::ecs().GetComponent<Tex>(entity).tex = ECS::ecs().GetComponent<Tex>(entity).texVariants[0]; // set default tex to first texVariant
		// resize size to tex dimensions
		ECS::ecs().GetComponent<Size>(entity).width = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetWidth();
		ECS::ecs().GetComponent<Size>(entity).height = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetHeight();
		ECS::ecs().GetComponent<Collider>(entity).dimension = { (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetWidth(), (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetHeight() };
		//// for mass rendering - add this entity to vector
		massRenderEntitiesList.emplace_back(entity);
	}
	layering[selectedLayer].emplace_back(entity);
	++cloneCounter;
	return entity;
}


/******************************************************************************
*
*	@brief Clones Master Model 2 (NEW!)
*
*	This function clones new game objects from another entity.
*
******************************************************************************/
Entity EntityFactory::CloneMaster(Entity& masterEntity) {
	static auto& typeMap{ ECS::ecs().GetTypeManager() };
	Entity entity = ECS::ecs().CreateEntity();

	for (auto& ecsType : typeMap) {
		if (ecsType.second->HasComponent(masterEntity)) {
			ecsType.second->AddComponent(entity);
			ecsType.second->CopyComponent(entity, masterEntity);
		}
	}
	if (ECS::ecs().HasComponent<Master>(entity)) {
		ECS::ecs().RemoveComponent<Master>(entity);
	}
	if (ECS::ecs().HasComponent<Clone>(entity)) {
		ECS::ecs().RemoveComponent<Clone>(entity);
	}
	ECS::ecs().AddComponent(entity, Clone{});

	if (ECS::ecs().HasComponent<Parent>(masterEntity)) {
		ECS::ecs().GetComponent<Parent>(entity).children.clear();
		for (auto& child : ECS::ecs().GetComponent<Parent>(masterEntity).children) {
			Entity childClone = ECS::ecs().CreateEntity();
			for (auto& ecsType : typeMap) {
				if (ecsType.second->HasComponent(child)) {
					ecsType.second->AddComponent(childClone);
					ecsType.second->CopyComponent(childClone, child);
				}
			}
			ECS::ecs().AddComponent<Clone>(childClone, Clone{});
			ECS::ecs().GetComponent<Child>(childClone).parent = entity;
			ECS::ecs().GetComponent<Parent>(entity).children.push_back(childClone);

			std::pair<size_t, size_t> p = FindInLayer(child);
			if (p.first != ULLONG_MAX && p.second != ULLONG_MAX) {
				layering[p.first].emplace_back(childClone);
			}
			else {
				if (layering.size() == 0) {
					layering.emplace_back(std::deque<Entity>{});
				}
				layering[layering.size() - 1].emplace_back(childClone);
			}
		}
	}

	if (assetmanager.GetPrefabName(masterEntity) != "") {
		ECS::ecs().GetComponent<Clone>(entity).prefab = assetmanager.GetPrefabName(masterEntity);
	}

	std::pair<size_t, size_t> p = FindInLayer(masterEntity);
	if (p.first != ULLONG_MAX && p.second != ULLONG_MAX) {
		layering[p.first].emplace_back(entity);
	}
	else {
		if (layering.size() == 0) {
			layering.emplace_back(std::deque<Entity>{});
		}
		layering[layering.size() - 1].emplace_back(entity);
	}

	RebuildLayeringAfterDeserialization();
	ExtractSkipLockAfterDeserialization();
	++cloneCounter;
	if (GetCurrentSystemMode() != SystemMode::GAMEHELP && GetCurrentSystemMode() != SystemMode::PAUSE && !initLevel) {
		undoRedo.RecordCurrent(entity, ACTION::ADDENTITY);
	}
	return entity;
}

/******************************************************************************
*
*	@brief Clones prefab
*
*	This function clones new game objects from an input prefab name
*
******************************************************************************/
Entity EntityFactory::ClonePrefab(std::string prefabName) {
	Entity prefab{ assetmanager.GetPrefab(prefabName) };
	if (prefab == 0) {
		return 0;
	}
	Entity clone{ CloneMaster(prefab) };
	ECS::ecs().GetComponent<Clone>(clone).prefab = prefabName;
	return clone;
}

/******************************************************************************
*
*	@brief Load Models Function
*
*	Sends random positions into the CloneMasterModel() function.
*
******************************************************************************/
void EntityFactory::LoadModels(uint32_t amount, bool isMainCharacter, const std::vector<const char*>& spritesheets) {
	// generate random positions to spawn models
	//std::default_random_engine rng;
	//std::uniform_real_distribution<float> rand_width(-GRAPHICS::w, GRAPHICS::w);
	//std::uniform_real_distribution<float> rand_height(-GRAPHICS::h, GRAPHICS::h);
	float column = sqrt(amount / GRAPHICS::ar);
	float row = column * GRAPHICS::ar;
	float count_column = 0;
	float count_row = 0;
	float gridwidth = GRAPHICS::defaultWidthF / row;
	float gridheight = GRAPHICS::defaultHeightF / column;

	for (uint32_t i = 0; i < amount; ++i) {
		float x{};
		float y{};
		if (spritesheets.size() > 0) {
			x = (count_row / row - 0.5f) * GRAPHICS::defaultWidthF * 0.9f;
			y = (count_column / column - 0.5f) * GRAPHICS::defaultHeightF * 0.9f;
		}
		else {
			x = 0.f;
			y = 0.f;
		}
		Entity entity = CloneMasterModel(x, y, isMainCharacter, spritesheets);
		if (spritesheets.size() > 0) {
			ECS::ecs().RemoveComponent<Collider>(entity);
			if (gridwidth < ECS::ecs().GetComponent<Size>(entity).width) {
				ECS::ecs().GetComponent<Size>(entity).width = gridwidth;
			}
			if (gridheight < ECS::ecs().GetComponent<Size>(entity).height) {
				ECS::ecs().GetComponent<Size>(entity).height = gridheight;
			}
		}
		++count_column;
		if (count_column > column) {
			count_column = 0;
			++count_row;
		}
		//CloneMasterModel(rand_width(rng), rand_height(rng), isMainCharacter, spritesheets);
	}
}

/******************************************************************************
*
*	Stop mass rendering
*
******************************************************************************/
void EntityFactory::RemoveMassRendering() {
	for (Entity const& entity : massRenderEntitiesList) {
		ECS::ecs().RemoveComponent<Clone>(entity);
	}
}

/******************************************************************************
*
*	Mass renders objects
*
******************************************************************************/
void EntityFactory::ReapplyMassRendering() {
	for (Entity const& entity : massRenderEntitiesList) {
		ECS::ecs().AddComponent(entity, Clone{});
	}
}

/******************************************************************************
*
*	Function for deleting Master Models (NEW!)
*
******************************************************************************/
void EntityFactory::DeleteMasterModel(Entity entity) {
	masterEntitiesList.erase(ECS::ecs().GetComponent<Name>(entity).name);
	ECS::ecs().DestroyEntity(entity);
	--masterCounter;
}

/******************************************************************************
*
*	Function for deleting Clone Models (NEW!)
*
******************************************************************************/
void EntityFactory::DeleteCloneModel(Entity entity) {
	deletionEntitiesList.push_back(entity);
	--cloneCounter;
}

/******************************************************************************
*
*	Function for deleting entities at the end of every frame
*
******************************************************************************/
void EntityFactory::UpdateDeletion() {
	for (Entity entity : deletionEntitiesList) {
		if (ECS::ecs().EntityExists(entity)) {
			ECS::ecs().DestroyEntity(entity);
			RemoveEntityFromLayering(entity);
		}
	}
	deletionEntitiesList.clear();
}
