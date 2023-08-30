#pragma once

#include <glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h>

class GraphicsManager {
public:
	static void Init();
	static void Draw();
private:
	static GLFWwindow* window;
	static int width;
	static int height;
};
