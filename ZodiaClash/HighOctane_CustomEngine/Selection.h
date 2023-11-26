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
*	@file		Selection.h
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
*	@brief		Handles selection of game entities for editing
*
*	This file contains the declarations for the functions used by the
*	selection system to handle selection of game entities for editing. This
*	includes multi-selected of game entities, setting key objects, grouping
*	objects, etc.
*
******************************************************************************/

#pragma once

#include <cstddef>
#include "Components.h"
#include "model.h"

void Selection(Entity& entity, Name& name, Transform& transform, Model& model, size_t layer_it);

void UnselectAll();

void ProcessSelection(Name& name, size_t layer_it);

void GroupSelection();

void UngroupSelection();

void SelectWholeGroup(size_t groupNumber);

void UnselectWholeGroup(size_t groupNumber);
