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
	ECS::ecs().AddComponent(entity, Collider{}); //add physics component
	ECS::ecs().AddComponent(entity, Master{});
	Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
	t->texVariants.push_back(assetmanager.texture.Get("duck.png"));
	t->texVariants.push_back(assetmanager.texture.Get("duck2.png"));
	t->tex = t->texVariants.at(0);
	ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
	ECS::ecs().AddComponent(entity, Script{}); //add script component


	//ECS::ecs().AddComponent(entity, Static{});
}

void EntityFactory::CreateMasterModel(const char* filename) {  ///////// MASTER
	Entity entity = ECS::ecs().CreateEntity();

	//std::ostringstream oss;
	//oss << "master_" << std::setfill('0') << std::setw(5) << masterCounter++;
	ECS::ecs().AddComponent(entity, Name{ filename });
	masterEntitiesList[filename] = entity;

	ECS::ecs().AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
	ECS::ecs().AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, 1.f, Vec2{ 0,0 }, 0.f, Vec2{}, TRUE });
	ECS::ecs().AddComponent(entity, Visible{ false });
	ECS::ecs().AddComponent(entity, Tex{}); //add tex component, init tex with duck sprite
	ECS::ecs().AddComponent(entity, Animator{ Animator::ANIMATION_TIME_BASED, 0.1f });
	ECS::ecs().AddComponent(entity, Model{});
	ECS::ecs().AddComponent(entity, Collider{}); //add physics component
	ECS::ecs().AddComponent(entity, Master{});
	Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
	assetmanager.LoadTexture(filename);
	t->texVariants.push_back(assetmanager.texture.Get(filename));
	t->tex = t->texVariants.at(0);
	ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
	++masterCounter;

}

void EntityFactory::CreateMasterModel(const char* filename, int rows, int cols) {  ///////// MASTER
	Entity entity = ECS::ecs().CreateEntity();

	ECS::ecs().AddComponent(entity, Name{ filename });
	masterEntitiesList[filename] = entity;

	ECS::ecs().AddComponent(entity, Color{ glm::vec4{ 1,1,1,1 } });
	ECS::ecs().AddComponent(entity, Transform{ Vec2{ 0.f,0.f }, 0.f, 1.f, Vec2{ 0,0 }, 0.f, Vec2{}, TRUE });
	ECS::ecs().AddComponent(entity, Visible{ false });
	ECS::ecs().AddComponent(entity, Tex{}); //add tex component, init tex with duck sprite
	ECS::ecs().AddComponent(entity, Animator{ Animator::ANIMATION_TIME_BASED, 0.1f });
	ECS::ecs().AddComponent(entity, Model{});
	ECS::ecs().AddComponent(entity, Collider{}); //add physics component
	ECS::ecs().AddComponent(entity, Master{});
	
	WriteSpriteConfig(filename, rows, cols);
	std::string filenameString = filename;
	std::ostringstream oss;
	oss << filenameString.substr(0, filenameString.find_last_of('.')) << ".spritesheet";

	Tex* t = &ECS::ecs().GetComponent<Tex>(entity);
	assetmanager.LoadSpritesheet(oss.str());
	t->texVariants.push_back(assetmanager.texture.Get(filename));
	t->tex = t->texVariants.at(0);
	ECS::ecs().AddComponent(entity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
	++masterCounter;

}


/******************************************************************************
*
*	@brief Clones Master Model
*
*	This function clones new game objects from the master model.
*
******************************************************************************/
///////// CLONE 1
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
		// (TO CHECK: add using copy ctor here? then default ctor if no spritesheet?)
		// set default aniType to time-based
		//ECS::ecs().GetComponent<Animator>(entity).animationType = Animator::ANIMATION_TIME_BASED;
		//ECS::ecs().GetComponent<Animator>(entity).frameDisplayDuration = 0.1f;
		// resize size to tex dimensions
		ECS::ecs().GetComponent<Size>(entity).width = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetWidth();
		ECS::ecs().GetComponent<Size>(entity).height = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetHeight();
		//// for mass rendering - add this entity to vector
		massRenderEntitiesList.emplace_back(entity);
	}
	layering[selectedLayer].emplace_back(entity);
	++cloneCounter;
	return entity;
}


void EntityFactory::CloneMaster(Entity& masterEntity) {  ///////// CLONE 2
	Entity entity = ECS::ecs().CreateEntity();
	
	ECS::ecs().AddComponent(entity, Name{ ( (ECS::ecs().GetComponent<Name>(masterEntity).name)+"_CLONE").c_str(),false });

	ECS::ecs().AddComponent(entity, Color{ ECS::ecs().GetComponent<Color>(masterEntity) });

	if (rightClick) {
		//Transform transform;
		ECS::ecs().AddComponent(entity, Transform{ ECS::ecs().GetComponent<Transform>(masterEntity) });
		ECS::ecs().GetComponent<Transform>(entity).position = { 0.f,0.f };
	}
	else {
		ECS::ecs().AddComponent(entity, Transform{ ECS::ecs().GetComponent<Transform>(masterEntity) });
	}

	if (ECS::ecs().HasComponent<Tex>(masterEntity)) {
		ECS::ecs().AddComponent(entity, Tex{ ECS::ecs().GetComponent<Tex>(masterEntity) });
	}
	

	ECS::ecs().AddComponent(entity, Visible{ true });
	ECS::ecs().AddComponent(entity, Size{ ECS::ecs().GetComponent<Size>(masterEntity) });
	ECS::ecs().AddComponent(entity, Model{});
	if (ECS::ecs().HasComponent<Animator>(masterEntity)) {
		ECS::ecs().AddComponent(entity, Animator{ ECS::ecs().GetComponent<Animator>(masterEntity) });
	}
	ECS::ecs().AddComponent(entity, Collider{});
	if (!(ECS::ecs().GetComponent<Name>(entity).name == "background_CLONE")) {
		ECS::ecs().AddComponent(entity, Movable{});
	}
	ECS::ecs().AddComponent(entity, Clone{});
	ECS::ecs().AddComponent(entity, Script{}); //add script component
	if (ECS::ecs().HasComponent<TextLabel>(masterEntity)) {
		ECS::ecs().AddComponent<TextLabel>(entity, TextLabel{ ECS::ecs().GetComponent<TextLabel>(masterEntity) });
	}
	if (ECS::ecs().HasComponent<Button>(masterEntity)) {
		ECS::ecs().AddComponent<Button>(entity, Button{ ECS::ecs().GetComponent<Button>(masterEntity) });
	}
	if (ECS::ecs().HasComponent<CharacterStats>(masterEntity)) {
		ECS::ecs().AddComponent<CharacterStats>(entity, CharacterStats{ ECS::ecs().GetComponent<CharacterStats>(masterEntity) });
	}

	layering[selectedLayer].emplace_back(entity);
	++cloneCounter;




	//ECS::ecs().GetComponent<Transform>(entity).position = { rW, rH };
	//ECS::ecs().GetComponent<Collider>(entity).bodyShape = Collider::SHAPE_BOX;
	//ECS::ecs().GetComponent<Transform>(entity).isStatic = true;

	// check if any spritesheets have been loaded
	//if (spritesheets.size() > 0) {
	//	for (const char* filename : spritesheets) {
	//		ECS::ecs().GetComponent<Tex>(entity).texVariants.push_back(assetmanager.texture.Get(filename)); // add a texVariant
	//	}
	//	ECS::ecs().GetComponent<Tex>(entity).tex = ECS::ecs().GetComponent<Tex>(entity).texVariants[0]; // set default tex to first texVariant
	//	// (TO CHECK: add using copy ctor here? then default ctor if no spritesheet?)
	//	// set default aniType to time-based
	//	//ECS::ecs().GetComponent<Animator>(entity).animationType = Animator::ANIMATION_TIME_BASED;
	//	//ECS::ecs().GetComponent<Animator>(entity).frameDisplayDuration = 0.1f;
	//	// resize size to tex dimensions
	//	ECS::ecs().GetComponent<Size>(entity).width = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetWidth();
	//	ECS::ecs().GetComponent<Size>(entity).height = (float)ECS::ecs().GetComponent<Tex>(entity).tex->GetHeight();
	//	//// for mass rendering - add this entity to vector
	//	massRenderEntitiesList.push_back(entity);
	//}
	
	//return entity;
}

/******************************************************************************
*
*	@brief Clones Master Model
*
*	This function clones new game objects from the master model.
*
******************************************************************************/
//void EntityFactory::CloneMasterModel2(float rW, float rH, bool isMainCharacter) {   ///////// CLONE 3
//	Entity entity = ECS::ecs().CreateEntity();
//	{
//		std::ostringstream oss;
//		oss << "entity_" << std::setfill('0') << std::setw(5) << masterCounter++;
//		ECS::ecs().AddComponent(entity, Name{ oss.str(), false });
//	}
//	Entity masterEntity = (masterEntitiesList.find("master_00002"))->second;
//	ECS::ecs().AddComponent(entity, Color{ ECS::ecs().GetComponent<Color>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Transform{ ECS::ecs().GetComponent<Transform>(masterEntity) });
//	ECS::ecs().GetComponent<Transform>(entity).position = {rW, rH};
//	ECS::ecs().AddComponent(entity, Tex{ ECS::ecs().GetComponent<Tex>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Visible{ true });
//	ECS::ecs().AddComponent(entity, Size{ ECS::ecs().GetComponent<Size>(masterEntity) });
//	if (isMainCharacter) {
//		//ECS::ecs().AddComponent(entity, MainCharacter{});
//	}		
//	ECS::ecs().AddComponent(entity, Model{ ECS::ecs().GetComponent<Model>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Animator{ ECS::ecs().GetComponent<Animator>(masterEntity)});
//	ECS::ecs().AddComponent(entity, Clone{});
//	ECS::ecs().AddComponent(entity, Collider{});
//	ECS::ecs().AddComponent(entity, Movable{});
//	ECS::ecs().AddComponent(entity, Script{}); //add script component
//	ECS::ecs().GetComponent<Collider>(entity).bodyShape = Collider::SHAPE_BOX;
//	ECS::ecs().GetComponent<Transform>(entity).isStatic = true;
//	layering[selectedLayer].emplace_back(entity);
//	++cloneCounter;
//}

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







//----------

//FUNCTIONS CREATED BY NIGEL FOR TEMP / TESTING PURPOSES
//Entity EntityFactory::CreateModel() {
//	Entity entity = ECS::ecs().CreateEntity();
//	Entity masterEntity = (masterEntitiesList.find("CAT"))->second;
//	ECS::ecs().AddComponent(entity, Color{ ECS::ecs().GetComponent<Color>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Transform{ ECS::ecs().GetComponent<Transform>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Tex{ ECS::ecs().GetComponent<Tex>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Visible{ true });
//	ECS::ecs().AddComponent(entity, Size{ ECS::ecs().GetComponent<Size>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Model{ ECS::ecs().GetComponent<Model>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Animator{ ECS::ecs().GetComponent<Animator>(masterEntity) });
//	ECS::ecs().AddComponent(entity, Clone{});
//	layerOrder.emplace_back(entity);
//	return entity;
//}





void EntityFactory::DeleteMasterModel(Entity entity) {
	masterEntitiesList.erase(ECS::ecs().GetComponent<Name>(entity).name);
	ECS::ecs().DestroyEntity(entity);
	--masterCounter;
}


void EntityFactory::DeleteCloneModel(Entity entity) {
	//remove from layer


	//{
	//	std::cout << "Layering Size: " << layering.size() << std::endl;
	//	int count = 0;
	//	for (auto& val : layering) {
	//		std::cout << "Layering[" << count++ << "] size: " << val.size() << std::endl;
	//	}
	//	for (std::string val : layerNames) {
	//		std::cout << "Layer Names: " << val << std::endl;
	//	}
	//	std::cout << "----------" << std::endl;
	//}



	ECS::ecs().DestroyEntity(entity);
	// find entity > remove from layer
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		layering[pos.first].erase(layering[pos.first].begin() + pos.second);
	}
	--cloneCounter;

	//{
	//	std::cout << "Layering Size: " << layering.size() << std::endl;
	//	int count = 0;
	//	for (auto& val : layering) {
	//		std::cout << "Layering[" << count++ << "] size: " << val.size() << std::endl;
	//	}
	//	for (std::string val : layerNames) {
	//		std::cout << "Layer Names: " << val << std::endl;
	//	}
	//	std::cout << "----------" << std::endl;
	//}

}


