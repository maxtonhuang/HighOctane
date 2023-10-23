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
*	@file		GUIManager.cpp
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file manages ImGui panels.Each panels will be split into their independant
*   source files.
******************************************************************************/
#include "GUIManager.h"
#include "debugdiagnostic.h"
#include "DebugProfile.h"
#include "enginecore.h"
#include "debuglog.h"
#include "ECS.h"
#include "Components.h"
#include "EntityFactory.h"
#include "physics.h"
#include "Serialization.h"
#include "EntityFactory.h"
#include "ImGuiConsole.h"
#include "ImGuiPerformance.h"
#include "ImGuiEntitiesManager.h"
#include "ImGuiPlayStop.h"
#include "ImGuiAssetLibrary.h"
#include "graphics.h"
#include "FrameBuffer.h"


constexpr float fontSize = 20.f;
constexpr float fontSizeLarge = 50.f;
ImFont* latoLargeBold;

//GUIManager guiManager;
//FrameBuffer frameBuffer;
GUIManager::GUIManager()
{
	ImGui::CreateContext();
    //ImPlot::CreateContext();
    //GUIWindow = nullptr;
}

GUIManager::~GUIManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //ImPlot::DestroyContext();
}


// Our state
//bool show_demo_window;

void GUIManager::Init()
{
    GLFWwindow* window = graphics.GetWindow();
        
    const char* glsl_version = "#version 450";
    //graphics.viewport.SetViewport(GRAPHICS::defaultWidthF * 0.25f, GRAPHICS::defaultHeightF * 0.5f, GRAPHICS::defaultWidthF * 0.5f, GRAPHICS::defaultHeightF * 0.5f);
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // Setup Dear ImGui style

    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Regular.ttf", fontSize);
    latoLargeBold = io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Bold.ttf", fontSizeLarge);
        
    ImGui::StyleColorsDark();
    // Init console window
    InitConsole();
    InitEntitiesManager();
    

#if ENABLE_DEBUG_PROFILE
    // Init performance window
    InitPerformance();
#endif

}

void GUIManager::Update()
{
    GLFWwindow* window = graphics.GetWindow();
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // Create the main dockable window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(main_viewport->WorkPos);
    ImGui::SetNextWindowSize(main_viewport->WorkSize);
    ImGui::SetNextWindowViewport(main_viewport->ID);

    {
        ImGui::Begin("Dockable Window", nullptr, window_flags);
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        
        }

        // Create a menu bar for the window
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Files")) {
                ImGui::MenuItem("Load Scene");
                ImGui::MenuItem("Save Scene");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    /*ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
        hoveringPanel = true;
    }
    else {
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        hoveringPanel = false;
    }*/
   /* if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);*/

    {
        ImGui::Begin("Game Viewport");
        unsigned textureID = graphics.framebuffer.GetTextureID();
        float xSizeAvailable = ImGui::GetContentRegionAvail().x;
        float ySizeAvailable = ImGui::GetContentRegionAvail().y;

        // Add spacing to centralize viewport
        if (ySizeAvailable < (xSizeAvailable * 9.f / 16.f)) {
            ImGui::Dummy(ImVec2(((xSizeAvailable - (ySizeAvailable * 16.f / 9.f)) / 2.f), 0));
            ImGui::SameLine();
        }
        else if ((xSizeAvailable * 9.f / 16.f) < ySizeAvailable) {
            ImGui::Dummy(ImVec2(0, ((ySizeAvailable - (xSizeAvailable * 9.f / 16.f)) / 2.f)));
        }
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        float w = ((xSizeAvailable * 9.f / 16.f) < ySizeAvailable) ? xSizeAvailable : (ySizeAvailable * 16.f / 9.f);
        float h = ((xSizeAvailable * 9.f / 16.f) < ySizeAvailable) ? (xSizeAvailable * 9.f / 16.f) : ySizeAvailable;
        graphics.viewport.SetViewport(window->DC.CursorPos.x,graphics.GetWindowHeight() - window->DC.CursorPos.y - h,w,h);
        ImGui::Image((void*)textureID, ImVec2{ w , h }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
        ImGui::End();
    }
  
    // Update the console
    UpdateConsole();
    UpdateEntitiesManager();
    UpdatePlayStop();
    UpdateAssetLibrary();

#if ENABLE_DEBUG_PROFILE
    // Update the performance console
    UpdatePerformance();
#endif

    // Rendering
    ImGui::Render();
int display_w, display_h;
glfwGetFramebufferSize(window, &display_w, &display_h);
//    //glViewport(0, 0, display_w, display_h);
//   // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
//   // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//   // glfwSwapBuffers(window);

}
