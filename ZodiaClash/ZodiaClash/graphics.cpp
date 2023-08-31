#include "graphics.h"
#include "input.h"
#include <iostream>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
GLFWwindow* GraphicsManager::window;
int GraphicsManager::width;
int GraphicsManager::height;
Shader GraphicsManager::shaderprogram;

void GraphicsManager::Init() {
    //TEMPORARY INITIALISATION, TO BE READ FROM FILE
    width = 1024;
    height = 768;

    glfwInit();

    //Set GLFW settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(width, height, "ZodiaClash", NULL, NULL);

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, InputManager::KeyCallback);
    glfwSetMouseButtonCallback(window, InputManager::MouseButtonCallback);
    glfwSetCursorPosCallback(window, InputManager::CursorPosCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glClearColor(0.f, 1.f, 0.f, 1.f);

    glViewport(0, 0, width, height);

    glewInit();

    //Compile shaders
    std::vector<std::pair<GLenum, std::string>> shadervector{
        std::make_pair(GL_VERTEX_SHADER, "../Assets/Shaders/vertexshader.vert"),
        std::make_pair(GL_FRAGMENT_SHADER, "../Assets/Shaders/fragmentshader.frag")
    };

    if (shaderprogram.Compile(shadervector) == false) {
        std::cout << "Unable to compile shader program! Exiting...\n";
        std::exit(EXIT_FAILURE);
    }

    //TEMP
    Draw();
}

void GraphicsManager::Cleanup() {
    shaderprogram.DeleteShader();
    glfwTerminate();
}

void GraphicsManager::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
}