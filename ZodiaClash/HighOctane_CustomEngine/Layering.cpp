#include "Layering.h"
#include "Global.h"
#include "Editing.h"
#include "ECS.h"
#include <sstream>
#include <algorithm>
#include <limits>




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


void LayerOrderSendBackward(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != 0) {
			std::swap(layering[pos.first][pos.second], layering[pos.first][pos.second - 1]);
		}
	}
}


void LayerOrderSendToBack(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != 0) {
			layering[pos.first].erase(layering[pos.first].begin() + pos.second);
			layering[pos.first].emplace_front(entity);
		}
	}
}


void LayerOrderBringForward(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != layering[pos.first].size() - 1) {
			std::swap(layering[pos.first][pos.second], layering[pos.first][pos.second + 1]);
		}
	}
}


void LayerOrderBringToFront(Entity entity) {
	std::pair<size_t, size_t> pos = FindInLayer(entity);
	if (pos.first != ULLONG_MAX && pos.second != ULLONG_MAX) {
		if (pos.second != layering[pos.first].size() - 1) {
			layering[pos.first].erase(layering[pos.first].begin() + pos.second);
			layering[pos.first].emplace_back(entity);
		}
	}
}


void CreateNewLayer() {
	std::deque<Entity> temp;
	layering.emplace_back(temp);
	std::ostringstream oss;
	oss << "Layer " << layerCounter++;
	layerNames.emplace_back(oss.str());
	currentLayer = selectedLayer = layering.size() - 1;
	UnselectAll();
}

void DeleteLayer() {
	for (Entity entity : layering[selectedLayer]) {
		std::cout << "Destroying entity: " << entity << std::endl;
		EntityFactory::entityFactory().DeleteCloneModel(entity);
	}
	toDestroy = false;
	selectedEntities.clear();
	layering.erase(layering.begin() + selectedLayer);
	layerNames.erase(layerNames.begin() + selectedLayer);
	currentLayer = selectedLayer = std::numeric_limits<size_t>::max();
}

void RemoveEntityFromLayering(Entity entity) {
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
			if (layering[layer_it][entity_it] == entity) {
				layering[layer_it].erase(layering[layer_it].begin() + entity_it);
				return;
			}
		}
	}
	printf("ENTITY CANNOT BE FOUND IN LAYERING!\n");
	return;
}

void PrepareLayeringForSerialization() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
			Name & n = nameArray.GetData(layering[layer_it][entity_it]);
			n.serializationLayer = layer_it;
			n.serializationOrderInLayer = entity_it;
		}
	}
}

void RebuildLayeringAfterDeserialization() {
	layering.clear();
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	std::set<Entity> * e = &(s_ptr->m_Entities);
	for (const Entity & entity : *e) {
		Name & n = nameArray.GetData(entity);
		if (n.serializationLayer < layering.size()) {
			if (n.serializationOrderInLayer < layering[n.serializationLayer].size()) {
				layering[n.serializationLayer].insert(layering[n.serializationLayer].begin() + n.serializationOrderInLayer, entity);
			}
			else {
				layering[n.serializationLayer].emplace_back(entity);
			}
		}
		else {
			while (layering.size() < n.serializationLayer) {
				std::deque<Entity> temp;
				layering.emplace_back(temp);
			}
			layering[n.serializationLayer].emplace_back(entity);
		}
	}
}





