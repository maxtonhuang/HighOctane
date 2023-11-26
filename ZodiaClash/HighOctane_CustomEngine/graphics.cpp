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
*	@email		p.foong\@digipen.edu
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

//#include "GUIManager.h"
#include "Graphics.h"
#include "Input.h"
#include "graphlib.h"
#include "debugdiagnostic.h"
#include "WindowsInterlink.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include "physics.h"
#include <algorithm>
#include <Windows.h>
#include "AssetManager.h"
#include "Serialization.h"

GraphicsManager graphics;

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

    if (window == nullptr) {
        ASSERT(1, "Unable to create game window!");
    }

    glfwMakeContextCurrent(window);

    //Set functions for input manager
    glfwSetKeyCallback(window, InputManager::KeyCallback);
    glfwSetMouseButtonCallback(window, InputManager::MouseButtonCallback);
    glfwSetCursorPosCallback(window, InputManager::CursorPosCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowFocusCallback(window, InputManager::WindowFocusCallback);
    
    HWND hwnd = glfwGetWin32Window(window);
    DragAcceptFiles(hwnd, TRUE);  // Allow the window to accept dropped files
    glfwSetDropCallback(window, FileDropCallback);
    
    //Set default background colour
    glClearColor(0.f, 0.f, 0.f, 1.f);

    //Initialise glew for glew functions
    glewInit();

    //Initialise window icon
    std::string gameIconName{ "zodiaclash_logo.png" };
    std::string defaultPath = "Assets/Textures/";
    std::string path{ defaultPath + gameIconName };
    Serializer serializer;
    if (!serializer.Open(path)) {
        defaultPath = "../Assets/Textures/";
        path = defaultPath + gameIconName;
        if (!serializer.Open(path)) {
            ASSERT(1, "Unable to find game icon!");
            return;
        }
    }
    assetmanager.texture.SetWindowIcon(window, path);

    //Create viewport
    viewport.SetViewport(0, 0, width, height);
    viewport.Unuse();

    //Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Initialise framebuffer
    framebuffer.Initialize();

    /*fonts.Initialize();*/

    //glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(GRAPHICS::defaultWidth), 0.0f, static_cast<float>(GRAPHICS::defaultHeight));

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
    graphics.backgroundsystem.Update();

    viewport.Unuse();
    for (auto& r : renderOrder) {
        r->Draw();
    }

    viewport.Use();
    renderer["static"].DrawFrameBuffer(); //END OF GAMEPLAY DRAW CALL
}

void GraphicsManager::EndDraw() {
    graphics.framebuffer.Clear();
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);
}

Renderer& GraphicsManager::AddRenderer(std::string name) {
    if (renderer.count(name) > 0) {
        return renderer[name];
    }
    renderer[name].SetName(name);
    renderOrder.push_back(&renderer[name]);
    return renderer[name];
}

void GraphicsManager::DrawPoint(float x, float y, float r, float g, float b, float a, Renderer* input) {
    if (input == nullptr) {
        input = &renderer["point"];
    }
    input->AddVertex(Vertex{glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec4{r,g,b,a}});
}

void GraphicsManager::DrawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a, Renderer* input) {
    if (input == nullptr) {
        input = &renderer["line"];
    }
    input->AddVertex(Vertex{glm::vec2{x1 / GRAPHICS::w, y1 / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    input->AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w, y2 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
}

void GraphicsManager::DrawCircle(float x, float y, float radius, float r, float g, float b, float a, Renderer* input) {
    if (input == nullptr) {
        input = &renderer["circle"];
    }
    const float angle = 2.f * PI / (float)GRAPHICS::CIRCLE_SLICES;
    input->AddVertex(Vertex{glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    for (int i = 0; i <= GRAPHICS::CIRCLE_SLICES; ++i) {
        input->AddVertex(Vertex{ glm::vec2{(x + radius * std::cos(angle * i)) / GRAPHICS::w, (y + radius * std::sin(angle * i)) / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    }
    input->Draw();
}

void GraphicsManager::DrawRect(float x1, float y1, float x2, float y2, float r, float g, float b, float a, Renderer* input) {
    if (input == nullptr) {
        input = &renderer["rectangle"];
    }
    input->AddVertex(Vertex{glm::vec2{x1 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    input->AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
    input->AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
    input->AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
    input->Draw();
}

void GraphicsManager::DrawOutline(float x1, float y1, float x2, float y2, float r, float g, float b, float a, Renderer* input) {
    DrawLine(x1, y1, x1, y2, r, g, b, a, input);
    DrawLine(x1, y1, x2, y1, r, g, b, a, input);
    DrawLine(x2, y2, x2, y1, r, g, b, a, input);
    DrawLine(x2, y2, x1, y2, r, g, b, a, input);
}

std::string GraphicsManager::GetName() {
    return "Graphics";
}

bool GraphicsManager::WindowClosed() {
    return glfwWindowShouldClose(window);
}

void GraphicsManager::Fullscreen(bool input) {
    if (input) {
        int x, y, w, h;
        glfwGetWindowSize(window, &w, &h);
        glfwGetWindowPos(window, &x, &y);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        UpdateWindow();
        viewport.SetViewport(0, 0, mode->width, mode->height);
        previousX = x;
        previousY = y;
        previousWidth = w;
        previousHeight = h;
    }
    else {
        glfwSetWindowMonitor(window, NULL, previousX, previousY, previousWidth, previousHeight, 0); //ypos at 32 as it is window title bar size
        UpdateWindow();
        viewport.SetViewport(0, 0, width, height);
    }
    graphics.framebuffer.Recreate();
}

void GraphicsManager::UpdateWindow() {
    glfwGetWindowSize(window, &width, &height);
}

float GraphicsManager::GetWindowWidth() {
    return (float)width;
}

float GraphicsManager::GetWindowHeight() {
    return (float)height;
}

GLFWwindow* GraphicsManager::GetWindow() {
    return window;
}

void GraphicsManager::DrawLabel(TextLabel& txtLblData, Vec2& relTextPos, glm::vec4 color) {    
    static Renderer* fontRenderer{ &renderer["font"] };

    if (previousRenderer != fontRenderer) {
        if (previousRenderer != nullptr) {
            previousRenderer->Draw();
        }
        previousRenderer = fontRenderer;
    }

    // enforce relFontSize to be in range [0.f, 1.f]
    float fontSize = txtLblData.relFontSize;
    Font& fontData{ (txtLblData.font != nullptr) ? *txtLblData.font : *fonts.GetDefaultFont() };

    // iterate through all characters 
    float initPosX = (relTextPos.x);
    float initPosY = (relTextPos.y);
    float xPos = initPosX;
    float yPos;

    std::string::const_iterator c;
    for (c = txtLblData.textString.begin(); c != txtLblData.textString.end(); c++)
    {
        Character ch{ fontData.characters[*c] };

        xPos = (xPos + ch.bearing.x * fontSize);
        yPos = (initPosY - (ch.size.y - ch.bearing.y) * fontSize);

        float w = ch.size.x * fontSize;
        float h = ch.size.y * fontSize;

        glm::vec2 botleft{ xPos / GRAPHICS::w, yPos / GRAPHICS::h };
        glm::vec2 botright{ (xPos + w) / GRAPHICS::w, yPos / GRAPHICS::h };
        glm::vec2 topright{ (xPos + w) / GRAPHICS::w, (yPos + h) / GRAPHICS::h };
        glm::vec2 topleft{ (xPos) / GRAPHICS::w, (yPos + h) / GRAPHICS::h };
        fontRenderer->AddVertex(Vertex{botleft, color, ch.textureID->GetTexCoords((int)ch.texPos,0), (float)ch.textureID->GetID() - 1});
        fontRenderer->AddVertex(Vertex{ botright,color, ch.textureID->GetTexCoords((int)ch.texPos,1), (float)ch.textureID->GetID() - 1 });
        fontRenderer->AddVertex(Vertex{ topleft, color, ch.textureID->GetTexCoords((int)ch.texPos,2), (float)ch.textureID->GetID() - 1 });
        fontRenderer->AddVertex(Vertex{ topright,color, ch.textureID->GetTexCoords((int)ch.texPos,3), (float)ch.textureID->GetID() - 1 });
        fontRenderer->AddVertex(Vertex{ botright,color, ch.textureID->GetTexCoords((int)ch.texPos,1), (float)ch.textureID->GetID() - 1 });
        fontRenderer->AddVertex(Vertex{ topleft, color, ch.textureID->GetTexCoords((int)ch.texPos,2), (float)ch.textureID->GetID() - 1 });
        xPos += (ch.advance >> 6) * fontSize; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64to get amount of pixels))
    }
}