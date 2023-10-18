#include "Editing.h"
#include "Components.h"
#include "message.h"
#include "Utilities.h"
#include "model.h"
#include <algorithm>

vmath::Vector2 mousePos{ 0.f, 0.f };
vmath::Vector2 offset{ 0.f, 0.f };

void UpdateProperties (Name & name, Transform & transform, Model & model) {
//	std::cout << "UpdateProperties" << std::endl;
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
			if (IsWithinObject(model, mousePos)) {
				name.selected = true;
			}
			offset = GetOffset(transform.position, mousePos);
			break;

		case TYPE::MOUSE_UP:
			if (!IsWithinObject(model, mousePos)) {
				name.selected = false;
			}
			offset = { 0.f, 0.f };
			break;

		case TYPE::MOUSE_DOWN:
			
			if (name.selected) {
				transform.position.x = std::clamp(mousePos.x - offset.x, -(GRAPHICS::defaultWidthF / 2.f) + (transform.position.x - model.GetMin().x), (GRAPHICS::defaultWidthF / 2.f) - (model.GetMax().x - transform.position.x));
				transform.position.y = std::clamp(mousePos.y - offset.y, -(GRAPHICS::defaultHeightF / 2.f) + (transform.position.y - model.GetMin().y), (GRAPHICS::defaultHeightF / 2.f) - (model.GetMax().y - transform.position.y));
			}
			break;

		default:
			break;

		}
	}
}