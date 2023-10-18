#include "model.h"
#include "Utilities.h"

bool IsWithinObject(Model & model, vmath::Vector2 cursorPos) {
	if (cursorPos.x < model.GetMin().x || cursorPos.x > model.GetMax().x || cursorPos.y < model.GetMin().y || cursorPos.y > model.GetMax().y) {
		return false;
	}
	return true;
}