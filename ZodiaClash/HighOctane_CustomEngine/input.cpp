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
*	This file contains the definitions of Inputs from keyboard and mouse
******************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Input.h"
#include "Message.h"
#include "DebugDiagnostic.h"
#include "AudioManager.h"
#include "EntityFactory.h"
#include <iostream>
#include <unordered_map>
#include "AssetManager.h"

#define UNREFERENCED_PARAMETER(P) (P)

std::unordered_map<int, INFO> keyStatus;
std::unordered_map<int, INFO> mouseStatus;


void InputManager::KeyCallback(GLFWwindow* pwin, int key, int scancode, int action, int mod) {
    UNREFERENCED_PARAMETER(mod); //unused variable
    UNREFERENCED_PARAMETER(scancode); //unused variable
    switch (action) {
    case GLFW_PRESS:

        keyStatus[key] = static_cast<INFO>(key);
        Mail::mail().CreatePostcard(TYPE::KEY_TRIGGERED, ADDRESS::INPUT, static_cast<INFO>(key), 0.f, 0.f);

        if (GLFW_KEY_ESCAPE == key) {
            glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        }
        if (GLFW_KEY_F11 == key) {
            static bool fullscreen = false;
            if (fullscreen) {
                fullscreen = false;
            }
            else {
                fullscreen = true;
            }
            graphics.Fullscreen(fullscreen);
        }

        // key input for toggling mass rendering
        //if (GLFW_KEY_Y == key) {
        //    static bool pressed = false;
        //    static bool created = false;
        //    if (!pressed) {
        //        if (!created) {
        //            std::vector<const char*> spritesheets;
        //            spritesheets.push_back("duck.png");
        //            spritesheets.push_back("duck2.png");
        //            EntityFactory::entityFactory().LoadModels(2500, false, spritesheets);
        //            // after initial creation of models, to stay true throughout runtime
        //            created = true;
        //        }
        //        else {
        //            EntityFactory::entityFactory().ReapplyMassRendering();
        //        }
        //        pressed = true;
        //    }
        //    else {
        //        EntityFactory::entityFactory().RemoveMassRendering();
        //        pressed = false;
        //    }
        //}
        break;
    case GLFW_RELEASE:
        keyStatus[key] = INFO::NONE;
        Mail::mail().CreatePostcard(TYPE::KEY_UP, ADDRESS::INPUT, static_cast<INFO>(key), 0.f, 0.f);
        break;
    default:
        break;
    }
}


void InputManager::KeyCheck() {
    for (std::pair<int, INFO> val : keyStatus) {
        if (val.second != INFO::NONE) {
            Mail::mail().CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, val.second, 0.f, 0.f);
        }
    }
}


void InputManager::MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod) {
    UNREFERENCED_PARAMETER(mod); //unused variable
    UNREFERENCED_PARAMETER(pwin); // unused variable;
    switch (action) {
    case GLFW_PRESS:
        mouseStatus[button] = static_cast<INFO>(button);
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            Mail::mail().CreatePostcard(TYPE::MOUSE_CLICK, ADDRESS::INPUT, INFO::MOUSE_LEFT, 0.f, 0.f);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            Mail::mail().CreatePostcard(TYPE::MOUSE_CLICK, ADDRESS::INPUT, INFO::MOUSE_RIGHT, 0.f, 0.f);
            break;
        }
        break;
    case GLFW_RELEASE:
        mouseStatus[button] = INFO::NONE;
        Mail::mail().CreatePostcard(TYPE::MOUSE_UP, ADDRESS::INPUT, static_cast<INFO>(button), 0.f, 0.f);
        break;
    default:
        break;
    }
}


void InputManager::CursorPosCallback(GLFWwindow* pwin, double xpos, double ypos) {
    UNREFERENCED_PARAMETER(pwin); //unused variable

    static int previousPosX = 0;
    static int previousPosY = 0;

    ypos = (double)graphics.GetWindowHeight() - ypos;

    xpos = (xpos - graphics.viewport.GetX()) / graphics.viewport.GetW() * GRAPHICS::defaultWidthF;
    ypos = (ypos - graphics.viewport.GetY()) / graphics.viewport.GetH() * GRAPHICS::defaultHeightF;

    int currPosX = static_cast<int>(static_cast<float>(xpos) - GRAPHICS::w);
    int currPosY = static_cast<int>(static_cast<float>(ypos) - GRAPHICS::h);

    if (currPosX != previousPosX || currPosY != previousPosY) {
        Mail::mail().CreatePostcard(TYPE::MOUSE_MOVE, ADDRESS::INPUT, INFO::NONE, static_cast<float>(currPosX), static_cast<float>(currPosY));
    }

    previousPosX = currPosX;
    previousPosY = currPosY;
}

void InputManager::MouseCheck() {
    for (std::pair<int, INFO> val : mouseStatus) {
        if (val.second != INFO::NONE) {
            Mail::mail().CreatePostcard(TYPE::MOUSE_DOWN, ADDRESS::INPUT, val.second, 0.f, 0.f);
        }
    }
}