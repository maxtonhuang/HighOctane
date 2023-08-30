#pragma once

#include <glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h>

class InputManager {
public:
	static void KeyCallback(GLFWwindow* pwin, int key, int scancode, int action, int mod);
	static void MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod);
	static void CursorPosCallback(GLFWwindow* pwin, double xpos, double ypos);
private:

};
