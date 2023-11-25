#include "Selection.h"
#include "Global.h"
#include "message.h"
#include "Utilities.h"
#include "Layering.h"
#include "ECS.h"
#include "model.h"
#include <limits>


//vmath::Vector2 mousePosition{ RESET_VEC2 };

constexpr float CORNER_SIZE = 10.f;




void Selection(Entity & entity, Name & name, Transform & transform, Model & model, size_t layer_it) {
	thereWasAClickThisCycle = false;

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {

		case TYPE::MOUSE_CLICK: {
			switch (msg.info) {
			case INFO::MOUSE_RIGHT:
				thereWasAClickThisCycle = true;
				//if (viewportWindowHovered) {
				printf("Right Click Detected\n");
				if (&model != nullptr) {
					if (IsWithinObject(model, currentMousePosition)) {
						//UnselectAll();
						//name.selected = true;
						if (!name.selected) {
							ProcessSelection(name, layer_it/*, CLICKED::SE*/); // <-----------------------
						}
						//newSelection = entity;

						somethingWasSelectedThisCycle = true;
						rightClick = true;
						rightClickPos = currentMousePosition;
						//printf("Selected Count: %d\n", selectedCount);
					}
					//}
				}

				break;
			}
		}
			break;

		case TYPE::MOUSE_UP: // selection of entity done here << --- needs a DRAGGED bool to check if it was dragged or not
			switch (msg.info) {
			case INFO::MOUSE_LEFT: {
				//printf("%d\n", draggingThisCycle);
				if (!draggingThisCycle) {
					if (viewportWindowHovered) {
						thereWasAClickThisCycle = true;
						if (&model != nullptr) {
							if (IsNearby(model.GetMax(), currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it/*, CLICKED::NE*/);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsNearby(model.GetMin(), currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it/*, CLICKED::SW*/);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsNearby({ model.GetMax().x, model.GetMin().y }, currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it/*, CLICKED::SE*/);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsNearby({ model.GetMin().x, model.GetMax().y }, currentMousePosition, CORNER_SIZE)) {
								ProcessSelection(name, layer_it/*, CLICKED::NW*/);
								somethingWasSelectedThisCycle = true;
								return;
							}
							else if (IsWithinObject(model, currentMousePosition)) {
									//printf("Within Object");
									if (name.selected) {
										if (keyObjectID != entity) {
											// set key object
											if (keyObjectID != std::numeric_limits<Entity>().max()) {
												ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>().GetData(keyObjectID).GetColorRef() = { 1.f, 1.f, 1.f, 1.f };
											}
											keyObjectID = entity;
										}
										else {
											model.GetColorRef() = { 1.f, 1.f, 1.f, 1.f };
											keyObjectID = std::numeric_limits<Entity>().max();
										}
									}
									else {
									ProcessSelection(name, layer_it/*, CLICKED::INSIDE*/);
								}
								somethingWasSelectedThisCycle = true;
								return;
							}
							/*else {
								if (!popupHovered) {
									rightClick = false;
									name.selected = false;
									selectedLayer = std::numeric_limits<size_t>::max();
									name.clicked = CLICKED::NOT;
								}
							}*/
							if (name.selected) {
								printf("Entity %d is selected on Layer %d\n", static_cast<int>(entity), static_cast<int>(layer_it));
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



				// change color for key object

			}
			break;

			//case INFO::MOUSE_RIGHT:
			//	thereWasAClickThisCycle = true;
			//	//if (viewportWindowHovered) {
			//	printf("Right Click Detected\n");
			//		if (IsWithinObject(model, currentMousePosition)) {
			//			//UnselectAll();
			//			//name.selected = true;
			//			if (!name.selected) {
			//				ProcessSelection(name, layer_it/*, CLICKED::SE*/); // <-----------------------
			//			}
			//			//newSelection = entity;
			//			
			//			somethingWasSelectedThisCycle = true;
			//			rightClick = true;
			//			rightClickPos = currentMousePosition;
			//			//printf("Selected Count: %d\n", selectedCount);
			//		}
			//	//}

			//break;


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
	//auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	for (auto& layer : layering) {
		for (auto& entity : layer) {
			if (!ECS::ecs().EntityExists(entity)) {
				continue;
			}
			Name& name = nameArray.GetData(entity);
			name.selected = false;
			name.clicked = CLICKED::NONE;
			//modelArray.GetData(entity).GetColorRef() = { 1.f, 1.f, 1.f, 1.f };
		}
	}
	selectedCount = 0;
	selectedLayer = std::numeric_limits<size_t>().max();
	thereWasAClickThisCycle = false;
	somethingWasSelectedThisCycle = false;
	//keyObjectID = std::numeric_limits<Entity>().max();
}




void ProcessSelection(Name& name, size_t layer_it/*, CLICKED location*/) {
	if (shiftKeyPressed) {
		if (name.selected) {
			if (name.group) {
				UnselectWholeGroup(name.group);
				//selectedLayer = GetHightestLayerWithSelection();
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
			/*if (selectedCount == 0) {
				selectedLayer = layer_it;
			}*/
			
			//name.clicked = location;
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
			printf("%s\n", name.name.c_str());
			selectedLayer = layer_it;
		}
		
		//name.clicked = location;
	}
}

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
	std::cout << "Group " << groupCounter << " created." << std::endl;
	
	//selectedCount = 0;
	//selectedLayer = std::numeric_limits<size_t>().max();
	//thereWasAClickThisCycle = false;
	//somethingWasSelectedThisCycle = false;
}


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