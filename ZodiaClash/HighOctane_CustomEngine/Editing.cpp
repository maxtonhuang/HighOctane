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
*	@file		Editing.cpp
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
*	@brief		Functions for handling editing functions for entities
*
*	This file contains functions for selecting and unselecting individual
*	entities, moving them around, and resizing them. It also contains functions
*	for setting the correct cursor for the mouse when hovering over an entity.
*
******************************************************************************/


#include "Editing.h"
#include "Components.h"
#include "message.h"
#include "Utilities.h"
#include "model.h"
#include "Global.h"
#include "debugdiagnostic.h"
#include "DebugProfile.h"
#include "EntityFactory.h"
#include <algorithm>
#include <limits>
#include "UndoRedo.h"

#define UNREFERENCED_PARAMETER(P) (P)

//vmath::Vector2 e_currMousePos{ RESET_VEC2 };
//vmath::Vector2 e_prevMousePos{ RESET_VEC2 };
//vmath::Vector2 e_mousePosDelta{ RESET_VEC2 };

constexpr float CORNER_SIZE = 10.f;


/******************************************************************************
*
*	@brief Updates the editing functions for entities
*
*	Includes entity selection, movement, and resizing.
*
******************************************************************************/
void UpdateProperties (Entity & entity, Name & name, Transform & transform, Model & model, size_t layer_it) {
	
	UNREFERENCED_PARAMETER(entity);
	UNREFERENCED_PARAMETER(layer_it);

	//e_mousePosDelta = { RESET_VEC2 };

	/*name.draggingOffset = transform.position - currentMousePosition;*/


	if (!popupHovered && name.selected) {
		if (&model == nullptr) {
			if (transform.position.distance(currentMousePosition) < GRAPHICS::DEBUG_CIRCLE_RADIUS) {
				SetCursor(hAllDirCursor);
			}
			else {
				SetCursor(hDefaultCursor);
			}
		}
		else if (selectedEntities.size() == 1 && IsNearby(model.GetMax(), currentMousePosition, CORNER_SIZE)) {
			//name.clicked == CLICKED::NE;
			SetCursor(hNESWCursor);
		}
		else if (selectedEntities.size() == 1 && IsNearby(model.GetMin(), currentMousePosition, CORNER_SIZE)) {
			//name.clicked == CLICKED::SW;
			SetCursor(hNESWCursor);
		}
		else if (selectedEntities.size() == 1 && IsNearby({ model.GetMax().x, model.GetMin().y }, currentMousePosition, CORNER_SIZE)) {
			//name.clicked == CLICKED::SE;
			SetCursor(hNWSECursor);
		}
		else if (selectedEntities.size() == 1 && IsNearby({ model.GetMin().x, model.GetMax().y }, currentMousePosition, CORNER_SIZE)) {
			//name.clicked == CLICKED::NW;
			SetCursor(hNWSECursor);
		}
		else if (IsWithinObject(model, currentMousePosition)) {
			//name.clicked == CLICKED::INSIDE;
			SetCursor(hAllDirCursor);
		}
		else { // <--- check this.
			SetCursor(hDefaultCursor);
		}
	}





	
	
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {

		case TYPE::KEY_DOWN:

			switch (msg.info) {

			case INFO::KEY_DEL:
				toDestroy = true;
				break;

			default: break;

			}

			break;

		case TYPE::MOUSE_CLICK: {

			if (name.selected) {
				if (&model == nullptr) {
					if (transform.position.distance(currentMousePosition) < GRAPHICS::DEBUG_CIRCLE_RADIUS) {
						name.clicked = CLICKED::INSIDE;
						printf("INSIDE ------");
					}
				}
				else if (IsNearby(model.GetMax(), currentMousePosition, CORNER_SIZE)) {
					name.clicked = CLICKED::NE;
					printf("NE ------");
				}
				else if (IsNearby(model.GetMin(), currentMousePosition, CORNER_SIZE)) {
					name.clicked = CLICKED::SW;
					printf("SW ------");
				}
				else if (IsNearby({ model.GetMax().x, model.GetMin().y }, currentMousePosition, CORNER_SIZE)) {
					name.clicked = CLICKED::SE;
					printf("SE ------");
				}
				else if (IsNearby({ model.GetMin().x, model.GetMax().y }, currentMousePosition, CORNER_SIZE)) {
					name.clicked = CLICKED::NW;
					printf("NW ------");
				}
				else if (IsWithinObject(model, currentMousePosition)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::INSIDE;
					//printf("INSIDE ------");
				}
				else if (IsNearby(model.GetRotPoint(), currentMousePosition, CORNER_SIZE*3.f)) {
					name.clicked = CLICKED::DOT;
					//printf("ROTATE ------");
				}
				else {
					name.clicked = CLICKED::NONE;
				}
			}


		}	break;

		case TYPE::MOUSE_DOWN: {

			switch (selectedEntities.size()) {

			case 1:

				if (name.selected) {
					//printf("Corner: %d\n ", name.clicked);
					switch (name.clicked) {
					case CLICKED::NE:
					{
						draggingThisCycle = true;
						//printf("Mouse Position: %f, %f\n", currentMousePosition.x, currentMousePosition.y);
						//printf("Model Position: %f, %f\n", model.GetMax().x, model.GetMax().y);
						float deltaX = currentMousePosition.x - model.GetMax().x;
						float deltaY = currentMousePosition.y - model.GetMax().y;
						//printf("Delta: %f, %f\n", deltaX, deltaY);
						float currWidth = model.GetMax().x - model.GetMin().x;
						float currHeight = model.GetMax().y - model.GetMin().y;
						if (deltaX < deltaY) {
							transform.scale *= (currWidth + deltaX) / currWidth;
							//transform.position.x += deltaX / 2.f;
							//transform.position.y += (deltaX * currHeight / currWidth) / 2.f;
						}
						else {
							transform.scale *= (currHeight + deltaY) / currHeight;
							//transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
							//transform.position.y += deltaY / 2.f;
						}
					}
					break;
					case CLICKED::SW:
					{
						draggingThisCycle = true;
						float deltaX = model.GetMin().x - currentMousePosition.x;
						float deltaY = model.GetMin().y - currentMousePosition.y;
						float currWidth = model.GetMax().x - model.GetMin().x;
						float currHeight = model.GetMax().y - model.GetMin().y;
						if (deltaX < deltaY) {
							transform.scale *= (currWidth + deltaX) / currWidth;
							//transform.position.x -= deltaX / 2.f;
							//transform.position.y -= (deltaX * currHeight / currWidth) / 2.f;
						}
						else {
							transform.scale *= (currHeight + deltaY) / currHeight;
							//transform.position.x -= (deltaY * currWidth / currHeight) / 2.f;
							//transform.position.y -= deltaY / 2.f;
						}
					}
					break;
					case CLICKED::NW:
					{
						draggingThisCycle = true;
						float deltaX = model.GetMin().x - currentMousePosition.x;
						float deltaY = currentMousePosition.y - model.GetMax().y;
						float currWidth = model.GetMax().x - model.GetMin().x;
						float currHeight = model.GetMax().y - model.GetMin().y;
						if (deltaX < deltaY) {
							transform.scale *= (currWidth + deltaX) / currWidth;
							//transform.position.x -= deltaX / 2.f;
							//transform.position.y += (deltaX * currHeight / currWidth) / 2.f;
						}
						else {
							transform.scale *= (currHeight + deltaY) / currHeight;
							//transform.position.x -= (deltaY * currWidth / currHeight) / 2.f;
							//transform.position.y += deltaY / 2.f;
						}
					}
					break;
					case CLICKED::SE:
					{
						draggingThisCycle = true;
						float deltaX = currentMousePosition.x - model.GetMax().x;
						float deltaY = model.GetMin().y - currentMousePosition.y;
						float currWidth = model.GetMax().x - model.GetMin().x;
						float currHeight = model.GetMax().y - model.GetMin().y;
						if (deltaX < deltaY) {
							transform.scale *= (currWidth + deltaX) / currWidth;
							//transform.position.x += deltaX / 2.f;
							//transform.position.y -= (deltaX * currHeight / currWidth) / 2.f;
						}
						else {
							transform.scale *= (currHeight + deltaY) / currHeight;
							//transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
							//transform.position.y -= deltaY / 2.f;
						}
					}
					break;
					case CLICKED::INSIDE:
						if (withinSomething) {
							draggingThisCycle = true;
							//printf("%f, %f\n", e_mousePosDelta.x, e_mousePosDelta.y);
							transform.position.x = currentMousePosition.x - name.draggingOffset.x;
							transform.position.y = currentMousePosition.y - name.draggingOffset.y;
						}
						break;
					case CLICKED::DOT:
						//if (withinSomething) {
							draggingThisCycle = true;
							//printf("%f, %f\n", e_mousePosDelta.x, e_mousePosDelta.y);
							transform.rotation = (vmath::PI / 2.f) - (atan2(currentMousePosition.y - transform.position.y, currentMousePosition.x - transform.position.x)) /** 180.f / vmath::PI*/;
						//}
						break;
					default:
						break;
					}
				}

				break;

			default:

				if (name.selected && withinSomething) {
					draggingThisCycle = true;
					//switch (name.clicked) {

					//case CLICKED::INSIDE:
						//printf("HERE\n");
					transform.position.x = currentMousePosition.x - name.draggingOffset.x;
					transform.position.y = currentMousePosition.y - name.draggingOffset.y;
					//break;
				//default:
					//break;
				//}
				}

				break;









			}
		}	break;



		default:
			break;

		}
	}
	
}


