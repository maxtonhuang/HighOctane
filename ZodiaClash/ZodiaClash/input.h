#pragma once

/*                                                                   includes
----------------------------------------------------------------------------- */
//#include <glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h>
#include <string>
#include "Graphics.h"
//#include "Model.h"

class InputManager {
	public:
		static bool InputInit(GLint w, GLint h, std::string t);
		static void KeyCallback(GLFWwindow* pwin, int key, int scancode, int action, int mod);
		static void MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod);
		static void CursorPosCallback(GLFWwindow* pwin, double xpos, double ypos);

		static GLboolean keyStateW;
		static GLboolean keyStateA;
		static GLboolean keyStateS;
		static GLboolean keyStateD;
		static GLboolean mouseClickL;
		static GLboolean mouseClickR;
	private:
		static double cursorposX;
		static double cursorposY;
};
