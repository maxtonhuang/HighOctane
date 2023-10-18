#include "model.h"
#include "Utilities.h"
#include "vmath.h"

bool IsWithinObject(Model & model, vmath::Vector2 cursorPos) {
	std::cout << "Cursor Pos: " << cursorPos.x << " , " << cursorPos.y << std::endl;
	std::cout << "Model Min: " << model.GetMin().x << " , " << model.GetMin().y << std::endl;
	std::cout << "Model Max: " << model.GetMax().x << " , " << model.GetMax().y << std::endl;

	if (cursorPos.x < model.GetMin().x || cursorPos.x > model.GetMax().x || cursorPos.y < model.GetMin().y || cursorPos.y > model.GetMax().y) {
		return false;
	}
	return true;
}

// offset of Point 2 from Point 1
vmath::Vector2 GetOffset(vmath::Vector2 point1, vmath::Vector2 point2) {
	return { point2.x - point1.x, point2.y - point1.y };
}
