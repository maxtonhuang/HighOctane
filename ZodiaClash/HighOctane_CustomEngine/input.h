/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		Input.h
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		4 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains the declarations of Inputs from keyboard and mouse
******************************************************************************/
#pragma once

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <string>
#include "Graphics.h"
#include "Message.h"

class InputManager {
	public:
		//static bool InputInit(GLint w, GLint h, std::string t);
/*  _________________________________________________________________________*/
/*! KeyCallback

@param GLFWwindow*
Handle to window that is receiving event

@param int
the keyboard key that was pressed or released

@parm int
Platform-specific scancode of the key

@parm int
GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
action will be GLFW_KEY_UNKNOWN if GLFW lacks a key token for it,
for example E-mail and Play keys.

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when keyboard buttons are pressed.
When the ESC key is pressed, the close flag of the window is set.
******************************************************************************/
		static void KeyCallback(GLFWwindow* pwin, int key, int scancode, int action, int mod);
		static void KeyCheck();
		//static INFO KeyConversion(int key);
/*  _________________________________________________________________________*/
/*! MouseButtonCallback

@param GLFWwindow*
Handle to window that is receiving event

@param int
the mouse button that was pressed or released
GLFW_MOUSE_BUTTON_LEFT and GLFW_MOUSE_BUTTON_RIGHT specifying left and right
mouse buttons are most useful

@parm int
action is either GLFW_PRESS or GLFW_RELEASE

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when mouse buttons are pressed.
****************************************************************************/
		static void MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod);
		static void MouseCheck();
/*  _________________________________________________________________________*/
/*! CursorPosCallback

@param GLFWwindow*
Handle to window that is receiving event

@param double
new cursor x-coordinate, relative to the left edge of the client area

@param double
new cursor y-coordinate, relative to the top edge of the client area

@return none

This functions receives the cursor position, measured in screen coordinates but
relative to the top-left corner of the window client area.
*****************************************************************************/
		static void CursorPosCallback(GLFWwindow* pwin, double xpos, double ypos);

		static void WindowFocusCallback(GLFWwindow* pwin, int focused);
	private:
		static double cursorposX;
		static double cursorposY;
};
