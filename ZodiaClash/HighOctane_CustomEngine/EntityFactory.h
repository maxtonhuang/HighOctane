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

	void LoadModels(uint32_t amount, bool isMainCharacter, const std::vector<const char*>& filenames = {});
	
	Entity CloneMasterModel(float rW, float rH, bool isMainCharacter, const std::vector<const char*>& spritesheets = {});
	void CloneMasterModel2(float rW, float rH, bool isMainCharacter);

	void LoadMasterModel();
	void CreateMasterModel(const char* filename);
	void CreateMasterModel(const char* filename, int rows, int cols);
	//extern std::unordered_map<std::string, Entity> masterEntitiesList;
	//FUNCTIONS CREATED BY NIGEL FOR TEMP / TESTING PURPOSES
	Entity CreateModel();


	//FUNCTIONS FOR MASS RENDERING - SUBJECT TO APPROVAL
	void RemoveMassRendering();
	void ReapplyMassRendering();

	void DeleteMasterModel(Entity entity);
	void DeleteCloneModel(Entity entity);


	size_t GetLayerOrder(Entity entity);
	void LayerOrderSendBackwards(Entity entity);
	void LayerOrderSendToBack(Entity entity);
	void LayerOrderBringForward(Entity entity);
	void LayerOrderBringToFront(Entity entity);

	std::vector<Entity> massRenderEntitiesList;
	std::unordered_map<std::string, Entity> masterEntitiesList;

private:

	EntityFactory() {}

	std::deque<Entity> layerOrder;

	size_t masterCounter{1};

	size_t cloneCounter{1};

};

	
