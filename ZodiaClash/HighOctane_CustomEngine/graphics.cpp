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
/*!***********************************************************************
 \brief
  Constructs a GraphicsManager object, initializing its width, height, and window pointers to default values.
 \param
  This constructor does not take any parameters.
 \return
  This constructor does not return a value.
 *************************************************************************/

GraphicsManager::GraphicsManager() {
    width = 0;
    height = 0;
    window = nullptr;
}

/*!***********************************************************************
 \brief
  Destroys the GraphicsManager object, performing necessary cleanup such as terminating GLFW.
 \param
  This destructor does not take any parameters.
 \return
  This destructor does not return a value.
 *************************************************************************/
GraphicsManager::~GraphicsManager() {
    glfwTerminate();
}

/*!***********************************************************************
 \brief
  Initializes the graphics system, setting up the GLFW window, OpenGL context, and other necessary graphics settings. This includes window creation, setting GLFW callbacks, initializing GLEW, setting the window icon, and preparing the rendering viewport and framebuffer.
 \param w
  The width of the window.
 \param h
  The height of the window.
 \return
  This method does not return a value. It initializes the graphics system with the specified window dimensions.
 *************************************************************************/
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
    glPointSize(15.f);
    glLineWidth(3.f);

    //Disable vsync when not in game mode
    if (!fullscreen) {
        glfwSwapInterval(0);
    }

    Fullscreen(fullscreen);
}

/*!***********************************************************************
 \brief
  Updates the graphics manager's state, including calculating and displaying the frames per second (FPS) in the window title.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It updates the state of the graphics manager.
 *************************************************************************/
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

/*!***********************************************************************
 \brief
  Handles the drawing operations for each frame, updating background systems, rendering entities, and managing the viewport.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It performs drawing operations for the current frame.
 *************************************************************************/
void GraphicsManager::Draw() {
    graphics.backgroundsystem.Update();

    viewport.Unuse();
    for (auto& r : renderOrder) {
        r->Draw();
    }

    viewport.Use();
    renderer["static"].DrawFrameBuffer(); //END OF GAMEPLAY DRAW CALL
}

/*!***********************************************************************
 \brief
  Finalizes drawing operations for the current frame, clearing the framebuffer and swapping the window buffers.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It finalizes the drawing operations.
 *************************************************************************/
void GraphicsManager::EndDraw() {
    graphics.framebuffer.Clear();
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);
}

/*!***********************************************************************
 \brief
  Adds a renderer to the graphics manager or retrieves an existing one by name. This method manages the collection of renderers used for drawing various graphics components.
 \param name
  The name identifier for the renderer.
 \return
  Returns a reference to the Renderer object associated with the given name. If the renderer does not exist, it creates a new one.
 *************************************************************************/
Renderer& GraphicsManager::AddRenderer(std::string name) {
    if (renderer.count(name) > 0) {
        return renderer[name];
    }
    renderer[name].SetName(name);
    renderOrder.push_back(&renderer[name]);
    return renderer[name];
}

/*!***********************************************************************
 \brief
  Draws a point on the screen with specified dimensions and color. Optionally, a specific renderer can be provided to perform the drawing; otherwise, a default renderer is used.
 \param
  Parameters vary based on the method but generally include coordinates, dimensions, color, and an optional renderer.
 \return
  This method does not return a value. It performs drawing operations for the specified shape.
 *************************************************************************/
void GraphicsManager::DrawPoint(float x, float y, float r, float g, float b, float a, Renderer* input) {
    if (input == nullptr) {
        input = &renderer["point"];
    }
    input->AddVertex(Vertex{glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec4{r,g,b,a}});
}

/*!***********************************************************************
 \brief
  Draws a line on the screen with specified dimensions and color. Optionally, a specific renderer can be provided to perform the drawing; otherwise, a default renderer is used.
 \param
  Parameters vary based on the method but generally include coordinates, dimensions, color, and an optional renderer.
 \return
  This method does not return a value. It performs drawing operations for the specified shape.
 *************************************************************************/

void GraphicsManager::DrawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a, Renderer* input) {
    if (input == nullptr) {
        input = &renderer["line"];
    }
    input->AddVertex(Vertex{glm::vec2{x1 / GRAPHICS::w, y1 / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    input->AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w, y2 / GRAPHICS::h}, glm::vec4{r,g,b,a} });
}

/*!***********************************************************************
 \brief
  Draws a circle on the screen with specified dimensions and color. Optionally, a specific renderer can be provided to perform the drawing; otherwise, a default renderer is used.
 \param
  Parameters vary based on the method but generally include coordinates, dimensions, color, and an optional renderer.
 \return
  This method does not return a value. It performs drawing operations for the specified shape.
 *************************************************************************/
void GraphicsManager::DrawCircle(float x, float y, float radius, float r, float g, float b, float a, Renderer* input) {
    if (input == nullptr) {
        input = &renderer["circle"];
    }
    const float angle = 2.f * PI / (float)GRAPHICS::CIRCLE_SLICES;
    Vertex center{ Vertex{glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec4{r,g,b,a}} };
    //input->AddVertex(Vertex{glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec4{r,g,b,a}});
    for (int i = 0; i <= GRAPHICS::CIRCLE_SLICES - 1; ++i) {
        input->AddVertex(center);
        input->AddVertex(Vertex{ glm::vec2{(x + radius * std::cos(angle * i)) / GRAPHICS::w, (y + radius * std::sin(angle * i)) / GRAPHICS::h}, glm::vec4{r,g,b,a}});
        input->AddVertex(Vertex{ glm::vec2{(x + radius * std::cos(angle * (i + 1))) / GRAPHICS::w, (y + radius * std::sin(angle * (i + 1))) / GRAPHICS::h}, glm::vec4{r,g,b,a} });
    }
    //input->Draw();
}

/*!***********************************************************************
 \brief
  Draws a Rectangle on the screen with specified dimensions and color. Optionally, a specific renderer can be provided to perform the drawing; otherwise, a default renderer is used.
 \param
  Parameters vary based on the method but generally include coordinates, dimensions, color, and an optional renderer.
 \return
  This method does not return a value. It performs drawing operations for the specified shape.
 *************************************************************************/

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

/*!***********************************************************************
 \brief
  Draws a outline on the screen with specified dimensions and color. Optionally, a specific renderer can be provided to perform the drawing; otherwise, a default renderer is used.
 \param
  Parameters vary based on the method but generally include coordinates, dimensions, color, and an optional renderer.
 \return
  This method does not return a value. It performs drawing operations for the specified shape.
 *************************************************************************/
void GraphicsManager::DrawOutline(float x1, float y1, float x2, float y2, float r, float g, float b, float a, Renderer* input) {
    DrawLine(x1, y1, x1, y2, r, g, b, a, input);
    DrawLine(x1, y1, x2, y1, r, g, b, a, input);
    DrawLine(x2, y2, x2, y1, r, g, b, a, input);
    DrawLine(x2, y2, x1, y2, r, g, b, a, input);
}

/*!***********************************************************************
 \brief
  retrieving the graphics manager's name
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/

std::string GraphicsManager::GetName() {
    return "Graphics";
}

/*!***********************************************************************
 \brief
  checking if the window is closed
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/

bool GraphicsManager::WindowClosed() {
    return glfwWindowShouldClose(window);
}

/*!***********************************************************************
 \brief
  toggling fullscreen mode
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/
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

/*!***********************************************************************
 \brief
  updating window dimensions
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/
void GraphicsManager::UpdateWindow() {
    glfwGetWindowSize(window, &width, &height);
}

/*!***********************************************************************
 \brief
  retrieving window width 
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/
float GraphicsManager::GetWindowWidth() {
    return (float)width;
}

/*!***********************************************************************
 \brief
  retrieving window height
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/
float GraphicsManager::GetWindowHeight() {
    return (float)height;
}

/*!***********************************************************************
 \brief
  retrieving window
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/
GLFWwindow* GraphicsManager::GetWindow() {
    return window;
}

/*!***********************************************************************
 \brief
  retrieving text label
 \param
  Parameters vary based on the method, including booleans for fullscreen mode and text label data for drawing.
 \return
  Returns vary based on the method, including boolean status, string names, window dimensions, and GLFW window pointers.
 *************************************************************************/
void GraphicsManager::DrawLabel(TextLabel& txtLblData, glm::vec4 color) {    
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

    std::string::const_iterator c;

    for (TextLine line : txtLblData.lineData) {
        float initPosX = line.relTransform.x;
        float initPosY = line.relTransform.y;
        float xPos = initPosX;
        float yPos;
        for (c = line.lineString.begin(); c != line.lineString.end(); c++) {
            Character ch{ fontData.characters[*c] };
            if (ch.textureID == nullptr) {
				continue;
			}
            xPos = (xPos + ch.bearing.x * fontSize);
            yPos = (initPosY - (ch.size.y - ch.bearing.y) * fontSize);
                
            float w = ch.size.x * fontSize;
            float h = ch.size.y * fontSize;

            glm::vec2 botleft{ xPos / GRAPHICS::w, yPos / GRAPHICS::h };
            glm::vec2 botright{ (xPos + w) / GRAPHICS::w, yPos / GRAPHICS::h };
            glm::vec2 topright{ (xPos + w) / GRAPHICS::w, (yPos + h) / GRAPHICS::h };
            glm::vec2 topleft{ (xPos) / GRAPHICS::w, (yPos + h) / GRAPHICS::h };
            fontRenderer->AddVertex(Vertex{ botleft, color, ch.textureID->GetTexCoords((int)ch.texPos,0), (float)ch.textureID->GetID() - 1 });
            fontRenderer->AddVertex(Vertex{ botright,color, ch.textureID->GetTexCoords((int)ch.texPos,1), (float)ch.textureID->GetID() - 1 });
            fontRenderer->AddVertex(Vertex{ topleft, color, ch.textureID->GetTexCoords((int)ch.texPos,2), (float)ch.textureID->GetID() - 1 });
            fontRenderer->AddVertex(Vertex{ topright,color, ch.textureID->GetTexCoords((int)ch.texPos,3), (float)ch.textureID->GetID() - 1 });
            fontRenderer->AddVertex(Vertex{ botright,color, ch.textureID->GetTexCoords((int)ch.texPos,1), (float)ch.textureID->GetID() - 1 });
            fontRenderer->AddVertex(Vertex{ topleft, color, ch.textureID->GetTexCoords((int)ch.texPos,2), (float)ch.textureID->GetID() - 1 });
            xPos += (ch.advance >> 6) * fontSize; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64to get amount of pixels))
        }
    }
}