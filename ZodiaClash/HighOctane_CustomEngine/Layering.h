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

void PrepareLayeringForSerialization();
void RebuildLayeringAfterDeserialization();

void EmbedSkipLockForSerialization();
void ExtractSkipLockAfterDeserialization();
bool CheckSkipLayerAllTrue(size_t layer_it);
bool CheckLockLayerAllTrue(size_t layer_it);

void SetWholeSkipLayer(size_t layer_it);
void SetWholeLockLayer(size_t layer_it);