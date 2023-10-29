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

vmath::Vector2 mousePos{ RESET_VEC2 };
vmath::Vector2 offset{ RESET_VEC2 };

constexpr float CORNER_SIZE = 10.f;

void UpdateProperties (Entity & entity, Name & name, Transform & transform, Model & model) {
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {

		case TYPE::KEY_DOWN:

			switch (msg.info) {

			case INFO::KEY_BACKSPACE:
				if (name.selected) {
					selectedEntities.emplace_back(entity);
					toDestroy = true;
				}
				//EntityFactory::entityFactory().DeleteCloneModel(entity);
				break;

			case INFO::KEY_DEL:   
				if (name.selected) {
					selectedEntities.emplace_back(entity);
					toDestroy = true;
				}
				//EntityFactory::entityFactory().DeleteCloneModel(entity);
				break;

			default: break;
			
			}
			
			break;

		case TYPE::MOUSE_MOVE:
			mousePos = { msg.posX, msg.posY };
			break;

		case TYPE::MOUSE_CLICK:
			switch (msg.info) {
				case INFO::MOUSE_LEFT: {
					
					if (IsNearby(model.GetMax(), mousePos, CORNER_SIZE)) {
						name.selected = true;
						name.clicked = CLICKED::NE;
					}
					else if (IsNearby(model.GetMin(), mousePos, CORNER_SIZE)) {
						name.selected = true;
						name.clicked = CLICKED::SW;
					}
					else if (IsNearby({ model.GetMax().x, model.GetMin().y }, mousePos, CORNER_SIZE)) {
						name.selected = true;
						name.clicked = CLICKED::SE;
					}
					else if (IsNearby({ model.GetMin().x, model.GetMax().y }, mousePos, CORNER_SIZE)) {
						name.selected = true;
						name.clicked = CLICKED::NW;
					}
					else if (IsWithinObject(model, mousePos)) {
						name.selected = true;
						name.clicked = CLICKED::INSIDE;
						offset = GetOffset(transform.position, mousePos);
					}
					else {
						if (!popupHovered) {
							rightClick = false;
							name.selected = false;
							name.clicked = CLICKED::NOT;
						}
					}
				}
				break;
				case INFO::MOUSE_RIGHT:
					name.selected = false;
					//rightClick = false;
					if (IsWithinObject(model, mousePos)) {
						name.selected = true;
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
					transform.position.x = std::clamp(mousePos.x - offset.x, -(GRAPHICS::w)+(transform.position.x - model.GetMin().x), (GRAPHICS::w)-(model.GetMax().x - transform.position.x));
					transform.position.y = std::clamp(mousePos.y - offset.y, -(GRAPHICS::h)+(transform.position.y - model.GetMin().y), (GRAPHICS::h)-(model.GetMax().y - transform.position.y));
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
		anyObjectSelected = true;
		selectedEntities.emplace_back(entity);
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



