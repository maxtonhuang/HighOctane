#include <iostream>
#include "graphics.h"

int main() {
	GraphicsManager::Init();

	while (!GraphicsManager::WindowClosed()) {
		glfwPollEvents();
	}
}
