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
*	@file		Layering.cpp
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
*	@date		3 November 2023
*
* *****************************************************************************
*
*	@brief		Functions to enable layering of entities in the Scenes
*
*	This file contains functions to rearrange layers, add new layers, delete
*   layers, and other functions related to layering.
*
******************************************************************************/


#include "Layering.h"
#include "Global.h"
#include "Editing.h"
#include "ECS.h"
#include "Selection.h"
#include <sstream>
#include <algorithm>
#include <limits>

/******************************************************************************
*
*	@brief Finds the position of an entity in the layering deque of deques
*
*	-
*
******************************************************************************/
std::pair<size_t, size_t> FindInLayer(Entity entity) {
	for (size_t i = 0; i < layering.size(); ++i) {
		for (size_t j = 0; j < layering[i].size(); ++j) {
			if (layering[i][j] == entity) {
				return { i, j };
			}
		}
	}
	return { ULLONG_MAX, ULLONG_MAX }; // if not found
}

/******************************************************************************
*
*	@brief Sends an entity backward one position
*
*	-
*
******************************************************************************/
void LayerOrderSendBackward(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != 0) {
			std::swap(layering[pos.first][pos.second], layering[pos.first][pos.second - 1]);
		}
	}
}

/******************************************************************************
*
*	@brief Sends an entity to the back of the layer
*
*	-
*
******************************************************************************/
void LayerOrderSendToBack(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != 0) {
			layering[pos.first].erase(layering[pos.first].begin() + pos.second);
			layering[pos.first].emplace_front(entity);
		}
	}
}

/******************************************************************************
*
*	@brief Brings an entity forward one position
*
*	-
*
******************************************************************************/
void LayerOrderBringForward(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != layering[pos.first].size() - 1) {
			std::swap(layering[pos.first][pos.second], layering[pos.first][pos.second + 1]);
		}
	}
}

/******************************************************************************
*
*	@brief Brings an entity to the front of the layer
*
*	-
*
******************************************************************************/
void LayerOrderBringToFront(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != layering[pos.first].size() - 1) {
			layering[pos.first].erase(layering[pos.first].begin() + pos.second);
			layering[pos.first].emplace_back(entity);
			PrepareLayeringForSerialization();
		}
	}
}

/******************************************************************************
*
*	@brief Creates a new layer on top of all other layers
*
*	-
*
******************************************************************************/
void CreateNewLayer() {
	std::deque<Entity> temp;
	layering.emplace_back(temp);
	std::ostringstream oss;
	oss << "Layer " << layerCounter++;
	layerNames.emplace_back(std::make_pair(oss.str(), true));
	selectedLayer = layering.size() - 1;
	UnselectAll();
}

/******************************************************************************
*
*	@brief Deletes the selected layer and destroys all entities in it
*
*	-
*
******************************************************************************/
void DeleteLayer() {
	selectedEntities.clear();
	for (Entity entity : layering[selectedLayer]) {
		selectedEntities.emplace_back(entity);
	}
	toDestroy = true;
	layerNames[selectedLayer].second = false;
	selectedLayer = std::numeric_limits<size_t>::max();
}

/******************************************************************************
*
*	@brief Removes an entity from the layering deque of deques
*
*	Used after destroying an entity
*
******************************************************************************/
void RemoveEntityFromLayering(Entity entity) {
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
			if (layering[layer_it][entity_it] == entity) {
				layering[layer_it].erase(layering[layer_it].begin() + entity_it);
				return;
			}
		}
	}
	return;
}

/******************************************************************************
*
*	@brief Transfers layering data into each entity
*
*	Copies the layering deque of deques into the Name component of each entity
*	for saving
*
******************************************************************************/
void PrepareLayeringForSerialization() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
			if (!ECS::ecs().EntityExists(layering[layer_it][entity_it])) {
				continue;
			}
			Name & n = nameArray.GetData(layering[layer_it][entity_it]);
			n.serializationLayer = layer_it;
			n.serializationOrderInLayer = entity_it;
		}
	}
}

/******************************************************************************
*
*	@brief Rebuilds the layering deque of deques from each entity's data
*
*	Rebuilds the layering deque of deques from the Name component of each
*	entity after loading
*
******************************************************************************/
void RebuildLayeringAfterDeserialization() {
	layering.clear();
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& cloneArray = componentManager.GetComponentArrayRef<Clone>();
	std::set<Entity> * e = &(edit_ptr->m_Entities);
	for (const Entity & entity : *e) {
		if (!cloneArray.HasComponent(entity)) {
			continue;
		}
		Name & n = nameArray.GetData(entity);
		if (n.serializationLayer < layering.size()) {
			if (n.serializationOrderInLayer < layering[n.serializationLayer].size()) {
				layering[n.serializationLayer].insert(layering[n.serializationLayer].begin() + n.serializationOrderInLayer, entity);
			}
			else {
				int i;
				for (i = 0; i < layering[n.serializationLayer].size(); i++) {
					if (nameArray.GetData(layering[n.serializationLayer][i]).serializationOrderInLayer > n.serializationOrderInLayer) {
						layering[n.serializationLayer].insert(layering[n.serializationLayer].begin() + i, entity);
						break;
					}
				}
				if (i >= layering[n.serializationLayer].size()) {
					layering[n.serializationLayer].emplace_back(entity);
				}
			}
		}
		else {
			while (layering.size() <= n.serializationLayer) {
				CreateNewLayer();
			}
 			layering[n.serializationLayer].emplace_back(entity);
		}
	}
}

/******************************************************************************
*
*	@brief Transfers the Visible and Lock settings into each entity
*
*	Embeds the Visible and Lock selection settings into each entity before
*	saving.
*
******************************************************************************/
void EmbedSkipLockForSerialization() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	std::set<Entity>* e = &(s_ptr->m_Entities);
	for (const Entity& entity : *e) {
		Name& n = nameArray.GetData(entity);
		n.skip = entitiesToSkip[static_cast<int>(entity)];
		n.lock = entitiesToLock[static_cast<int>(entity)];
	}
}

/******************************************************************************
*
*	@brief Extracts the Visible and Lock settings from each entity
*
*	Replaces the Visible and Lock selection settings from for each entity after
*	loading.
*
******************************************************************************/
void ExtractSkipLockAfterDeserialization() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	std::set<Entity>* e = &(s_ptr->m_Entities);
	for (const Entity& entity : *e) {
		Name& n = nameArray.GetData(entity);
		entitiesToSkip[static_cast<int>(entity)] = n.skip;
		entitiesToLock[static_cast<int>(entity)] = n.lock;
	}
	std::fill(layersToSkip.begin(), layersToSkip.end(), true);
	std::fill(layersToLock.begin(), layersToLock.end(), true);
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		bool allFalse1 = true;
		for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
			if (entitiesToSkip[layering[layer_it][entity_it]]) {
				allFalse1 = false;
				break;
			}
		}
		if (allFalse1) {
			layersToSkip[layer_it] = false;
		}
		bool allFalse2 = true;
		for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
			if (entitiesToLock[layering[layer_it][entity_it]]) {
				allFalse2 = false;
				break;
			}
		}
		if (allFalse2) {
			layersToLock[layer_it] = false;
		}
	}
}

/******************************************************************************
*
*	@brief Checks if all entities in a layer are set to not visible
* 
*	Checks if all entities in a layer are set to not visible, so that the layer
*	can be set to not visible
*
******************************************************************************/
bool CheckSkipLayerAllTrue(size_t layer_it) {
	for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
		if (entitiesToSkip[layering[layer_it][entity_it]]) {
			return true;
		}
	}
	return false;
}

/******************************************************************************
*
*	@brief Checks if all entities in a layer are set to locked
*
*	Checks if all entities in a layer are set to locked, so that the layer
*	can be set to locked
*
******************************************************************************/
bool CheckLockLayerAllTrue(size_t layer_it) {
	for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
		if (entitiesToLock[layering[layer_it][entity_it]]) {
			return true;
		}
	}
	return false;
}

/******************************************************************************
*
*	@brief Sets all entities in a layer to not visible
*
*	Sets all entities in a layer to not visible if the layer is set to not
*	visible
*
******************************************************************************/
void SetWholeSkipLayer(size_t layer_it) {
	for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
		entitiesToSkip[layering[layer_it][entity_it]] = layersToSkip[layer_it];
	}
}

/******************************************************************************
*
*	@brief Sets all entities in a layer to locked
*
*	Sets all entities in a layer to locked if the layer is set to locked
*
******************************************************************************/
void SetWholeLockLayer(size_t layer_it) {
	for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
		entitiesToLock[layering[layer_it][entity_it]] = layersToLock[layer_it];
	}
}

/******************************************************************************
*
*	@brief Checks whether there is any Entity selected in the layer
*
*	-
*
******************************************************************************/
bool CheckAnySelectedInLayer(size_t layer_it) {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	//std::set<Entity>* e = &(s_ptr->m_Entities);
	for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
		Name& n = nameArray.GetData(layering[layer_it][entity_it]);
		if (n.selected) {
			return true;
		}
	}
	return false;
}

/******************************************************************************
*
*	@brief Returns which is the top-most layer with a selected object
*
*	-
*
******************************************************************************/
size_t GetHightestLayerWithSelection() {
	for (int layer_it = static_cast<int>(layering.size() - 1); layer_it >= 0; --layer_it) {
		if (CheckAnySelectedInLayer(static_cast<size_t>(layer_it))) {
			return static_cast<size_t>(layer_it);
		}
	}
	return std::numeric_limits<size_t>::max();
}

/******************************************************************************
*
*	@brief Moves a specified entity to the top of a specified layer
*
*	-
*
******************************************************************************/
void TransferToLayer(Entity e, size_t LayerIndex) {
	// remove entity from source layer
	RemoveEntityFromLayering(e);
	// insert entity into target layer
	layering[LayerIndex].emplace_back(e);
}

/******************************************************************************
*
*	@brief Returns the Top Most Layer Index
*
*	-
*
******************************************************************************/
size_t GetTopLayer() {
	return layering.size() - 1;
}
