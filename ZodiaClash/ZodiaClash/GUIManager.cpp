#include "GUIManager.h"
#include "debugdiagnostic.h"
#include "DebugProfile.h"
#include "enginecore.h"
#include "debuglog.h"

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    extern std::vector<std::shared_ptr<System>> systemList;
    extern DebugProfiling debugSysProfile;
#endif

GUIManager guiManager;
GUIManager::GUIManager()
{
	ImGui::CreateContext();
    //GUIWindow = nullptr;
}

GUIManager::~GUIManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


// Our state
bool show_demo_window;
bool show_another_window;

bool usageWindow;
bool debugWindow;

void GUIManager::Init(GLFWwindow* window)
{
    show_demo_window = true;
    show_another_window = true;

    #if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
        usageWindow = true;
        debugWindow = true;
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

}

void GUIManager::Update(GLFWwindow* window)
{



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

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    if (usageWindow) {
        // Number of data points
        size_t valuesCount = systemList.size();
        float progressbarHeight = 30.0f;

        ImVec2 windowSize(300.f, valuesCount * 80.f);
        //ImVec2 windowPos(100, 100);

        // For setting a fixed size for the window
        //ImGui::SetNextWindowPos(windowPos, 0); // Set the position
        ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);
        ImGui::Begin("Percent Usage", &usageWindow, ImGuiWindowFlags_NoResize);

        // For the plotting of the horizontal histogram
        for (size_t i = 0; i < valuesCount; ++i) {
            float percentage = debugSysProfile.GetPercentage(systemList[i]);

            // Change this to system name in the future when max implemented it
            std::string histogramName = "System " + std::to_string(i);

            // Create a group to hold the horizontal histogram and text side by side
            ImGui::BeginGroup();

            // For calculating the colour
            // Default to green
            ImVec4 progressBarColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            if (percentage > 70.0f) {

                // Red if > 70%
                progressBarColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
            else if (percentage > 30.0f) {

                // Yellow if > 30%
                progressBarColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            }

            // Horizontal histogram
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, progressBarColor);
            ImGui::ProgressBar(percentage / 100.0f, ImVec2(-1, progressbarHeight), "");
            ImGui::PopStyleColor();

            // For the position of the percentage text
            ImGui::SetCursorPos(ImVec2(20.f, ImGui::GetCursorPosY()));

            // For the percentage text
            ImGui::Text("%s %.2f%%", histogramName.c_str(), percentage);

            // End the group
            ImGui::EndGroup();

            // Separate each bar with a separator
            if (i < valuesCount - 1) {
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

                // For customising the separator's appearance
                ImGuiStyle& style = ImGui::GetStyle();
                style.ItemSpacing.y = 10.0f;
            }
        }
        ImGui::End();
    }

    // Debug window
    if (debugWindow) {

        // For opening and closing the window
        ImGui::Begin("Debug Window", &debugWindow);

        ImGui::Text("This is a debugging windows for all the debugging needs");

        // Buttons
        if (ImGui::Button("Open/Close performance window"))
			usageWindow = !usageWindow;

        // Example log messages
        ImGui::Text("This is a regular log message.");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This is an error message in red.");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "This is a warning message in yellow.");
        
        ImGui::TextDisabled("This is a disabled text.");

        //ImGui::TextUnformatted(debuglog::logger.GetLogContent().c_str());

        ImGui::End();
    }

#endif

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    //glViewport(0, 0, display_w, display_h);
   // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
   // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   // glfwSwapBuffers(window);
}
