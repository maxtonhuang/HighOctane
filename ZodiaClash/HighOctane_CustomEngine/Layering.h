#pragma once

#include "EntityFactory.h"



std::pair<size_t, size_t> FindInLayer(Entity entity);
void LayerOrderSendBackward(Entity entity);
void LayerOrderSendToBack(Entity entity);
void LayerOrderBringForward(Entity entity);
void LayerOrderBringToFront(Entity entity);

void CreateNewLayer();
void DeleteLayer();
void RemoveEntityFromLayering(Entity entity);