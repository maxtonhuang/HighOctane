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
*	@file		Layering.h
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
*	@brief		Function declarations to functions in Layering.cpp
*
*	-
*
******************************************************************************/


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

bool CheckAnySelectedInLayer(size_t layer_it);
size_t GetHightestLayerWithSelection();
void TransferToLayer(Entity e, size_t LayerIndex);