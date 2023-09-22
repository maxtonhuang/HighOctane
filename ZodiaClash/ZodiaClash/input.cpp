
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
GLboolean InputManager::keyStateW{ GL_FALSE };
GLboolean InputManager::keyStateA{ GL_FALSE };
GLboolean InputManager::keyStateS{ GL_FALSE };
GLboolean InputManager::keyStateD{ GL_FALSE };
GLboolean InputManager::mouseClickL{ GL_FALSE };
GLboolean InputManager::mouseClickR{ GL_FALSE };
double InputManager::cursorposX;
double InputManager::cursorposY;

std::unordered_map<int, INFO> keyStatus;

extern Mail mail;


//bool InputManager::InputInit(GLint w, GLint h, std::string t) {
//    InputManager::width = w;
//    InputManager::height = h;
//    InputManager::title = t;
//
//    if (!glfwInit()) {
//        std::cout << "GLFW init has failed - abort program!!!" << std::endl;
//        return false;
//    }
//
//    InputManager::ptr_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
//    if (!InputManager::ptr_window) {
//        std::cerr << "GLFW unable to create OpenGL context - abort program\n";
//        glfwTerminate();
//        return false;
//    }
//    glfwMakeContextCurrent(InputManager::ptr_window);
//    glfwSetKeyCallback(InputManager::ptr_window, InputManager::KeyCallback);
//    glfwSetMouseButtonCallback(InputManager::ptr_window, InputManager::MouseButtonCallback);
//    glfwSetInputMode(InputManager::ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//}

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
        if (GLFW_KEY_ESCAPE == key) {
            glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        }
        if (GLFW_KEY_M == key) {
            audio.PlaySounds("../Assets/Sound/ping.wav");
        }
        //switch (key) {
        //case GLFW_KEY_ESCAPE:
        //    glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        //case GLFW_KEY_W:
        //    //keyStateW = GL_TRUE;
        //    
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_W);
        //    //test_model.AddPos(0, 100);
        //    break;
        //case GLFW_KEY_A:
        //    //keyStateA = GL_TRUE;
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_A);
        //    //test_model.AddPos(-100, 0);
        //    break;
        //case GLFW_KEY_S:
        //    //keyStateS = GL_TRUE;
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_S);
        //    //test_model.AddPos(0, -100);
        //    break;
        //case GLFW_KEY_D:
        //    //keyStateD = GL_TRUE;
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_D);
        //    //test_model.AddPos(100, 0);
        //    break;
        //case GLFW_KEY_E:
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_E);
        //    //test_model.AddRot(10);
        //    break;
        //case GLFW_KEY_Q:
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_Q);
        //    //test_model.AddRot(-10);
        //    break;
        //}
        break;
    case GLFW_RELEASE:
        keyStatus[key] = INFO::NONE;
        mail.CreatePostcard(TYPE::KEY_UP, ADDRESS::INPUT, static_cast<INFO>(key));
        //switch (key) {
        //case GLFW_KEY_ESCAPE:
        //    glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        //case GLFW_KEY_W:
        //    //keyStateW = GL_TRUE;
        //    mail.CreatePostcard(TYPE::KEY_UP, ADDRESS::INPUT, INFO::KEY_W);
        //    //test_model.AddPos(0, 100);
        //    break;
        //case GLFW_KEY_A:
        //    //keyStateA = GL_TRUE;
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_A);
        //    //test_model.AddPos(-100, 0);
        //    break;
        //case GLFW_KEY_S:
        //    //keyStateS = GL_TRUE;
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_S);
        //    //test_model.AddPos(0, -100);
        //    break;
        //case GLFW_KEY_D:
        //    //keyStateD = GL_TRUE;
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_D);
        //    //test_model.AddPos(100, 0);
        //    break;
        //case GLFW_KEY_E:
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_E);
        //    //test_model.AddRot(10);
        //    break;
        //case GLFW_KEY_Q:
        //    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, INFO::KEY_Q);
        //    //test_model.AddRot(-10);
        //    break;
        //}
        break;
    default:
        break;
    }
}

void InputManager::KeyCheck() {
    
    //for (Postcard msg : mail.mailbox[ADDRESS::INPUT]) {
    //   if (msg.type == TYPE::KEY_CHECK) {
            for (std::pair<int,INFO> val : keyStatus) {
                if (val.second != INFO::NONE) {
                    mail.CreatePostcard(TYPE::KEY_DOWN, ADDRESS::INPUT, val.second);
                }
            }
    //    }
    //}
    //mail.mailbox[ADDRESS::INPUT].clear();
}

/*
INFO InputManager::KeyConversion(int key) {
    switch (key) {
    case GLFW_KEY_A: return INFO::KEY_A;
    case GLFW_KEY_B: return INFO::KEY_B;
    case GLFW_KEY_C: return INFO::KEY_C;
    case GLFW_KEY_D: return INFO::KEY_D;
    case GLFW_KEY_E: return INFO::KEY_E;
    case GLFW_KEY_F: return INFO::KEY_F;
    case GLFW_KEY_G: return INFO::KEY_G;
    case GLFW_KEY_H: return INFO::KEY_H;
    case GLFW_KEY_I: return INFO::KEY_I;
    case GLFW_KEY_J: return INFO::KEY_J;
    case GLFW_KEY_K: return INFO::KEY_K;
    case GLFW_KEY_L: return INFO::KEY_L;
    case GLFW_KEY_M: return INFO::KEY_M;
    case GLFW_KEY_N: return INFO::KEY_N;
    case GLFW_KEY_O: return INFO::KEY_O;
    case GLFW_KEY_P: return INFO::KEY_P;
    case GLFW_KEY_Q: return INFO::KEY_Q;
    case GLFW_KEY_R: return INFO::KEY_R;
    case GLFW_KEY_S: return INFO::KEY_S;
    case GLFW_KEY_T: return INFO::KEY_T;
    case GLFW_KEY_U: return INFO::KEY_U;
    case GLFW_KEY_V: return INFO::KEY_V;
    case GLFW_KEY_W: return INFO::KEY_W;
    case GLFW_KEY_X: return INFO::KEY_X;
    case GLFW_KEY_Y: return INFO::KEY_Y;
    case GLFW_KEY_Z: return INFO::KEY_Z;
    case GLFW_KEY_UP: return INFO::KEY_UP;
    case GLFW_KEY_DOWN: return INFO::KEY_DOWN;
    case GLFW_KEY_LEFT: return INFO::KEY_LEFT;
    case GLFW_KEY_RIGHT: return INFO::KEY_RIGHT;
    case GLFW_KEY_SPACE: return INFO::KEY_SPACE;
    case GLFW_KEY_ENTER: return INFO::KEY_ENTER;
    case GLFW_KEY_ESCAPE: return INFO::KEY_ESC;
    case GLFW_KEY_0:
    default:return INFO::NONE;
    }
}
*/
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
            mouseClickL = GL_TRUE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouseClickR = GL_TRUE;
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
    cursorposX = xpos - GRAPHICS::w;
    cursorposY = -ypos + GRAPHICS::h;

    test_model.SetPos(cursorposX, cursorposY);
}
