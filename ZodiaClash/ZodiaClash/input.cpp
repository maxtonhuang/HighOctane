
/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Input.h"
#include "ZodiaClash.h"
#include "Message.h"
#include "DebugDiagnostic.h"
#include "AudioManager.h"
#include <iostream>
#include <unordered_map>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
//GLboolean InputManager::keyStateW{ GL_FALSE };
//GLboolean InputManager::keyStateA{ GL_FALSE };
//GLboolean InputManager::keyStateS{ GL_FALSE };
//GLboolean InputManager::keyStateD{ GL_FALSE };
//GLboolean InputManager::mouseClickL{ GL_FALSE };
//GLboolean InputManager::mouseClickR{ GL_FALSE };
//double InputManager::cursorposX;
//double InputManager::cursorposY;

std::unordered_map<int, INFO> keyStatus;

extern Mail mail;

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
    switch (action) {
    case GLFW_PRESS:
        keyStatus[key] = static_cast<INFO>(key);
        mail.CreatePostcard(TYPE::KEY_TRIGGERED, ADDRESS::INPUT, static_cast<INFO>(key), 0.f, 0.f);
        if (GLFW_KEY_ESCAPE == key) {
            glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        }
        if (GLFW_KEY_M == key) {
            audio.PlaySounds("../Assets/Sound/ping.wav");
        }
        break;
    case GLFW_RELEASE:
        keyStatus[key] = INFO::NONE;
        mail.CreatePostcard(TYPE::KEY_UP, ADDRESS::INPUT, static_cast<INFO>(key), 0.f, 0.f);
        break;
    default:
        break;
    }
}

void InputManager::KeyCheck() {
    for (std::pair<int,INFO> val : keyStatus) {
        if (val.second != INFO::NONE) {
            mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, val.second, 0.f, 0.f);
        }
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
    if (GLFW_PRESS == action) {
        /*if (GLFW_KEY_ESCAPE == key) {
            glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        }*/
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            mail.CreatePostcard(TYPE::MOUSE_CLICK, ADDRESS::INPUT, INFO::MOUSE_LEFT, 0.f, 0.f);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mail.CreatePostcard(TYPE::MOUSE_CLICK, ADDRESS::INPUT, INFO::MOUSE_RIGHT, 0.f, 0.f);
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
    
    mail.CreatePostcard(TYPE::MOUSE_MOVE, ADDRESS::INPUT, INFO::NONE, (static_cast<float>(xpos) - GRAPHICS::w), (-static_cast<float>(ypos) + GRAPHICS::h));
    
    //cursorposX = ;
    //cursorposY = -static_cast<float>(ypos) + GRAPHICS::h;

    //test_model.SetPos(cursorposX, cursorposY);
}

