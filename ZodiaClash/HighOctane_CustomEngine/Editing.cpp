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

constexpr float SNAP_SENSITIVITY{ 10.f };
constexpr float SNAP_LINE_EXTENSION{ 100.f };
constexpr float CORNER_SIZE{ 10.f };



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
		float pointAngle{ 0.f };
		bool changeCursor{ false };
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

		case TYPE::MOUSE_UP: {

			if (snappingOn) {
				// clear the maps
				auto& modelArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>();
				for (auto& [e, color] : snappingHighlight) {
					modelArray.GetData(e).GetColorRef() = color;
				}
				snappingHighlight.clear();
				snappingLines.clear();
				centerVertical.clear();
				intersectVertical.clear();
				centerHorizontal.clear();
				intersectHorizontal.clear();
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

							if (snappingOn) {
								//snapping

								auto& modelArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>();

								if (model->GetTop().x == model->GetBot().x/* || model->GetTop().y == model->GetBot().y*/) { // test whether This is vertical/xhorizontalx
									for (size_t layer_it2 = 0; layer_it2 < layering.size(); ++layer_it2) {				// -----------------------------------------------------// Go through layering system.
										if (layersToSkip[layer_it2] && layersToLock[layer_it2]) {																				//
											for (Entity& e : layering[layer_it2]) {																								//
												if (entitiesToSkip[static_cast<uint32_t>(e)] && entitiesToLock[static_cast<uint32_t>(e)] && ECS::ecs().EntityExists(e)) {		//
													if (e == entity) {																											//
														continue;																												//
													}																															//
													Model& mB = modelArray.GetData(e);									// -----------------------------------------------------//
													if (mB.GetTop().x == mB.GetBot().x/* || mB.GetTop().y == mB.GetBot().y*/) { // test whether B is vertical/xhorizontalx

														bool none = true;
														bool vertical = true;
														bool horizontal = true;
														centerVertical.erase(e);
														centerHorizontal.erase(e);
														intersectVertical.erase(e);
														intersectHorizontal.erase(e);

														if (std::abs(mB.GetTop().x - transform.position.x) < SNAP_SENSITIVITY) {
															transform.position.x = mB.GetTop().x;
															snappingHighlight.emplace(e, mB.GetColorRef());
															vmath::Vector2 pt1 = { mB.GetTop().x, mB.GetLeft().y };
															vmath::Vector2 pt2 = { transform.position };
															snappingLines[e][VERTICAL] = std::make_tuple(pt1, pt2, pt1, pt2);
															none = false;
															vertical = false;
															centerVertical.emplace(e);
															intersectVertical.emplace(e);
														}
														else if (std::abs(mB.GetRight().x - (transform.position.x - (model->GetTop().x - model->GetLeft().x))) < SNAP_SENSITIVITY) {
															transform.position.x = mB.GetRight().x + (model->GetTop().x - model->GetLeft().x);
															snappingHighlight.emplace(e, mB.GetColorRef());
															vmath::Vector2 pt1 = { mB.GetRight().x, std::max(mB.GetTop().y, transform.position.y + (model->GetTop().y - model->GetLeft().y)) + SNAP_LINE_EXTENSION };
															vmath::Vector2 pt2 = { mB.GetRight().x, std::min(mB.GetBot().y, transform.position.y - (model->GetLeft().y - model->GetBot().y)) - SNAP_LINE_EXTENSION };
															vmath::Vector2 cpt1 = { mB.GetRight().x, mB.GetRight().y };
															vmath::Vector2 cpt2 = { mB.GetRight().x, transform.position.y };
															snappingLines[e][VERTICAL] = std::make_tuple(pt1, pt2, cpt1, cpt2);
															none = false;
															vertical = false;
															intersectVertical.emplace(e);
														}
														else if (std::abs(mB.GetLeft().x - (transform.position.x + (model->GetRight().x - model->GetTop().x))) < SNAP_SENSITIVITY) {
															transform.position.x = mB.GetLeft().x - (model->GetRight().x - model->GetTop().x);
															snappingHighlight.emplace(e, mB.GetColorRef());
															vmath::Vector2 pt1 = { mB.GetLeft().x, std::max(mB.GetTop().y, transform.position.y + (model->GetTop().y - model->GetLeft().y)) + SNAP_LINE_EXTENSION };
															vmath::Vector2 pt2 = { mB.GetLeft().x, std::min(mB.GetBot().y, transform.position.y - (model->GetLeft().y - model->GetBot().y)) - SNAP_LINE_EXTENSION };
															vmath::Vector2 cpt1 = { mB.GetLeft().x, mB.GetLeft().y };
															vmath::Vector2 cpt2 = { mB.GetLeft().x, transform.position.y };
															snappingLines[e][VERTICAL] = std::make_tuple(pt1, pt2, cpt1, cpt2);
															none = false;
															vertical = false;
															intersectVertical.emplace(e);
														}

														if (std::abs(mB.GetLeft().y - transform.position.y) < SNAP_SENSITIVITY) {
															transform.position.y = mB.GetLeft().y;
															snappingHighlight.emplace(e, mB.GetColorRef());
															vmath::Vector2 pt1 = { mB.GetTop().x, mB.GetLeft().y };
															vmath::Vector2 pt2 = { transform.position };
															snappingLines[e][HORIZONTAL] = std::make_tuple(pt1, pt2, pt1, pt2);
															none = false;
															horizontal = false;
															centerHorizontal.emplace(e);
															intersectHorizontal.emplace(e);
														}
														else if (std::abs(mB.GetTop().y - (transform.position.y - (model->GetLeft().y - model->GetBot().y))) < SNAP_SENSITIVITY) {
															transform.position.y = mB.GetTop().y + (model->GetLeft().y - model->GetBot().y);
															snappingHighlight.emplace(e, mB.GetColorRef());
															vmath::Vector2 pt1 = { std::max(mB.GetRight().x, transform.position.x + (model->GetRight().x - model->GetTop().x)) + SNAP_LINE_EXTENSION, mB.GetTop().y };
															vmath::Vector2 pt2 = { std::min(mB.GetLeft().x, transform.position.x - (model->GetTop().x - model->GetLeft().x)) - SNAP_LINE_EXTENSION, mB.GetTop().y };
															vmath::Vector2 cpt1 = { mB.GetTop().x, mB.GetTop().y };
															vmath::Vector2 cpt2 = { transform.position.x, mB.GetTop().y };
															snappingLines[e][HORIZONTAL] = std::make_tuple(pt1, pt2, cpt1, cpt2);
															none = false;
															horizontal = false;
															intersectHorizontal.emplace(e);
														}
														else if (std::abs(mB.GetBot().y - (transform.position.y + (model->GetTop().y - model->GetLeft().y))) < SNAP_SENSITIVITY) {
															transform.position.y = mB.GetBot().y - (model->GetTop().y - model->GetLeft().y);
															snappingHighlight.emplace(e, mB.GetColorRef());
															vmath::Vector2 pt1 = { std::max(mB.GetRight().x, transform.position.x + (model->GetRight().x - model->GetTop().x)) + SNAP_LINE_EXTENSION, mB.GetBot().y };
															vmath::Vector2 pt2 = { std::min(mB.GetLeft().x, transform.position.x - (model->GetTop().x - model->GetLeft().x)) - SNAP_LINE_EXTENSION, mB.GetBot().y };
															vmath::Vector2 cpt1 = { mB.GetBot().x, mB.GetBot().y };
															vmath::Vector2 cpt2 = { transform.position.x, mB.GetBot().y };
															snappingLines[e][HORIZONTAL] = std::make_tuple(pt1, pt2, cpt1, cpt2);
															none = false;
															horizontal = false;
															intersectHorizontal.emplace(e);
														}

														if (none) {
															if (snappingHighlight.count(e)) {
																mB.GetColorRef() = snappingHighlight[e];
																snappingHighlight.erase(e);
															}
														}

														if (vertical) {
															if (snappingLines.count(e) && snappingLines[e].count(VERTICAL)) {
																snappingLines[e].erase(VERTICAL);
																if (snappingLines[e].empty()) {
																	snappingLines.erase(e);
																}
															}
														}
														if (horizontal) {
															if (snappingLines.count(e) && snappingLines[e].count(HORIZONTAL)) {
																snappingLines[e].erase(HORIZONTAL);
																if (snappingLines[e].empty()) {
																	snappingLines.erase(e);
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}

						break;

					case CLICKED::DOT:
						draggingThisCycle = true;
						transform.rotation = (vmath::PI / 2.f) - (std::atan2(currentMousePosition.y - transform.position.y, currentMousePosition.x - transform.position.x));
						transform.rotation = (transform.rotation > vmath::PI) ? (transform.rotation - (2.f * vmath::PI)) : transform.rotation;

						if (shiftKeyPressed) {
							transform.rotation = std::round(transform.rotation / (vmath::PI / 4.f)) * (vmath::PI / 4.f);
							float multiple = std::round((transform.rotation + 0.002f) / vmath::PI / 4.f) * vmath::PI / 4.f;
							if (std::fabs(transform.rotation - multiple) < 0.001f) {
								transform.rotation = multiple;
							}
							transform.rotation = transform.rotation == -vmath::PI ? vmath::PI : transform.rotation;
						}

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
