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
#include "MultiThreading.h"

GUIManager guiManager;
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
bool show_demo_window;

void GUIManager::Init(GLFWwindow* window)
{


    show_demo_window = true;

        
    const char* glsl_version = "#version 450";
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    /*window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL) {
        ASSERT(false, "Failed to create GUI window");
    }*/
    //glfwMakeContextCurrent(window);
    //glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Init console window
    InitConsole();

#if ENABLE_DEBUG_PROFILE
    // Init performance window
    InitPerformance();
#endif

}

void GUIManager::Update(GLFWwindow* window)
{
    //debugUpdate(window);
    

    //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Poll and handle events (inputs, window resize, etc.)
       // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
       // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
       // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
       // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    //glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static int counter = 0;
        static bool entityAddedFlag = false;
        ImGui::Begin("Creating Entity");                         

        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //Entity testEntity;
            Entity testEntity;			
        if (ImGui::Button("Create Entity")) {
            testEntity = ECS::ecs().CreateEntity();
            masterEntitiesList["CatTest"] = testEntity;
            ECS::ecs().AddComponent(testEntity, Color{ glm::vec4{ 1,1,1,1 } });
            ECS::ecs().AddComponent(testEntity, Transform{ Vec2{ 0.f,0.f }, 0.f, Vec2{ 1.f, 1.f }, vmath::Vector2{ 0,0 } });
            ECS::ecs().AddComponent(testEntity, Visible{ false });
            ECS::ecs().AddComponent(testEntity, Tex{ texList.Add("cat.png") });
            Tex* t = &ECS::ecs().GetComponent<Tex>(testEntity);
            t->texVariants.push_back(texList.Add("cat.png"));
            t->texVariants.push_back(texList.Add("cat.png"));
            t->tex = t->texVariants.at(0);
            ECS::ecs().AddComponent(testEntity, Animation{});
            Animation* a = &ECS::ecs().GetComponent<Animation>(testEntity);
            a->animationType = Animation::ANIMATION_TIME_BASED;
            a->frameDisplayDuration = 0.1f;
            ECS::ecs().AddComponent(testEntity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
            ECS::ecs().AddComponent(testEntity, Model{});
            ECS::ecs().AddComponent<physics::Body>(testEntity, physics::Body{});
            ECS::ecs().AddComponent<Collider>(testEntity, Collider{});
            entityAddedFlag = true;
            counter++;
            DEBUG_PRINT("entity ID: %i",testEntity)
        }
        ImGui::Text("Entities added = %d", counter);
        if (ImGui::Button("Clone Entity")) {
            if (!entityAddedFlag) {
                ASSERT(!entityAddedFlag, "Entity has not been added.");
            }
            else {
                CloneMasterModel2(0.f, 0.f, true);
            }
        } 
        if (ImGui::Button("Save my Cat")) {
            if (!entityAddedFlag) {
                ASSERT(!entityAddedFlag, "Entity has not been added.");
            }
            else {
                Serializer::SaveEntityToJson("../Assets/CatTestEntity.json", masterEntitiesList["CatTest"]);
            }
        }
        if (ImGui::Button("Load my Cat")) {
            Serializer::LoadEntityFromJson("../Assets/CatTestEntity.json");
            entityAddedFlag = true;
            
        }
        ImGui::SameLine();

        }
        ImGui::End();
    // Update the console
    UpdateConsole();

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
