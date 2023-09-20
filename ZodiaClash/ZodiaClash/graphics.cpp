#include "GUIManager.h"
#include "Graphics.h"
#include "Input.h"
#include "graphlib.h"
#include "debugdiagnostic.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

Model test_model;
Texture test_tex;
GraphicsManager graphics;

Renderer flatRenderer;
Renderer textureRenderer;

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
//GLFWwindow* GraphicsManager::window;
//int GraphicsManager::width;
//int GraphicsManager::height;
//Shader GraphicsManager::shaderprogram;
//GraphicsManager::VAOInfo GraphicsManager::vao;

GraphicsManager::GraphicsManager() {
    width = 0;
    height = 0;
    window = nullptr;
}

GraphicsManager::~GraphicsManager() {
    glfwTerminate();
}

void GraphicsManager::Initialize(int w, int h) {
    //TEMPORARY INITIALISATION, TO BE READ FROM FILE
    width = w;
    height = h;

    glfwInit();

    //Set GLFW settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //Create window
    window = glfwCreateWindow(width, height, "ZodiaClash", NULL, NULL);

    glfwMakeContextCurrent(window);

    //Set functions for input manager
    glfwSetKeyCallback(window, InputManager::KeyCallback);
    glfwSetMouseButtonCallback(window, InputManager::MouseButtonCallback);
    glfwSetCursorPosCallback(window, InputManager::CursorPosCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //Set default background colour
    glClearColor(1.f, 0.f, 0.f, 1.f);

    //Create viewport
    glViewport(0, 0, width, height);

    //Initialise glew for glew functions
    glewInit();

    //Enable alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //Initialize renderers
    flatRenderer.Initialize("../Assets/Shaders/FlatVertexShader2.vert", "../Assets/Shaders/FlatFragmentShader2.frag");
    textureRenderer.Initialize("../Assets/Shaders/texture.vert", "../Assets/Shaders/texture.frag");

    texList.AddSpriteSheet("duck.png", 1, 6, 6);
    test_model.AttachTexture("duck.png");

    guiManager.Init(window);

    //TEMP
    std::default_random_engine rng;
    std::uniform_real_distribution<float> rand_width(0, width);
    std::uniform_real_distribution<float> rand_height(0, height);
    for (int i = 0; i < 0; i++) {
        Model mdl;
        mdl.AttachTexture("cat.png");
        mdl.SetPos(rand_width(rng) - width / 2, rand_height(rng) - height / 2);
        mdl.SetScale(0.1, 0.1);
        modelList.emplace_back(mdl);
    }

    //glfwSwapInterval(0);
    //Draw();
}

void GraphicsManager::Update() {
    static float fpsInterval = 1.f;
    static int count = 0;
    fpsInterval += g_dt;
    ++count;
    if (fpsInterval > 1) {
        std::stringstream title;
        title << "ZodiaClash " << count;
        //glfwSetWindowTitle(window, title.str().c_str());
        fpsInterval -= 1;
        count = 0;
    }
}

void GraphicsManager::Draw() { // ------------- Can this go into ECS? ------------------------------//
    glClear(GL_COLOR_BUFFER_BIT);

    for (Model& m : modelList) {
        m.Update();
        m.Draw();
    }

    test_circle1.Update();
    test_circle2.Update();
    test_circle3.Update();
    test_circle1.Draw();
    test_circle2.Draw();
    test_circle3.Draw();
    test_rect1.Update();
    test_rect2.Update();
    test_rect3.Update();
    test_rect1.Draw();
    test_rect2.Draw();
    test_rect3.Draw();

    test_model.Update();
    test_model.Draw();
    test_model.DrawOutline();

    textureRenderer.Draw();
    
    guiManager.Update(window);
    glfwSwapBuffers(window);
}

void GraphicsManager::DrawPoint(float x, float y, float pointsize) {
    glPointSize(pointsize);
    flatRenderer.AddVertex(Vertex{ glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.Draw(GL_POINTS);
}

void GraphicsManager::DrawLine(float x1, float y1, float x2, float y2) {
    flatRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w, y1 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w, y2 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.Draw(GL_LINES);
}

std::string GraphicsManager::GetName() {
    return "Graphics";
}

bool GraphicsManager::WindowClosed() {
    return glfwWindowShouldClose(window);
}

float GraphicsManager::GetWidth() {
    return (float)width;
}

float GraphicsManager::GetHeight() {
    return (float)height;
}
