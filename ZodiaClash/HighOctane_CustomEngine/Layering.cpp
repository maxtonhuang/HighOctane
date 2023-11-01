#include "Layering.h"
#include "Global.h"
#include "Editing.h"
#include <sstream>
#include <algorithm>




std::pair<size_t, size_t> FindInLayer(Entity entity) {
	for (size_t i = 1; i < layering.size(); ++i) {
		for (size_t j = 0; j < layering[i].size(); ++j) {
			if (layering[i][j] == entity) {
				return { i, j };
			}
		}
	}
	return { ULLONG_MAX, ULLONG_MAX }; // if not found
}


void LayerOrderSendBackwards(Entity entity) {
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
	
	//clearAllSelection = true;



	//std::cout << "Layering Size: " << layering.size() << std::endl;
	//int count = 0;
	//for (auto& val : layering) {
	//	std::cout << "Layering[" << count++ << "] size: " << val.size() << std::endl;
	//}
	//for (std::string val : layerNames) {
	//	std::cout << "Layer Names: " << val << std::endl;
	//}
	//std::cout << "----------" << std::endl;


}

void DeleteLayer() {
	std::cout << "Layering Size: " << layering[selectedLayer].size() << std::endl;
	for (auto& entity : layering[selectedLayer]) {
		selectedEntities.emplace_back(entity);
		std::cout << "Marked for destruction: " << entity << std::endl;
	}
	std::cout << "---" << std::endl;
	
	for (Entity entity : selectedEntities) {
		std::cout << "Destroying entity: " << entity << std::endl;
		EntityFactory::entityFactory().DeleteCloneModel(entity);
	}
	toDestroy = false;
	selectedEntities.clear();
	
	layering.erase(layering.begin() + selectedLayer);
	layerNames.erase(layerNames.begin() + selectedLayer);
}