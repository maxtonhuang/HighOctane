#include "Editing.h"
#include "Components.h"
#include "message.h"
#include "Utilities.h"
#include "model.h"

vmath::Vector2 mousePos{ 0.f, 0.f };

void UpdateProperties (Name & name, Transform & transform, Model & model) {

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

			break;

		case TYPE::MOUSE_UP:
			if (IsWithinObject(model, mousePos)) {
				name.selected = true;
			}
			break;

		case TYPE::MOUSE_DOWN:
			// lock on position of entity
			// get offset
			if (name.selected) {
				transform.position = mousePos - GetOffset(transform.position, mousePos);
			}
			// move entity
			// update position



			break;

		default:
			break;

		}
	}
}