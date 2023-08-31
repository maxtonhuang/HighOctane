#include "input.h"
#include <iostream>

void InputManager::KeyCallback(GLFWwindow* pwin, int key, int scancode, int action, int mod) {
    if (GLFW_PRESS == action) {
        if (GLFW_KEY_ESCAPE) {
            std::exit(0);
        }
    }
    else if (GLFW_REPEAT == action) {

    }
    else if (GLFW_RELEASE == action) {

    }
}
void InputManager::MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod) {

}
void InputManager::CursorPosCallback(GLFWwindow* pwin, double xpos, double ypos) {

}
