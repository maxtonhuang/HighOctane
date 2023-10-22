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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include "physics.h"
#include <algorithm>

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
    //fonts.~FontManager();
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

    //Set default background colour
    glClearColor(1.f, 0.f, 0.f, 1.f);

    //Initialise glew for glew functions
    glewInit();

    //Create viewport
    viewport.SetViewport(0, 0, width, height);
    viewport.Unuse();

    //Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Initialise framebuffer
    framebuffer.Initialize();

    fonts.Initialize();

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(GRAPHICS::defaultWidth), 0.0f, static_cast<float>(GRAPHICS::defaultHeight));

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
   

    // note: to draw as entity!
    std::string labelText = "© 2023 High Octane";
    float relFontSize = 0.48f;
    Vec2 relTextPos = { 0.45f, 0.85f };
    glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };
    DrawLabel(labelText, "mikachan", "Regular", relFontSize, relTextPos, color);

    labelText = "ZodiaClash v0.1";
    relTextPos = { -0.95f, -0.9f };
    DrawLabel(labelText, "Danto Lite Normal", "Regular", relFontSize, relTextPos, color);
    //DrawLabel(labelText, "mikachan", "Regular", relFontSize, relTextPos, color);
    //physics::PHYSICS->DebugDraw();

    for (auto& r : renderer) {
        r.second.Draw();
    }

    viewport.Use();
    renderer["static"].DrawFrameBuffer(); //END OF GAMEPLAY DRAW CALL
    viewport.Unuse();
}

void GraphicsManager::EndDraw() {
    graphics.framebuffer.Clear();
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsManager::DrawPoint(float x, float y, float r, float g, float b, float a) {
    renderer["point"].AddVertex(Vertex{glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec4{r,g,b,a}});
}

void GraphicsManager::DrawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    renderer["line"].AddVertex(Vertex{glm::vec2{x1 / GRAPHICS::w, y1 / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    renderer["line"].AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w, y2 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
}

void GraphicsManager::DrawCircle(float x, float y, float radius, float r, float g, float b, float a) {
    const float PI = 3.141592653589793238463f;
    const float angle = 2.f * PI / (float)GRAPHICS::CIRCLE_SLICES;
    renderer["circle"].AddVertex(Vertex{glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    for (int i = 0; i <= GRAPHICS::CIRCLE_SLICES; ++i) {
        renderer["circle"].AddVertex(Vertex{ glm::vec2{(x + radius * std::cos(angle * i)) / GRAPHICS::w, (y + radius * std::sin(angle * i)) / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    }
    renderer["circle"].Draw();
}

void GraphicsManager::DrawRect(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    renderer["rectangle"].AddVertex(Vertex{glm::vec2{x1 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    renderer["rectangle"].AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
    renderer["rectangle"].AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
    renderer["rectangle"].AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
    renderer["rectangle"].Draw();
}

void GraphicsManager::DrawOutline(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    DrawLine(x1, y1, x1, y2, r, g, b, a);
    DrawLine(x1, y1, x2, y1, r, g, b, a);
    DrawLine(x2, y2, x2, y1, r, g, b, a);
    DrawLine(x2, y2, x1, y2, r, g, b, a);
}

std::string GraphicsManager::GetName() {
    return "Graphics";
}

bool GraphicsManager::WindowClosed() {
    return glfwWindowShouldClose(window);
}

void GraphicsManager::Fullscreen(bool input) {
    if (input) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        UpdateWindow();
        viewport.SetViewport(0, 0, mode->width, mode->height);
    }
    else {
        glfwSetWindowMonitor(window, NULL, 0, 32, width, height, 0); //ypos at 32 as it is window title bar size
        viewport.SetViewport(0, 0, width, height);
    }
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

void GraphicsManager::DrawLabel(std::string labelText, std::string fontFamily, std::string fontVariant, float relFontSize, Vec2 relTextPos, glm::vec4 color) {
    
    static Renderer* fontRenderer{ &renderer["font"] };
    //ASSERT(((relFontSize < 0.f) || (relFontSize > 1.f)), "Relative font size specified is out of range [0.f,1.f]!");

    // enforce relFontSize to be in range [0.f, 1.f]
    relFontSize = std::max(0.f, relFontSize);
    relFontSize = std::min(relFontSize, 1.f);

    // TODO some sort of non null checking for fontData?
    // find font in fontCollection (null checking included)
    Font& fontData{ *fonts.GetFont(fontFamily, fontVariant) };

    if (fontData.characters.empty() == false) {
        // test GetInfo
        std::pair<std::string, std::string> fontInfo = fontData.GetInfo();

        // iterate through all characters 
        float initPosX = (relTextPos.x * GRAPHICS::w);
        float initPosY = (relTextPos.y * GRAPHICS::h);
        float xPos = initPosX;
        float yPos;

        std::string::const_iterator c;
        for (c = labelText.begin(); c != labelText.end(); c++)
        {
            Character ch{ fontData.characters[*c] };

            xPos = (xPos + ch.bearing.x * relFontSize);
            yPos = (initPosY - (ch.size.y - ch.bearing.y) * relFontSize);

            float w = ch.size.x * relFontSize;
            float h = ch.size.y * relFontSize;
            //glm::vec3 color(1.f, 1.f, 1.f);
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
            xPos += (ch.advance >> 6) * relFontSize; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }    
}
