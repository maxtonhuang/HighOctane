#include <iostream>
#include "graphics.h"

int main() {
	GraphicsManager::Init();

	while (1) {
		glfwPollEvents();
	}
}
