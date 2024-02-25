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
*	@file		Selection.cpp
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
*	This file contains the selection system to handle selection of game
*	entities for editing. This includes multi-selected of game entities,
*	setting key objects, grouping objects, etc.
*
******************************************************************************/

#include "Selection.h"
#include "Global.h"
#include "message.h"
#include "Utilities.h"
#include "Layering.h"
#include "ECS.h"
#include "model.h"
#include <limits>


constexpr float CORNER_SIZE = 10.f;


/******************************************************************************
*
*	@brief Handles mouse input for selection of entities
*
*	-
*
******************************************************************************/
void Selection(Entity & entity, Name & name, Transform & transform, Model & model, size_t layer_it) {
	thereWasAClickThisCycle = false;

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {

		case TYPE::MOUSE_CLICK: {
			switch (msg.info) {
			case INFO::MOUSE_RIGHT:
				thereWasAClickThisCycle = true;
				if (&model != nullptr) {
					if (IsWithinObject(model, currentMousePosition)) {
						if (!name.selected) {
							ProcessSelection(name, layer_it);
						}
						somethingWasSelectedThisCycle = true;
						rightClick = true;
						rightClickPos = currentMousePosition;
					}
				}
				break;
			}
		}
			break;

		case TYPE::MOUSE_UP: // selection of entity done here for Left Click
			switch (msg.info) {
			case INFO::MOUSE_LEFT: {
				if (!draggingThisCycle) {
					if (viewportWindowHovered) {
						thereWasAClickThisCycle = true;
						if (&model != nullptr) {
							if (IsNearby(model.GetMax(), currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsNearby(model.GetMin(), currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsNearby({ model.GetMax().x, model.GetMin().y }, currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsNearby({ model.GetMin().x, model.GetMax().y }, currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsWithinObject(model, currentMousePosition)) {
									if (name.selected) {
										if (keyObjectID != entity) {
											// set key object
											if (keyObjectID != std::numeric_limits<Entity>().max()) {
												ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>().GetData(keyObjectID).GetColorRef() = keyObjectColor;
											}
											keyObjectID = entity;
											keyObjectColor = model.GetColorRef();
										}
										else { // unset key object
											model.GetColorRef() = keyObjectColor;
											keyObjectID = std::numeric_limits<Entity>().max();
											keyObjectColor = glm::vec4(RESET_VEC4);
										}
									}
									else {
									ProcessSelection(name, layer_it);
								}
								somethingWasSelectedThisCycle = true;
								return;
							}
							if (name.selected) {
							}
						}
						else {
							//Selection for attach points
							if (transform.position.distance(currentMousePosition) < GRAPHICS::DEBUG_CIRCLE_RADIUS) {
								ProcessSelection(name, layer_it);
								somethingWasSelectedThisCycle = true;
								return;
							}
						}
					}
				}
			}
			break;

			default:
				break;
			}
		}
	}
}

/******************************************************************************
*
*	@brief Unselects all entities
*
*	-
*
******************************************************************************/
void UnselectAll() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	for (auto& layer : layering) {
		for (auto& entity : layer) {
			if (!ECS::ecs().EntityExists(entity)) {
				continue;
			}
			Name& name = nameArray.GetData(entity);
			name.selected = false;
			name.clicked = CLICKED::NONE;
		}
	}
	selectedCount = 0;
	selectedLayer = std::numeric_limits<size_t>().max();
	thereWasAClickThisCycle = false;
	somethingWasSelectedThisCycle = false;
}

/******************************************************************************
*
*	@brief Processes the actual selection
*
*	Processes the selection, including checking whether the entity is part of
*	a group, and handles it accordingly.
*
******************************************************************************/
void ProcessSelection(Name& name, size_t layer_it) {
	if (shiftKeyPressed) {
		if (name.selected) {
			if (name.group) {
				UnselectWholeGroup(name.group);
			}
			else {
				name.selected = false;
				--selectedCount;
			}
			
			if (CheckAnySelectedInLayer(layer_it)) {
				selectedLayer = layer_it;
			}
			else {
				selectedLayer = GetHightestLayerWithSelection();
			}


		}
		else {
			if (name.group) {
				SelectWholeGroup(name.group);
				selectedLayer = GetHightestLayerWithSelection();
			}
			else {
				name.selected = true;
				++selectedCount;
			}
		}
	}
	else {
		UnselectAll();
		if (name.group) {
			SelectWholeGroup(name.group);
			selectedLayer = GetHightestLayerWithSelection();
		}
		else {
			name.selected = true;
			++selectedCount;
			selectedLayer = layer_it;
		}
	}
}

/******************************************************************************
*
*	@brief Selects the whole group
*
*	Selects all entities in the entire group.
*
******************************************************************************/
void SelectWholeGroup(size_t groupNumber) {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	for (auto& layer : layering) {
		for (auto& entity : layer) {
			Name& name = nameArray.GetData(entity);
			if (name.group == groupNumber) {
				name.selected = true;
				++selectedCount;
			}
		}
	}
}

/******************************************************************************
*
*	@brief Unselects the whole group
*
*	Unselects all entities in the entire group.
*
******************************************************************************/
void UnselectWholeGroup(size_t groupNumber) {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	for (auto& layer : layering) {
		for (auto& entity : layer) {
			Name& name = nameArray.GetData(entity);
			if (name.group == groupNumber) {
				name.selected = false;
				--selectedCount;
			}
		}
	}
}

/******************************************************************************
*
*	@brief Groups currently selected entities together
*
*	Creates a new group for all the currently selected entities.
*
******************************************************************************/
void GroupSelection() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	for (auto& layer : layering) {
		for (auto& entity : layer) {
			Name& name = nameArray.GetData(entity);
			if (name.selected && name.group) {
				std::cout << "Object already belongs to another group. Please ungroup first." << std::endl;
				return;
			}
		}
	}
	++groupCounter;
	for (auto& layer : layering) {
		for (auto& entity : layer) {
			Name& name = nameArray.GetData(entity);
			if (name.selected) {
				std::cout << groupCounter << std::endl;
				name.group = groupCounter;
			}
		}
	}
}

/******************************************************************************
*
*	@brief Ungroups all groups that are currently selected
*
*	-
*
******************************************************************************/
void UngroupSelection() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	for (auto& layer : layering) {
		for (auto& entity : layer) {
			Name& name = nameArray.GetData(entity);
			if (name.selected) {
				name.group = 0;
			}
		}
	}
}