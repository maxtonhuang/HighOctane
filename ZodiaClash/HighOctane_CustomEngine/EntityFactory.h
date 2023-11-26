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
*	@file		EntityFactory.h
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*  	@co-author	Foong Pun Yuen Nigel (CloneMaster and ClonePrefab functions)
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
*	@brief		Function Declarations to functions in EntityFactory.cpp
*
*	-
*
******************************************************************************/

#pragma once

#include "ECS.h"


class EntityFactory {

public:
	
	// Disallow copying to prevent creation of more than one instance
	EntityFactory(const EntityFactory&) = delete;
	EntityFactory& operator=(const EntityFactory&) = delete;

	// Public accessor for the Singleton instance
	static EntityFactory& entityFactory() {
		static EntityFactory ef;
		return ef;
	}

	//Clones a new entity using input masterEntity
	Entity CloneMaster(Entity& masterEntity);

	//Clones a new entity using input prefab name
	Entity ClonePrefab(std::string prefabName);

	Entity CreateMasterModel(const char* filename);
	Entity CreateMasterModel(const char* filename, int rows, int cols);

	void DeleteMasterModel(Entity entity);

	//Deletes the input entity
	void DeleteCloneModel(Entity entity);

	//Handles entity deletion at the end of every frame
	void UpdateDeletion();

	std::vector<Entity> massRenderEntitiesList;
	std::vector<Entity> deletionEntitiesList;
	std::unordered_map<std::string, Entity> masterEntitiesList;

	size_t masterCounter{1};
	size_t cloneCounter{1};

private:

	EntityFactory() {}

};

	
