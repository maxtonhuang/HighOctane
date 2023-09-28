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

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    //extern std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
    //extern DebugProfiling debugSysProfile;



#endif

    //int bar_data[11] = { 20, 45, 70, 15, 90, 30, 50, 80, 35, 65, 25 };
    //float x_data[10] = { 0.2f, 0.5f, 0.8f, 0.1f, 0.9f, 0.3f, 0.6f, 0.7f, 0.4f, 0.25f };
    //float y_data[10] = { 0.4f, 0.1f, 0.7f, 0.9f, 0.2f, 0.6f, 0.3f, 0.8f, 0.5f, 0.75f };
    //struct ScrollingBuffer {
    //    int MaxSize;
    //    int Offset;
    //    ImVector<ImVec2> Data;
    //    ScrollingBuffer(int max_size = 2000) {
    //        MaxSize = max_size;
    //        Offset = 0;
    //        Data.reserve(MaxSize);
    //    }
    //    void AddPoint(float x, float y) {
    //        if (Data.size() < MaxSize)
    //            Data.push_back(ImVec2(x, y));
    //        else {
    //            Data[Offset] = ImVec2(x, y);
    //            Offset = (Offset + 1) % MaxSize;
    //        }
    //    }
    //    void Erase() {
    //        if (Data.size() > 0) {
    //            Data.shrink(0);
    //            Offset = 0;
    //        }
    //    }
    //};

    //// utility structure for realtime plot
    //struct RollingBuffer {
    //    float Span;
    //    ImVector<ImVec2> Data;
    //    RollingBuffer() {
    //        Span = 10.0f;
    //        Data.reserve(2000);
    //    }
    //    void AddPoint(float x, float y) {
    //        float xmod = fmodf(x, Span);
    //        if (!Data.empty() && xmod < Data.back().x)
    //            Data.shrink(0);
    //        Data.push_back(ImVec2(xmod, y));
    //    }
    //};

GUIManager guiManager;
GUIManager::GUIManager()
{
	ImGui::CreateContext();
    ImPlot::CreateContext();
    //GUIWindow = nullptr;
}

GUIManager::~GUIManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
}


// Our state
bool show_demo_window;

bool debugWindow;
bool plotWindow;

void GUIManager::Init(GLFWwindow* window)
{


   // show_demo_window = true;

    #if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
        debugWindow = true;
        plotWindow = true;

    #endif
        
    const char* glsl_version = "#version 450";

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Init console window
    InitConsole();
    InitEntitiesManager();
#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    // Init performance window
    InitPerformance();
#endif

}

void GUIManager::Update(GLFWwindow* window)
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

   /* if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);*/

    // Update the console
    UpdateConsole();
    UpdateEntitiesManager();
#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    // Update the performance console
    UpdatePerformance();
#endif


//#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
//    if (debugWindow) {
//        ImGui::Begin("Debug Window", &debugWindow);
//
//        // For creating a tab bar
//        if (ImGui::BeginTabBar("DebugTabs")) {
//
//            /************** TAB 1 ***************/
//            if (ImGui::BeginTabItem("Console Tab")) {
//
//                // Button to toggle the console window
//                if (ImGui::Button(consoleWindow ? "Console On" : "Console Off")) {
//                    consoleWindow = !consoleWindow;
//                }
//
//                if (ImGui::Button("Clear")) {
//                    imguiOutputBuffer.ClearBuffer();
//                }
//
//                ImGui::EndTabItem();
//            }
//            /************** TAB 1 ***************/
//
//            /************** TAB 2 ***************/
//            if (ImGui::BeginTabItem("Usage Tab")) {
//
//                if (ImGui::Button(usageWindow ? "Usage On" : "Usage Off")) {
//					usageWindow = !usageWindow;
//				}
//
//                ImGui::EndTabItem();
//            }
//            /************** TAB 2 ***************/
//
//            // End the tab bar
//            ImGui::EndTabBar();
//        }
//
//        ImGui::End();
//    }
//
//#endif
//
//#if ENABLE_DEBUG_DIAG
//
//    // Plot window
//    //if (plotWindow) {
//        //ImGui::BulletText("Move your mouse to change the data!");
//        //ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
//
//        //static ScrollingBuffer sdata1, sdata2;
//        //static RollingBuffer   rdata1, rdata2;
//        //ImVec2 mouse = ImGui::GetMousePos();
//        //static float t = 0;
//        //t += ImGui::GetIO().DeltaTime;
//        //sdata1.AddPoint(t, mouse.x * 0.0005f);
//        //rdata1.AddPoint(t, mouse.x * 0.0005f);
//        //sdata2.AddPoint(t, mouse.y * 0.0005f);
//        //rdata2.AddPoint(t, mouse.y * 0.0005f);
//
//        //static float history = 10.0f;
//        //ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
//        //rdata1.Span = history;
//        //rdata2.Span = history;
//
//        //static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
//
//        //if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150))) {
//        //    ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
//        //    ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
//        //    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
//        //    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
//        //    ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, 0, sdata1.Offset, 2 * sizeof(float));
//        //    ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2 * sizeof(float));
//        //    ImPlot::EndPlot();
//        //}
//        //if (ImPlot::BeginPlot("##Rolling", ImVec2(-1, 150))) {
//        //    ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
//        //    ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
//        //    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
//        //    ImPlot::PlotLine("Mouse X", &rdata1.Data[0].x, &rdata1.Data[0].y, rdata1.Data.size(), 0, 0, 2 * sizeof(float));
//        //    ImPlot::PlotLine("Mouse Y", &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(), 0, 0, 2 * sizeof(float));
//        //    ImPlot::EndPlot();
//        //}
//        //ImPlot::ShowDemoWindow();
//    //}
//#endif
//
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
