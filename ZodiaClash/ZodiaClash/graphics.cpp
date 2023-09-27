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
*	@file		Graphics.cpp
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief		Graphics system of the engine
*
*	This file contains functions used in the main graphic system of the engine
*   This includes the initialisation and destruction of the graphics system as well
*   as the main draw call of the system
*
******************************************************************************/

#include "GUIManager.h"
#include "Graphics.h"
#include "Input.h"
#include "graphlib.h"
#include "debugdiagnostic.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

//Model test_model;
Texture test_tex;
GraphicsManager graphics;

Renderer flatRenderer;
Renderer textureRenderer;
Renderer pointRenderer;
Renderer lineRenderer;
Renderer lineloopRenderer;
Renderer rectRenderer;
Renderer circleRenderer;
Renderer fontRenderer;

/*                                                   objects with file scope
----------------------------------------------------------------------------- */

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Initialize renderers
    flatRenderer.Initialize("../Assets/Shaders/flat.vert", "../Assets/Shaders/flat.frag",GL_TRIANGLES);
    textureRenderer.Initialize("../Assets/Shaders/texture.vert", "../Assets/Shaders/texture.frag",GL_TRIANGLES);
    pointRenderer.Initialize(flatRenderer.ShaderProgram(), GL_POINTS);
    lineRenderer.Initialize(flatRenderer.ShaderProgram(), GL_LINES);
    lineloopRenderer.Initialize(flatRenderer.ShaderProgram(), GL_LINE_LOOP);
    rectRenderer.Initialize(flatRenderer.ShaderProgram(), GL_TRIANGLE_STRIP);
    circleRenderer.Initialize(flatRenderer.ShaderProgram(), GL_TRIANGLE_FAN);
    fontRenderer.Initialize("../Assets/Shaders/font.vert", "../Assets/Shaders/font.frag");

    texList.AddSpriteSheet("duck.png", 1, 6, 6);
    texList.AddSpriteSheet("duck2.png", 1, 6, 6);

    fonts.Initialize();
    fonts.LoadFont("Danto Lite Normal.ttf");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(GRAPHICS::defaultWidth), 0.0f, static_cast<float>(GRAPHICS::defaultHeight));
    fontRenderer.shaderprogram.Use();
    std::cout << fontRenderer.shaderprogram.GetHandle() << "\n";
    std::cout << glGetUniformLocation(fontRenderer.shaderprogram.GetHandle(), "projection") << "\n";
    std::cout << glGetUniformLocation(fontRenderer.shaderprogram.GetHandle(), "textColor") << "\n";
    //glUniformMatrix4fv(glGetUniformLocation(fontRenderer.shaderprogram.GetHandle(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    guiManager.Init(window);

    //TEMP
    glPointSize(10.f);
    glLineWidth(3.f);
    glfwSwapInterval(0);
}

void GraphicsManager::Update() {
    static float fpsInterval = 1.f;
    static int count = 0;
    fpsInterval += g_dt;
    ++count;
    if (fpsInterval > 1) {
        std::stringstream title;
        title << "ZodiaClash " << count;
        glfwSetWindowTitle(window, title.str().c_str());
        fpsInterval -= 1;
        count = 0;
    }
}

void GraphicsManager::Draw() {
    textureRenderer.Draw();
    flatRenderer.Draw();
    lineloopRenderer.Draw();
    lineRenderer.Draw();
    pointRenderer.Draw();

    fontRenderer.Draw();
    //test_model.DrawOutline();
    
    DrawLabel("AAA", "Danto Lite Normal.ttf", 0.5f, Transform{ Vec2{ 0.f,0.f }, 0.f, Vec2{ 1.f, 1.f }, vmath::Vector2{ 0,0 } }, Color{ glm::vec4{ 1,1,1,1 } });

    DrawCircle(100, -100, 20);
    DrawRect(0, 0, 50, 50);
    DrawOutline(-50, -50, 0, 0);

    guiManager.Update(window);
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsManager::DrawPoint(float x, float y, float r, float g, float b) {
    pointRenderer.AddVertex(Vertex{ glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec3{r,g,b} });
}

void GraphicsManager::DrawLine(float x1, float y1, float x2, float y2, float r, float g, float b) {
    lineRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w, y1 / GRAPHICS::h}, glm::vec3{r,g,b} });
    lineRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w, y2 / GRAPHICS::h}, glm::vec3{r,g,b} });
}

void GraphicsManager::DrawCircle(float x, float y, float radius, float r, float g, float b) {
    const float PI = 3.141592653589793238463f;
    const float angle = 2.f * PI / (float)GRAPHICS::CIRCLE_SLICES;
    circleRenderer.AddVertex(Vertex{ glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec3{r,g,b} });
    for (int i = 0; i <= GRAPHICS::CIRCLE_SLICES; ++i) {
        circleRenderer.AddVertex(Vertex{ glm::vec2{(x + radius * std::cos(angle * i)) / GRAPHICS::w, (y + radius * std::sin(angle * i)) / GRAPHICS::h}, glm::vec3{r,g,b}});
    }
    circleRenderer.Draw();
}

void GraphicsManager::DrawRect(float x1, float y1, float x2, float y2, float r, float g, float b) {
    rectRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{r,g,b} });
    rectRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{r,g,b} });
    rectRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{r,g,b} });
    rectRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{r,g,b} });
    rectRenderer.Draw();
}

void GraphicsManager::DrawOutline(float x1, float y1, float x2, float y2, float r, float g, float b) {
    lineloopRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{r,g,b}});
    lineloopRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{r,g,b} });
    lineloopRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{r,g,b} });
    lineloopRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{r,g,b} });
    lineloopRenderer.Draw();
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

void GraphicsManager::DrawLabel(std::string labelText, std::string fontName, float scale, Transform transData, Color colorData) {
    
    Font fontData = fonts.GetFont(fontName);
    // TODO some sort of non null checking for fontData?
    // iterate through all characters 
    std::string::const_iterator c;
    for (c = labelText.begin(); c != labelText.end(); c++)
    {
        Character ch = fontData.Characters[*c];

        float xpos = (transData.position.x + ch.Bearing.x * scale);
        float ypos = (transData.position.y - (ch.Size.y - ch.Bearing.y) * scale);

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        glm::vec3 color(1.f, 1.f, 1.f);
        glm::vec2 botleft{ xpos / GRAPHICS::w, ypos / GRAPHICS::h };
        glm::vec2 botright{ (xpos + w) / GRAPHICS::w, ypos / GRAPHICS::h };
        glm::vec2 topright{ (xpos + w) / GRAPHICS::w, (ypos + h) / GRAPHICS::h };
        glm::vec2 topleft{ (xpos) / GRAPHICS::w, (ypos + h) / GRAPHICS::h };
        fontRenderer.AddVertex(Vertex{ botleft,color, glm::vec2{0,1}, (float)ch.TextureID - 1});
        fontRenderer.AddVertex(Vertex{ botright,color, glm::vec2{1,1}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ topleft,color, glm::vec2{0,0}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ topright,color, glm::vec2{1,0}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ botright,color, glm::vec2{1,1}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ topleft,color, glm::vec2{0,0}, (float)ch.TextureID - 1 });
        fontRenderer.FontDraw(ch.TextureID);
        transData.position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
}
