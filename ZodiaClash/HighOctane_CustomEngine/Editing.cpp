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

#define UNREFERENCED_PARAMETER(P) (P)

vmath::Vector2 mousePos{ RESET_VEC2 };
vmath::Vector2 offset{ RESET_VEC2 };

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

		case TYPE::MOUSE_MOVE:
			mousePos = { msg.posX, msg.posY };
			break;

		case TYPE::MOUSE_CLICK: // selection of entity done here
			switch (msg.info) {
				case INFO::MOUSE_LEFT: {
					if (viewportWindowHovered) {
						if (IsNearby(model.GetMax(), mousePos, CORNER_SIZE)) {
							// clear all previous selection
							UnselectAll();
							name.selected = true;
							currentLayer = selectedLayer = layer_it;
							name.clicked = CLICKED::NE;
						}
						else if (IsNearby(model.GetMin(), mousePos, CORNER_SIZE)) {
							UnselectAll();
							name.selected = true;
							currentLayer = selectedLayer = layer_it;
							name.clicked = CLICKED::SW;
						}
						else if (IsNearby({ model.GetMax().x, model.GetMin().y }, mousePos, CORNER_SIZE)) {
							UnselectAll();
							name.selected = true;
							currentLayer = selectedLayer = layer_it;
							name.clicked = CLICKED::SE;
						}
						else if (IsNearby({ model.GetMin().x, model.GetMax().y }, mousePos, CORNER_SIZE)) {
							UnselectAll();
							name.selected = true;
							currentLayer = selectedLayer = layer_it;
							name.clicked = CLICKED::NW;
						}
						else if (IsWithinObject(model, mousePos)) {
							UnselectAll();
							name.selected = true;
							currentLayer = selectedLayer = layer_it;
							name.clicked = CLICKED::INSIDE;
							offset = GetOffset(transform.position, mousePos);
						}
						else {
							if (!popupHovered) {
								rightClick = false;
								name.selected = false;
								currentLayer = selectedLayer = std::numeric_limits<size_t>::max();
								name.clicked = CLICKED::NOT;
							}
						}
						if (name.selected) {
							printf("Entity %d is selected on Layer %d\n", static_cast<int>(entity), static_cast<int>(layer_it));
						}
					}
				}
				break;
				
				case INFO::MOUSE_RIGHT:
					
					if (IsWithinObject(model, mousePos)) {
						UnselectAll();
						name.selected = true;
						newSelection = entity;
						rightClick = true;
						rightClickPos = mousePos;
					}

					break;
			}

			break;

		case TYPE::MOUSE_UP:

			if (!IsWithinObject(model, mousePos) &&
					!IsNearby(model.GetMax(), mousePos, CORNER_SIZE) &&
					!IsNearby(model.GetMin(), mousePos, CORNER_SIZE) &&
					!IsNearby({ model.GetMax().x, model.GetMin().y }, mousePos, CORNER_SIZE) &&
					!IsNearby({ model.GetMax().y, model.GetMin().x }, mousePos, CORNER_SIZE)) {
				name.clicked = CLICKED::NOT;
			}
			else {
				name.clicked = CLICKED::NOT;
			}
			offset = { RESET_VEC2 };

			break;

		case TYPE::MOUSE_DOWN: {

			if (name.selected) {
				switch (name.clicked) {
				case CLICKED::NE:
				{
					float deltaX = mousePos.x - model.GetMax().x;
					float deltaY = mousePos.y - model.GetMax().y;
					float currWidth = model.GetMax().x - model.GetMin().x;
					float currHeight = model.GetMax().y - model.GetMin().y;
					if (deltaX < deltaY) {
						transform.scale *= (currWidth + deltaX) / currWidth;
						transform.position.x += deltaX / 2.f;
						transform.position.y += (deltaX * currHeight / currWidth ) / 2.f;
					}
					else {
						transform.scale *= (currHeight + deltaY) / currHeight;
						transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
						transform.position.y += deltaY / 2.f;
					}
				}
					break;
				case CLICKED::SW:
				{
					float deltaX = model.GetMin().x - mousePos.x;
					float deltaY = model.GetMin().y - mousePos.y;
					float currWidth = model.GetMax().x - model.GetMin().x;
					float currHeight = model.GetMax().y - model.GetMin().y;
					if (deltaX < deltaY) {
						transform.scale *= (currWidth + deltaX) / currWidth;
						transform.position.x -= deltaX / 2.f;
						transform.position.y -= (deltaX * currHeight / currWidth) / 2.f;
					}
					else {
						transform.scale *= (currHeight + deltaY) / currHeight;
						transform.position.x -= (deltaY * currWidth / currHeight) / 2.f;
						transform.position.y -= deltaY / 2.f;
					}
				}
					break;
				case CLICKED::NW:
				{
					float deltaX = model.GetMin().x - mousePos.x;
					float deltaY = mousePos.y - model.GetMax().y;
					float currWidth = model.GetMax().x - model.GetMin().x;
					float currHeight = model.GetMax().y - model.GetMin().y;
					if (deltaX < deltaY) {
						transform.scale *= (currWidth + deltaX) / currWidth;
						transform.position.x -= deltaX / 2.f;
						transform.position.y += (deltaX * currHeight / currWidth) / 2.f;
					}
					else {
						transform.scale *= (currHeight + deltaY) / currHeight;
						transform.position.x -= (deltaY * currWidth / currHeight) / 2.f;
						transform.position.y += deltaY / 2.f;
					}
				}
					break;
				case CLICKED::SE:
				{
					float deltaX = mousePos.x - model.GetMax().x;
					float deltaY = model.GetMin().y - mousePos.y;
					float currWidth = model.GetMax().x - model.GetMin().x;
					float currHeight = model.GetMax().y - model.GetMin().y;
					if (deltaX < deltaY) {
						transform.scale *= (currWidth + deltaX) / currWidth;
						transform.position.x += deltaX / 2.f;
						transform.position.y -= (deltaX * currHeight / currWidth) / 2.f;
					}
					else {
						transform.scale *= (currHeight + deltaY) / currHeight;
						transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
						transform.position.y -= deltaY / 2.f;
					}
				}
					break;
				case CLICKED::INSIDE:
					transform.position.x = mousePos.x - offset.x;
					transform.position.y = mousePos.y - offset.y;
					break;
				default:
					break;
				}
			}
			break;
		}
		default:
			break;

		}
	}
	
	if (name.selected) {

		if (name.clicked == CLICKED::NE || name.clicked == CLICKED::SW || (IsNearby(model.GetMax(), mousePos, CORNER_SIZE) || IsNearby(model.GetMin(), mousePos, CORNER_SIZE))) {
			SetCursor(hNESWCursor);
		}
		else if (name.clicked == CLICKED::NW || name.clicked == CLICKED::SE || IsNearby({ model.GetMax().x, model.GetMin().y }, mousePos, CORNER_SIZE) || IsNearby({ model.GetMin().x, model.GetMax().y }, mousePos, CORNER_SIZE)) {
			SetCursor(hNWSECursor);
		}
		else if (IsWithinObject(model, mousePos)) {
			SetCursor(hAllDirCursor);
		}
		else {
			SetCursor(hDefaultCursor);
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
			nameArray.GetData(entity).selected = false;
		}
	}
}

