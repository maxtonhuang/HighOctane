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
*	This file contains functions for moving individual entities around
*	(dragging), resizing them, and rotating them. It also contains functions
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
#include "vmath.h"
#include <algorithm>
#include <limits>
#include "UndoRedo.h"

#define UNREFERENCED_PARAMETER(P) (P)

constexpr float CORNER_SIZE{ 10.f };
float pointAngle{ 0.f };
bool changeCursor{ false };

/******************************************************************************
*
*	@brief Updates the editing functions for entities
*
*	Includes entity movement, resizing, and rotation.
*
******************************************************************************/
void UpdateProperties (Entity & entity, Name & name, Transform & transform, Size & size, Model * model, size_t layer_it) {
	
	UNREFERENCED_PARAMETER(entity);
	UNREFERENCED_PARAMETER(layer_it);

	if (viewportWindowHovered && !popupHovered && name.selected && !draggingThisCycle) {
		changeCursor = false;
		if (model == nullptr) {
			if (transform.position.distance(currentMousePosition) < GRAPHICS::DEBUG_CIRCLE_RADIUS) {
				SetCursor(hAllDirCursor);
			}
			else {
				SetCursor(hDefaultCursor);
			}
			cursorEditingTooltipState = CursorEditingTooltip::NONE;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetTop(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation;
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::SIDE;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetRight(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation + (vmath::PI / 2.f);
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::SIDE;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetBot(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation + vmath::PI;
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::SIDE;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetLeft(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation + (3 * vmath::PI / 2.f);
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::SIDE;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetTopRight(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation + (vmath::PI / 4.f);
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::CORNER;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetBotLeft(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation + (5 * vmath::PI / 4.f);
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::CORNER;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetBotRight(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation + (3 * vmath::PI / 4.f);
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::CORNER;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetTopLeft(), currentMousePosition, CORNER_SIZE)) {
			pointAngle = transform.rotation + (7 * vmath::PI / 4.f);
			changeCursor = true;
			cursorEditingTooltipState = CursorEditingTooltip::CORNER;
		}
		else if (selectedEntities.size() == 1 && IsNearby(model->GetRotPoint(), currentMousePosition, CORNER_SIZE * 3.f)) {
			SetCursor(hHandCursor);
			cursorEditingTooltipState = CursorEditingTooltip::NONE;
		}
		else if (IsWithinObject(*model, currentMousePosition)) {
			SetCursor(hAllDirCursor);
			cursorEditingTooltipState = CursorEditingTooltip::NONE;
		}
		else {
			SetCursor(hDefaultCursor);
			cursorEditingTooltipState = CursorEditingTooltip::NONE;
		}

		pointAngle = (pointAngle > vmath::PI) ? (pointAngle - (2.f * vmath::PI)) : pointAngle;

		if (changeCursor) {
		
			if (pointAngle < - (7 * vmath::PI / 8)) {
				SetCursor(hNSCursor);
			}
			else if (pointAngle < -(5 * vmath::PI / 8)) {
				SetCursor(hNESWCursor);
			}
			else if (pointAngle < -(3 * vmath::PI / 8)) {
				SetCursor(hEWCursor);
			}
			else if (pointAngle < -(vmath::PI / 8)) {
				SetCursor(hNWSECursor);
			}
			else if (pointAngle < (vmath::PI / 8)) {
				SetCursor(hNSCursor);
			}
			else if (pointAngle < (3 * vmath::PI / 8)) {
				SetCursor(hNESWCursor);
			}
			else if (pointAngle < (5 * vmath::PI / 8)) {
				SetCursor(hEWCursor);
			}
			else if (pointAngle < (7 * vmath::PI / 8)) {
				SetCursor(hNWSECursor);
			}
			else {
				SetCursor(hNSCursor);
			}
		
		}
	}
	
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {

		case TYPE::KEY_DOWN:

			switch (msg.info) {

			case INFO::KEY_DEL:
				toDestroy = true;
				break;

			case INFO::KEY_UP:
				if (name.selected) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					transform.position.y += shiftKeyPressed ? 10 : 1;
				}
				break;

			case INFO::KEY_DOWN:
				if (name.selected) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					transform.position.y -= shiftKeyPressed ? 10 : 1;
				}
				break;

			case INFO::KEY_RIGHT:
				if (name.selected) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					transform.position.x += shiftKeyPressed ? 10 : 1;
				}
				break;

			case INFO::KEY_LEFT:
				if (name.selected) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					transform.position.x -= shiftKeyPressed ? 10 : 1;
				}
				break;

			default: break;

			}

			break;

		case TYPE::MOUSE_CLICK: {

			if (name.selected) {
				if (model == nullptr) {
					if (transform.position.distance(currentMousePosition) < GRAPHICS::DEBUG_CIRCLE_RADIUS) {
						name.clicked = CLICKED::INSIDE;
					}
				}
				else if (IsNearby(model->GetTop(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::N;
				}
				else if (IsNearby(model->GetRight(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::E;
				}
				else if (IsNearby(model->GetBot(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::S;
				}
				else if (IsNearby(model->GetLeft(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::W;
				}
				else if (IsNearby(model->GetTopRight(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::NE;
				}
				else if (IsNearby(model->GetBotLeft(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::SW;
				}
				else if (IsNearby(model->GetBotRight(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::SE;
				}
				else if (IsNearby(model->GetTopLeft(), currentMousePosition, CORNER_SIZE)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::NW;
				}
				else if (IsWithinObject(*model, currentMousePosition)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::INSIDE;
				}
				else if (IsNearby(model->GetRotPoint(), currentMousePosition, CORNER_SIZE * 3.f)) {
					undoRedo.RecordCurrent(entity, ACTION::TRANSFORM);
					name.clicked = CLICKED::DOT;
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
					switch (name.clicked) {

					case CLICKED::N:
					{
						draggingThisCycle = true;
						if (mouseMoved) {

							size.height *= (vmath::Vector2::DistanceBetweenPoints(vmath::Vector2::ProjectedPointOnLine(model->GetTop(), transform.position, currentMousePosition), transform.position) / vmath::Vector2::DistanceBetweenPoints(model->GetTop(), transform.position));

						}
					}
					break;
					
					case CLICKED::E:
					{
						draggingThisCycle = true;
						if (mouseMoved) {

							size.width *= (vmath::Vector2::DistanceBetweenPoints(vmath::Vector2::ProjectedPointOnLine(model->GetRight(), transform.position, currentMousePosition), transform.position) / vmath::Vector2::DistanceBetweenPoints(model->GetRight(), transform.position));

						}
					}
					break;
					
					case CLICKED::S:
					{
						draggingThisCycle = true;
						if (mouseMoved) {

							size.height *= (vmath::Vector2::DistanceBetweenPoints(vmath::Vector2::ProjectedPointOnLine(model->GetBot(), transform.position, currentMousePosition), transform.position) / vmath::Vector2::DistanceBetweenPoints(model->GetBot(), transform.position));

						}
					}
					break;
					
					case CLICKED::W:
					{
						draggingThisCycle = true;
						if (mouseMoved) {

							size.width *= (vmath::Vector2::DistanceBetweenPoints(vmath::Vector2::ProjectedPointOnLine(model->GetLeft(), transform.position, currentMousePosition), transform.position) / vmath::Vector2::DistanceBetweenPoints(model->GetRight(), transform.position));

						}
					}
					break;

					case CLICKED::NE:
					{
						draggingThisCycle = true;
						if (mouseMoved) {
							float deltaX = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetTopRight(), model->GetBotRight(), currentMousePosition));
							float deltaY = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetTopRight(), model->GetTopLeft(), currentMousePosition));
							
							deltaX = vmath::Vector2::IsPointOutside(model->GetTopRight(), model->GetBotRight(), currentMousePosition) ? deltaX : -deltaX;
							deltaY = vmath::Vector2::IsPointOutside(model->GetTopRight(), model->GetTopLeft(), currentMousePosition) ? -deltaY : deltaY;
							
							float currWidth = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetTopLeft());
							float currHeight = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetBotRight());
							
							if (deltaX < deltaY) {
								transform.scale = std::clamp( transform.scale * ((currWidth + deltaX) / currWidth), 0.1f, 10.f);
								//transform.position.x += deltaX / 2.f;
								//transform.position.y += (deltaX * currHeight / currWidth) / 2.f;
							}
							else {
								transform.scale = std::clamp( transform.scale * ((currHeight + deltaY) / currHeight), 0.1f, 10.f);
								//transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
								//transform.position.y += deltaY / 2.f;
							}
						}
					}
					break;

					case CLICKED::SW:
					{
						draggingThisCycle = true;
						if (mouseMoved) {
							float deltaX = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetBotLeft(), model->GetTopLeft(), currentMousePosition));
							float deltaY = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetBotLeft(), model->GetBotRight(), currentMousePosition));

							deltaX = vmath::Vector2::IsPointOutside(model->GetBotLeft(), model->GetTopLeft(), currentMousePosition) ? deltaX : -deltaX;
							deltaY = vmath::Vector2::IsPointOutside(model->GetBotLeft(), model->GetBotRight(), currentMousePosition) ? -deltaY : deltaY;

							float currWidth = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetTopLeft());
							float currHeight = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetBotRight());

							if (deltaX < deltaY) {
								transform.scale = std::clamp(transform.scale * ((currWidth + deltaX) / currWidth), 0.1f, 10.f);
								//transform.position.x += deltaX / 2.f;
								//transform.position.y += (deltaX * currHeight / currWidth) / 2.f;
							}
							else {
								transform.scale = std::clamp(transform.scale * ((currHeight + deltaY) / currHeight), 0.1f, 10.f);
								//transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
								//transform.position.y += deltaY / 2.f;
							}
						}
					}

					break;

					case CLICKED::NW:
					{
						draggingThisCycle = true;
						if (mouseMoved) {
							float deltaX = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetTopLeft(), model->GetBotLeft(), currentMousePosition));
							float deltaY = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetTopLeft(), model->GetTopRight(), currentMousePosition));

							deltaX = vmath::Vector2::IsPointOutside(model->GetTopLeft(), model->GetBotLeft(), currentMousePosition) ? -deltaX : deltaX;
							deltaY = vmath::Vector2::IsPointOutside(model->GetTopLeft(), model->GetTopRight(), currentMousePosition) ? deltaY : -deltaY;

							float currWidth = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetTopLeft());
							float currHeight = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetBotRight());

							if (deltaX < deltaY) {
								transform.scale = std::clamp(transform.scale * ((currWidth + deltaX) / currWidth), 0.1f, 10.f);
								//transform.position.x += deltaX / 2.f;
								//transform.position.y += (deltaX * currHeight / currWidth) / 2.f;
							}
							else {
								transform.scale = std::clamp(transform.scale * ((currHeight + deltaY) / currHeight), 0.1f, 10.f);
								//transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
								//transform.position.y += deltaY / 2.f;
							}
						}
					}

					break;

					case CLICKED::SE:
					{
						draggingThisCycle = true;
						if (mouseMoved) {
							float deltaX = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetBotRight(), model->GetTopRight(), currentMousePosition));
							float deltaY = vmath::Vector2::DistanceBetweenPoints(currentMousePosition, vmath::Vector2::ProjectedPointOnLine(model->GetBotRight(), model->GetBotLeft(), currentMousePosition));

							deltaX = vmath::Vector2::IsPointOutside(model->GetBotRight(), model->GetTopRight(), currentMousePosition) ? -deltaX : deltaX;
							deltaY = vmath::Vector2::IsPointOutside(model->GetBotRight(), model->GetBotLeft(), currentMousePosition) ? deltaY : -deltaY;

							float currWidth = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetTopLeft());
							float currHeight = vmath::Vector2::DistanceBetweenPoints(model->GetTopRight(), model->GetBotRight());

							if (deltaX < deltaY) {
								transform.scale = std::clamp(transform.scale * ((currWidth + deltaX) / currWidth), 0.1f, 10.f);
								//transform.position.x += deltaX / 2.f;
								//transform.position.y += (deltaX * currHeight / currWidth) / 2.f;
							}
							else {
								transform.scale = std::clamp(transform.scale * ((currHeight + deltaY) / currHeight), 0.1f, 10.f);
								//transform.position.x += (deltaY * currWidth / currHeight) / 2.f;
								//transform.position.y += deltaY / 2.f;
							}
						}
					}
					break;
					
					case CLICKED::INSIDE:
						if (withinSomething) {
							draggingThisCycle = true;
							transform.position.x = currentMousePosition.x - name.draggingOffset.x;
							transform.position.y = currentMousePosition.y - name.draggingOffset.y;
						}
						break;
					
					case CLICKED::DOT:
						draggingThisCycle = true;
						transform.rotation = (vmath::PI / 2.f) - (atan2(currentMousePosition.y - transform.position.y, currentMousePosition.x - transform.position.x));
						transform.rotation = (transform.rotation > vmath::PI) ? (transform.rotation - (2.f * vmath::PI)) : transform.rotation;
						break;

					default:
						break;
					}
				}
				break;

			default:
				if (name.selected && withinSomething) {
					draggingThisCycle = true;
					transform.position.x = currentMousePosition.x - name.draggingOffset.x;
					transform.position.y = currentMousePosition.y - name.draggingOffset.y;
				}
				break;
			}
		}	break;

		default:
			break;
		}
	}
}
