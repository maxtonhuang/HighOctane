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
#include "ImGuiAnimator.h"
#include "ImGuiComponents.h"
#include "ImGuiSceneSettings.h"

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

    std::string defaultPath{ assetmanager.GetDefaultPath() + "Fonts/" };
    std::string latoMediumPath{ defaultPath + "Lato/Lato-Regular.ttf" };
    std::string latoLargePath{ defaultPath + "Lato/Lato-Bold.ttf" };
    latoMedium = io.Fonts->AddFontFromFileTTF(latoMediumPath.c_str(), fontSizeM);
    latoLargeBold = io.Fonts->AddFontFromFileTTF(latoLargePath.c_str(), fontSizeXL);

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

    assetmanager.LoadAllPrefabs();


    ImGui::StyleColorsDark();
 
    // Init console window
    InitConsole();
    //InitEntitiesManager();


#if _DEBUG
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

    //style.Colors[ImGuiCol_WindowBg] = ImVec4(0.3f, 0.15f, 0.04f, 1.00f); // 0.18f, 0.23f, 0.30f, 1.00f
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

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ITEM")) {
                const char* droppedItemPath = (const char*)payload->Data;
                
                events.Call("Change Scene", droppedItemPath);
            }
            ImGui::EndDragDropTarget();
        }
        //popupHovered = false;
        if (rightClick && selectedEntities.size() > 0) {
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::MenuItem("Copy")) {
                    toCopy = true;
                    rightClick = false;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Delete")) {
                    toDestroy = true;
                    rightClick = false;
                    
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Bring Forward", NULL, false, selectedEntities.size() == 1 ? true : false)) {
                    //
                    LayerOrderBringForward(selectedEntities.front());
                    rightClick = false;
                }
                if (ImGui::MenuItem("Bring to Front", NULL, false, selectedEntities.size() == 1 ? true : false)) {
                    //
                    LayerOrderBringToFront(selectedEntities.front());
                    rightClick = false;
                }
                if (ImGui::MenuItem("Send Backward", NULL, false, selectedEntities.size() == 1 ? true : false)) {
                    //
                    LayerOrderSendBackward(selectedEntities.front());
                    rightClick = false;
                }
                if (ImGui::MenuItem("Send to Back", NULL, false, selectedEntities.size() == 1 ? true : false)) {
                    //
                    LayerOrderSendToBack(selectedEntities.front());
                    rightClick = false;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Align Top")) {
					//
                    auto& transformArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>();
                    auto& modelArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>();
                    float alignmentReference;
                    if (keyObjectID != std::numeric_limits<Entity>().max()) {
                        alignmentReference = transformArray.GetData(keyObjectID).position.y + ((modelArray.GetData(keyObjectID).GetMax().y - modelArray.GetData(keyObjectID).GetMin().y) / 2.f);
                    }
                    else {
                        alignmentReference = graphics.GetWindowHeight() * 0.5f;
                    }
                    for (Entity e : selectedEntities) {
                        transformArray.GetData(e).position.y = alignmentReference - ((modelArray.GetData(e).GetMax().y - modelArray.GetData(e).GetMin().y) / 2.f);
                    }
                    rightClick = false;
				}
                if (ImGui::MenuItem("Align Middle")) {
                    //
                    auto& transformArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>();
                    float alignmentReference;
                    if (keyObjectID != std::numeric_limits<Entity>().max()) {
                        alignmentReference = transformArray.GetData(keyObjectID).position.y;
                    }
                    else {
                        alignmentReference = 0.f;
                    }
                    for (Entity e : selectedEntities) {
                        transformArray.GetData(e).position.y = alignmentReference;
                    }
                    rightClick = false;
                }
                if (ImGui::MenuItem("Align Bottom")) {
                    //
                    auto& transformArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>();
                    auto& modelArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>();
                    float alignmentReference;
                    if (keyObjectID != std::numeric_limits<Entity>().max()) {
                        alignmentReference = transformArray.GetData(keyObjectID).position.y - ((modelArray.GetData(keyObjectID).GetMax().y - modelArray.GetData(keyObjectID).GetMin().y) / 2.f);
                    }
                    else {
                        alignmentReference = -graphics.GetWindowHeight() * 0.5f;
                    }
                    for (Entity e : selectedEntities) {
                        transformArray.GetData(e).position.y = alignmentReference + ((modelArray.GetData(e).GetMax().y - modelArray.GetData(e).GetMin().y) / 2.f);
                    }
                    rightClick = false;
                }
                if (ImGui::MenuItem("Align Left")) {
                    // 
                    auto& transformArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>();
                    auto& modelArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>();
                    float alignmentReference;
                    if (keyObjectID != std::numeric_limits<Entity>().max()) {
                        alignmentReference = transformArray.GetData(keyObjectID).position.x - ((modelArray.GetData(keyObjectID).GetMax().x - modelArray.GetData(keyObjectID).GetMin().x) / 2.f);
                    }
                    else {
                        alignmentReference = -graphics.GetWindowWidth() * 0.5f;
                    }
                    for (Entity e : selectedEntities) {
                        transformArray.GetData(e).position.x = alignmentReference + ((modelArray.GetData(e).GetMax().x - modelArray.GetData(e).GetMin().x) / 2.f);
                    }
                    rightClick = false;
                }
                if (ImGui::MenuItem("Align Center")) {
                    // 
                    auto& transformArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>();
                    float alignmentReference;
                    if (keyObjectID != std::numeric_limits<Entity>().max()) {
                        alignmentReference = transformArray.GetData(keyObjectID).position.x;
                    }
                    else {
                        alignmentReference = 0.f;
                    }
                    for (Entity e : selectedEntities) {
                        transformArray.GetData(e).position.x = alignmentReference;
                    }
                    rightClick = false;
                }
                if (ImGui::MenuItem("Align Right")) {
                    // 
                    auto& transformArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>();
                    auto& modelArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>();
                    float alignmentReference;
                    if (keyObjectID != std::numeric_limits<Entity>().max()) {
                        alignmentReference = transformArray.GetData(keyObjectID).position.x + ((modelArray.GetData(keyObjectID).GetMax().x - modelArray.GetData(keyObjectID).GetMin().x) / 2.f);
                    }
                    else {
                        alignmentReference = graphics.GetWindowWidth() * 0.5f;
                    }
                    for (Entity e : selectedEntities) {
                        transformArray.GetData(e).position.x = alignmentReference - ((modelArray.GetData(e).GetMax().x - modelArray.GetData(e).GetMin().x) / 2.f);
                    }
                    rightClick = false;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save as Prefab", NULL, false, selectedEntities.size() == 1 ? true : false)) {
                    std::string prefabPath{ SaveFileDialog("*.prefab","Prefab") };
                    SaveAsPrefab(prefabPath, newSelection);
                }
                ImGui::EndPopup();
            }
        }
        //selectedEntities.clear();





        ImGui::End();
    }






  
    // Update Panels
    UpdateConsole();
    //UpdateEntitiesManager();
    UpdatePlayStop();
    UpdateAssetLibrary();
    UpdateSceneHierachy();
    UpdateContentBrowser();
    UpdateLayer();
    UpdateAnimator();
    UpdateComponentViewer();
    UpdatePrefabHierachy();
    UpdateSceneSettingsWindow();

#if _DEBUG
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
