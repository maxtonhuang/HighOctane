#include "Editing.h"
#include "Components.h"
#include "message.h"
#include "Utilities.h"
#include "model.h"
#include "Global.h"
#include "debugdiagnostic.h"
#include "DebugProfile.h"
#include <algorithm>

vmath::Vector2 mousePos{ RESET_VEC2 };
vmath::Vector2 offset{ RESET_VEC2 };
CLICKED clicked{ CLICKED::NOT };

constexpr float CORNER_SIZE = 10.f;

void UpdateProperties (Name & name, Transform & transform, Model & model, Size & size) {
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {

		case TYPE::KEY_DOWN:

			//switch (msg.info) {
			//case INFO::KEY_L:   ;   break;
			//default: break;
			//}
			break;

		case TYPE::MOUSE_MOVE:
			mousePos = { msg.posX, msg.posY };
			break;

		case TYPE::MOUSE_CLICK:
			if (IsNearby(model.GetMax(), mousePos, CORNER_SIZE)) {
				name.selected = true;
				clicked = CLICKED::NE;
			}
			else if (IsNearby(model.GetMin(), mousePos, CORNER_SIZE)) {
				name.selected = true;
				clicked = CLICKED::SW;
			}
			else if (IsNearby({ model.GetMax().x, model.GetMin().y }, mousePos, CORNER_SIZE)) {
				name.selected = true;
				clicked = CLICKED::SE;
			}
			else if (IsNearby({ model.GetMin().x, model.GetMax().y }, mousePos, CORNER_SIZE)) {
				name.selected = true;
				clicked = CLICKED::NW;
			}
			else if (IsWithinObject(model, mousePos)) {
				name.selected = true;
				clicked = CLICKED::INSIDE;
				offset = GetOffset(transform.position, mousePos);
			}
			else {
				name.selected = false;
				clicked = CLICKED::NOT;
			}
			
			break;

		case TYPE::MOUSE_UP:
			if (!IsWithinObject(model, mousePos) &&
					!IsNearby(model.GetMax(), mousePos, CORNER_SIZE) &&
					!IsNearby(model.GetMin(), mousePos, CORNER_SIZE) &&
					!IsNearby({ model.GetMax().x, model.GetMin().y }, mousePos, CORNER_SIZE) &&
					!IsNearby({ model.GetMax().y, model.GetMin().x }, mousePos, CORNER_SIZE)) {
				clicked = CLICKED::NOT;
			}
			else {
				clicked = CLICKED::NOT;
			}
			offset = { RESET_VEC2 };
			break;

		case TYPE::MOUSE_DOWN: {
			if (name.selected) {
				switch (clicked) {
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
					transform.position.x = std::clamp(mousePos.x - offset.x, -(GRAPHICS::defaultWidthF / 2.f) + (transform.position.x - model.GetMin().x), (GRAPHICS::defaultWidthF / 2.f) - (model.GetMax().x - transform.position.x));
					transform.position.y = std::clamp(mousePos.y - offset.y, -(GRAPHICS::defaultHeightF / 2.f) + (transform.position.y - model.GetMin().y), (GRAPHICS::defaultHeightF / 2.f) - (model.GetMax().y - transform.position.y));
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
		if (clicked == CLICKED::NE || clicked == CLICKED::SW || (IsNearby(model.GetMax(), mousePos, CORNER_SIZE) || IsNearby(model.GetMin(), mousePos, CORNER_SIZE))) {
			SetCursor(hNESWCursor);
		}
		else if (clicked == CLICKED::NW || clicked == CLICKED::SE || IsNearby({ model.GetMax().x, model.GetMin().y }, mousePos, CORNER_SIZE) || IsNearby({ model.GetMin().x, model.GetMax().y }, mousePos, CORNER_SIZE)) {
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