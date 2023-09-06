
/*                                                                   includes
----------------------------------------------------------------------------- */
#include "input.h"
#include <iostream>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
GLboolean InputManager::keyStateW{ GL_FALSE };
GLboolean InputManager::keyStateA{ GL_FALSE };
GLboolean InputManager::keyStateS{ GL_FALSE };
GLboolean InputManager::keyStateD{ GL_FALSE };
GLboolean InputManager::mouseClickL{ GL_FALSE };
GLboolean InputManager::mouseClickR{ GL_FALSE };


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
    
    if (GLFW_PRESS == action) {
        if (GLFW_KEY_ESCAPE == key) {
            glfwSetWindowShouldClose(pwin, GLFW_TRUE);
        }
        switch (key) {
        case GLFW_KEY_W:
            keyStateW = GL_TRUE;
            test_model.AddPos(0, 100);
            break;
        case GLFW_KEY_A:
            keyStateA = GL_TRUE;
            test_model.AddPos(-100, 0);
            break;
        case GLFW_KEY_S:
            keyStateS = GL_TRUE;
            test_model.AddPos(0, -100);
            break;
        case GLFW_KEY_D:
            keyStateD = GL_TRUE;
            test_model.AddPos(100, 0);
            break;
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

}
