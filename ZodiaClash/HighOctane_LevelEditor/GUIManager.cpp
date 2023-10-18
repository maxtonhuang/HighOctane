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
#include "graphics.h"

//GUIManager guiManager;

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

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // Setup Dear ImGui style
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

   /* if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);*/

    // Update the console
    UpdateConsole();
    UpdateEntitiesManager();
    UpdatePlayStop();

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
