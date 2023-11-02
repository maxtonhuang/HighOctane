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
#include "ImGuiMenuBar.h"
#include "ImGuiSceneHierarchy.h"
#include "ImGuiContentBrowser.h"
#include "ImGuiLayer.h"
#include "graphics.h"
#include "FrameBuffer.h"
#include "AssetManager.h"
#include "Global.h"
#include "vmath.h"
#include <algorithm>
#include <vector>
#include "AssetManager.h"
#include "Layering.h"

constexpr float fontSizeS = 10.f;
constexpr float fontSizeM = 20.f;
constexpr float fontSizeL = 30.f;
constexpr float fontSizeXL = 50.f;
ImFont* latoSmall;
ImFont* latoMedium;
ImFont* latoLarge;
ImFont* latoLargeBold;
float currentFontSize{ fontSizeM };
ImGuiStyle originalStyle;
bool firstSet = false;


//GUIManager guiManager;
//FrameBuffer frameBuffer;
GUIManager::GUIManager()
{
    //ImPlot::CreateContext();
    //GUIWindow = nullptr;
    LoadIcons();
}

GUIManager::~GUIManager()
{
    if (initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    //ImPlot::DestroyContext();
    UnloadIcons();
}



void GUIManager::Init()
{
    ImGui::CreateContext();
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
    latoMedium = io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Regular.ttf", fontSizeM);
    latoLargeBold = io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Bold.ttf", fontSizeXL);

    //ImFontConfig config;
    //io.Fonts->Clear();
    //char m[] = "latoMedium";
    //strcpy_s(config.Name, strlen(m) + 1, m);
    //latoMedium = io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Regular.ttf", fontSizeM, &config);
    //char s[] = "latoSmall";
    //strcpy_s(config.Name, strlen(s) + 1, s);
    //latoSmall = io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Regular.ttf", fontSizeS, &config);
    //char l[] = "latoLarge";
    //strcpy_s(config.Name, strlen(l) + 1, l);
    //latoLarge = io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Regular.ttf", fontSizeL, &config);
    //char xl[] = "latoXL";
    //strcpy_s(config.Name, strlen(xl) + 1, xl);
    //latoLargeBold = io.Fonts->AddFontFromFileTTF("../Assets/Fonts/Lato/Lato-Bold.ttf", fontSizeXL, &config);
    //
    //ImGui::PushFont(latoMedium);
    //currentFontSize = fontSizeM;

    


    ImGui::StyleColorsDark();
 
    // Init console window
    InitConsole();
    InitEntitiesManager();


#if ENABLE_DEBUG_PROFILE
    // Init performance window
    InitPerformance();
#endif


    std::fill(layersToSkip.begin(), layersToSkip.end(), true);
    std::fill(entitiesToSkip.begin(), entitiesToSkip.end(), true);
    std::fill(layersToLock.begin(), layersToLock.end(), true);
    std::fill(entitiesToLock.begin(), entitiesToLock.end(), true);

    initialized = true;
}

void GUIManager::Update()
{
    GLFWwindow* window = graphics.GetWindow();
    
    // blinks entire window if Drag & Drop file is released
    ImGuiStyle& style = ImGui::GetStyle();
    
    if (fileDropped && !firstSet) {
        
        // save the original style to revert later on
        originalStyle = style;
        firstSet = true;

        // change ImGui style to a faded appearance
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            ImVec4& col = style.Colors[i];
            col.x = std::min(col.x * 4.f, 1.0f);
            col.y = std::min(col.y * 4.f, 1.0f);
            col.z = std::min(col.z * 4.f, 1.0f);
        }
    }
    else if (!fileDropped) {
        style = originalStyle;
    }

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

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.23f, 0.30f, 1.00f);
    
    ImGui::Begin("Dockable Window", nullptr, window_flags);
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        
    }

    UpdateMenuBar();

    ImGui::End();
    
   /* if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);*/

    {
        ImGui::Begin("Game Viewport");

        if (ImGui::IsWindowHovered()) {
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            viewportWindowHovered = true;
        }
        else {
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
            viewportWindowHovered = false;
        }
        
        
 
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
        ImGuiWindow* ImWindow = ImGui::GetCurrentWindow();
        float w = ((xSizeAvailable * 9.f / 16.f) < ySizeAvailable) ? xSizeAvailable : (ySizeAvailable * 16.f / 9.f);
        float h = ((xSizeAvailable * 9.f / 16.f) < ySizeAvailable) ? (xSizeAvailable * 9.f / 16.f) : ySizeAvailable;
        graphics.viewport.SetViewport((int)ImWindow->DC.CursorPos.x,(int)(graphics.GetWindowHeight() - ImWindow->DC.CursorPos.y - h),(unsigned int)w,(unsigned int)h);
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(textureID)), ImVec2{ w , h }, ImVec2{ 0,1 }, ImVec2{ 1,0 });


        // need to edit popupHovered
        popupHovered = false;
        if (rightClick && anyObjectSelected) {
            
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::IsWindowHovered()) {
                    popupHovered = true;
                }
                if (ImGui::MenuItem("Copy")) {
                    toCopy = true;
                    rightClick = false;
                }
                ImGui::MenuItem("Paste", NULL, false, false);
                ImGui::Separator();
                if (ImGui::MenuItem("Delete")) {
                    toDestroy = true;
                    rightClick = false;
                    
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Bring Forward")) {
                    //
                    LayerOrderBringForward(newSelection);
                    rightClick = false;

                }
                if (ImGui::MenuItem("Bring to Front")) {
                    //
                    LayerOrderBringToFront(newSelection);
                    rightClick = false;

                }
                if (ImGui::MenuItem("Send Backward")) {
                    //
                    LayerOrderSendBackward(newSelection);
                    rightClick = false;

                }
                if (ImGui::MenuItem("Send to Back")) {
                    //
                    LayerOrderSendToBack(newSelection);
                    rightClick = false;

                }
                ImGui::EndPopup();
            }
        }
        //selectedEntities.clear();





        ImGui::End();
    }






  
    // Update Panels
    UpdateConsole();
    UpdateEntitiesManager();
    UpdatePlayStop();
    UpdateAssetLibrary();
    UpdateSceneHierachy();
    UpdateContentBrowser();
    UpdateLayer();


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

    dropTimer = ((dropTimer - g_dt) <= 0.f) ? 0.f : (dropTimer - g_dt);

    if (dropTimer == 0.f) {
        fileDropped = false;
        firstSet = false;
    }
    
}
