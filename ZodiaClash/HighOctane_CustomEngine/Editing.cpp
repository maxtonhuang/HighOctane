#include "Editing.h"
#include "Components.h"
#include "message.h"
#include "Utilities.h"
#include "model.h"

vmath::Vector2 mousePos{ 0.f, 0.f };
vmath::Vector2 offset{ 0.f,0.f };

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
			//transform.position = { msg.posX, msg.posY };
			break;

		case TYPE::MOUSE_CLICK:
			//std::cout << "Clicked" << std::endl;
			offset = GetOffset(transform.position, mousePos);
			break;

		case TYPE::MOUSE_UP:
			//std::cout << "MUP" << std::endl;
			if (!IsWithinObject(model, mousePos)) {
				name.selected = false;
			}
			offset = { 0.f,0.f };
			break;

		case TYPE::MOUSE_DOWN:
			//std::cout << "MDOWN" << std::endl;
			// lock on position of entity
			// get offset
			if (IsWithinObject(model, mousePos)) {
				name.selected = true;
			}
			if (name.selected) {
				std::cout << "Transform Pos: " << transform.position.x << " , " << transform.position.y << std::endl;
				std::cout << "Mouse Pos: " << mousePos.x << " , " << mousePos.y << std::endl;
				std::cout << "Offset: " << GetOffset(transform.position, mousePos).x << " , " << GetOffset(transform.position, mousePos).y << std::endl;
				transform.position = mousePos - offset;
			}
			// move entity
			// update position



			break;

		default:
			break;

		}
	}
}