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
*	@file		Input.cpp
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
*	This file contains the buttons to handle entities within the system.
*	Such as loading and saving entitites/gameobject from a file
******************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Input.h"
#include "ZodiaClash.h"
#include "Message.h"
#include "DebugDiagnostic.h"
#include "AudioManager.h"
#include "EntityFactory.h"
#include <iostream>
#include <unordered_map>


std::unordered_map<int, INFO> keyStatus;

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
*/
void InputManager::KeyCallback(GLFWwindow* pwin, int key, int scancode, int action, int mod) {
    mod = mod; //unused variable
    scancode = scancode; //unused variable
    switch (action) {
    case GLFW_PRESS:
        keyStatus[key] = static_cast<INFO>(key);
        Mail::mail().CreatePostcard(TYPE::KEY_TRIGGERED, ADDRESS::INPUT, static_cast<INFO>(key), 0.f, 0.f);
        if (GLFW_KEY_ESCAPE == key) {
            glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        }
        if (GLFW_KEY_M == key) {
            audio.PlaySounds("../Assets/Sound/ping.wav");
        }
        if (GLFW_KEY_N == key) {
            audio.PlaySounds("../Assets/Sound/bonk.wav");
        }

        // key input for toggling mass rendering
        if (GLFW_KEY_Y == key) {
            static bool pressed = false;
            static bool created = false;
            if (!pressed) {
                if (!created) {
                    std::vector<const char*> spritesheets;
                    spritesheets.push_back("duck.png");
                    spritesheets.push_back("duck2.png");
                    LoadModels(2500, false, spritesheets);
                    // after initial creation of models, to stay true throughout runtime
                    created = true;
                }
                else {
                    ReapplyMassRendering();
                }
                pressed = true;
            }
            else {
                RemoveMassRendering();
                pressed = false;
            }
        }
        break;
    case GLFW_RELEASE:
        keyStatus[key] = INFO::NONE;
        Mail::mail().CreatePostcard(TYPE::KEY_UP, ADDRESS::INPUT, static_cast<INFO>(key), 0.f, 0.f);
        break;
    default:
        break;
    }
}

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
*/
void InputManager::MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod) {
    mod = mod; //unused variable
    pwin = pwin; // unused variable;
    if (GLFW_PRESS == action) {
        /*if (GLFW_KEY_ESCAPE == key) {
            glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        }*/
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            Mail::mail().CreatePostcard(TYPE::MOUSE_CLICK, ADDRESS::INPUT, INFO::MOUSE_LEFT, 0.f, 0.f);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            Mail::mail().CreatePostcard(TYPE::MOUSE_CLICK, ADDRESS::INPUT, INFO::MOUSE_RIGHT, 0.f, 0.f);
            break;
        }
    }
}

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
*/
void InputManager::CursorPosCallback(GLFWwindow* pwin, double xpos, double ypos) {
    pwin = pwin; //unused variable

    static int previousPosX = 0;
    static int previousPosY = 0;
    
    int currPosX = static_cast<int>(static_cast<float>(xpos) - GRAPHICS::w);
    int currPosY = static_cast<int>(-static_cast<float>(ypos) + GRAPHICS::h);
    
    if (currPosX != previousPosX || currPosY != previousPosY) {
        Mail::mail().CreatePostcard(TYPE::MOUSE_MOVE, ADDRESS::INPUT, INFO::NONE, static_cast<float>(currPosX), static_cast<float>(currPosY));
    }

    previousPosX = currPosX;
    previousPosY = currPosY;
    //cursorposX = ;
    //cursorposY = -static_cast<float>(ypos) + GRAPHICS::h;

    //test_model.SetPos(cursorposX, cursorposY);
}

void InputManager::KeyCheck() {
    for (std::pair<int, INFO> val : keyStatus) {
        if (val.second != INFO::NONE) {
            Mail::mail().CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, val.second, 0.f, 0.f);
        }
    }
}
