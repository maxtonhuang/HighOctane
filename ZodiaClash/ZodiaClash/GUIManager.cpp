#include "GUIManager.h"
#include "debugdiagnostic.h"
#include "DebugProfile.h"
#include "enginecore.h"
#include "debuglog.h"
#include "ImGuiConsole.h"
#include "ImGuiPerformance.h"

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    //extern std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
    //extern DebugProfiling debugSysProfile;



#endif

    //int bar_data[11] = { 20, 45, 70, 15, 90, 30, 50, 80, 35, 65, 25 };
    //float x_data[10] = { 0.2f, 0.5f, 0.8f, 0.1f, 0.9f, 0.3f, 0.6f, 0.7f, 0.4f, 0.25f };
    //float y_data[10] = { 0.4f, 0.1f, 0.7f, 0.9f, 0.2f, 0.6f, 0.3f, 0.8f, 0.5f, 0.75f };
    struct ScrollingBuffer {
        int MaxSize;
        int Offset;
        ImVector<ImVec2> Data;
        ScrollingBuffer(int max_size = 2000) {
            MaxSize = max_size;
            Offset = 0;
            Data.reserve(MaxSize);
        }
        void AddPoint(float x, float y) {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x, y));
            else {
                Data[Offset] = ImVec2(x, y);
                Offset = (Offset + 1) % MaxSize;
            }
        }
        void Erase() {
            if (Data.size() > 0) {
                Data.shrink(0);
                Offset = 0;
            }
        }
    };

    // utility structure for realtime plot
    struct RollingBuffer {
        float Span;
        ImVector<ImVec2> Data;
        RollingBuffer() {
            Span = 10.0f;
            Data.reserve(2000);
        }
        void AddPoint(float x, float y) {
            float xmod = fmodf(x, Span);
            if (!Data.empty() && xmod < Data.back().x)
                Data.shrink(0);
            Data.push_back(ImVec2(xmod, y));
        }
    };

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
    ImPlot::CreateContext();
}


// Our state
bool show_demo_window;
bool show_another_window;

bool debugWindow;
bool plotWindow;

void GUIManager::Init(GLFWwindow* window)
{


    show_demo_window = true;
    show_another_window = true;

    #if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
        debugWindow = true;
        plotWindow = true;

    #endif

    //// GL 3.0 + GLSL 130
    const char* glsl_version = "#version 450";
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    /*window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL) {
        Assert(false, "Failed to create GUI window");
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
    InitConsole(window);

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    // Init performance window
    InitPerformance(window);
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
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // Update the console
    UpdateConsole(window);

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    // Update the performance console
    UpdatePerformance(window);
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
