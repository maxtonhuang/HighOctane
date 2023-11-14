#include "Selection.h"
#include "Global.h"
#include "message.h"
#include "Utilities.h"
#include "Layering.h"
#include <limits>


vmath::Vector2 mousePosition{ RESET_VEC2 };

constexpr float CORNER_SIZE = 10.f;

bool shiftKeyPressed{ false };


void Selection(Entity & entity, Name & name, Transform & transform, Model & model, size_t layer_it) {
	clickedThisCycle = false;

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {
		case TYPE::KEY_TRIGGERED:
			if (msg.info == INFO::KEY_RSHIFT || msg.info == INFO::KEY_LSHIFT) {
				shiftKeyPressed = true;
			}
		break;

		case TYPE::KEY_UP:
			if (msg.info == INFO::KEY_RSHIFT || msg.info == INFO::KEY_LSHIFT) {
				shiftKeyPressed = false;
			}
		break;

		case TYPE::MOUSE_MOVE:
			mousePosition = { msg.posX, msg.posY };
			break;

		case TYPE::MOUSE_CLICK: // selection of entity done here
			switch (msg.info) {
			case INFO::MOUSE_LEFT: {
				clickedThisCycle = true; //  <-------------------------------
				if (viewportWindowHovered) {
					if (IsNearby(model.GetMax(), mousePosition, CORNER_SIZE)) {
						ProcessSelection(name, layer_it, CLICKED::NE);
						return;
					}
					else if (IsNearby(model.GetMin(), mousePosition, CORNER_SIZE)) {
						ProcessSelection(name, layer_it, CLICKED::SW);
						return;
					}
					else if (IsNearby({ model.GetMax().x, model.GetMin().y }, mousePosition, CORNER_SIZE)) {
						ProcessSelection(name, layer_it, CLICKED::SE);
						return;
					}
					else if (IsNearby({ model.GetMin().x, model.GetMax().y }, mousePosition, CORNER_SIZE)) {
						ProcessSelection(name, layer_it, CLICKED::NW);
						return;
					}
					else if (IsWithinObject(model, mousePosition)) {
						draggingOffset = GetOffset(transform.position, mousePosition);
						ProcessSelection(name, layer_it, CLICKED::INSIDE);
						return;
						
					}
					/*else {
						if (!popupHovered) {
							rightClick = false;
							name.selected = false;
							currentLayer = selectedLayer = std::numeric_limits<size_t>::max();
							name.clicked = CLICKED::NOT;
						}
					}*/
					if (name.selected) {
						printf("Entity %d is selected on Layer %d\n", static_cast<int>(entity), static_cast<int>(layer_it));
					}
				}
			}
			break;

			case INFO::MOUSE_RIGHT:



				//if (IsWithinObject(model, mousePos)) {
				//	UnselectAll();
				//	name.selected = true;
				//	newSelection = entity;
				//	rightClick = true;
				//	rightClickPos = mousePos;
				//}

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
			Name& name = nameArray.GetData(entity);
			name.selected = false;
			name.clicked = CLICKED::NOT;
		}
	}
	selectedCount = 0;
	selectedLayer = std::numeric_limits<size_t>().max();
	clickedThisCycle = false;
}




void ProcessSelection(Name& name, size_t layer_it, CLICKED corner) {
	if (shiftKeyPressed) {
		if (name.selected) {
			name.selected = false;
			--selectedCount;
			if (CheckAnySelectedInLayer(layer_it)) {
				currentLayer = selectedLayer = layer_it;
			}
			else {
				currentLayer = selectedLayer = GetHightestLayerWithSelection();
			}
		}
		else {
			name.selected = true;
			if (selectedCount == 0) {
				currentLayer = selectedLayer = layer_it;
			}
			++selectedCount;
			name.clicked = corner;
		}
	}
	else {
		UnselectAll();
		name.selected = true;
		++selectedCount;
		currentLayer = selectedLayer = layer_it;
		name.clicked = corner;
	}
}