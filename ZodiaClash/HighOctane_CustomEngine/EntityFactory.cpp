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
#include "Particles.h"
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
	ECS::ecs().AddComponent(entity, Tex{}); //add tex component
	ECS::ecs().AddComponent(entity, Model{});
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
	ECS::ecs().AddComponent(entity, Collider{ Collider::SHAPE_BOX,Collider::WALL, {static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight())}, {0.0f, 0.0f} }); //add physics component

	++masterCounter;
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
	if (ECS::ecs().HasComponent<Clone>(entity)) {
		deletionEntitiesList.push_back(entity);
		--cloneCounter;
	}
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
