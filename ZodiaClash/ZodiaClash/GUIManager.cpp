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

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    extern std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
    extern DebugProfiling debugSysProfile;



#endif

  //  std::unordered_map<std::string, Entity> masterEntitiesList;



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

bool usageWindow;
bool consoleWindow;
bool debugWindow;

void GUIManager::Init(GLFWwindow* window)
{
    
    show_demo_window = true;

    #if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
        usageWindow = true;
        consoleWindow = true;
        debugWindow = true;

    #endif
        
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
            ECS::ecs().AddComponent<Body>(testEntity, Body{});
            ECS::ecs().AddComponent<Collider>(testEntity, Collider{});
            entityAddedFlag = true;
            counter++;
            DebugPrint("entity ID: %i",testEntity)
        }
        ImGui::Text("Entities added = %d", counter);
        if (ImGui::Button("Clone Entity")) {
            if (!entityAddedFlag) {
                Assert(!entityAddedFlag, "Entity has not been added.");
            }
            else {
                CloneMasterModel2(0.f, 0.f, true);
            }
        } 
        if (ImGui::Button("Save my Cat")) {
            if (!entityAddedFlag) {
                Assert(!entityAddedFlag, "Entity has not been added.");
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

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    if (debugWindow) {
        ImGui::Begin("Debug Window", &debugWindow);

        // For creating a tab bar
        if (ImGui::BeginTabBar("DebugTabs")) {

            /************** TAB 1 ***************/
            if (ImGui::BeginTabItem("Console Tab")) {

                // Button to toggle the console window
                if (ImGui::Button(consoleWindow ? "Console On" : "Console Off")) {
                    consoleWindow = !consoleWindow;
                }

                if (ImGui::Button("Clear")) {
                    imguiOutputBuffer.ClearBuffer();
                }

                ImGui::EndTabItem();
            }
            /************** TAB 1 ***************/

            /************** TAB 2 ***************/
            if (ImGui::BeginTabItem("Usage Tab")) {

                if (ImGui::Button(usageWindow ? "Usage On" : "Usage Off")) {
					usageWindow = !usageWindow;
				}
            }
            /************** TAB 2 ***************/

            // End the tab bar
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

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
        for (int i = 0; i < valuesCount; ++i) {
            float percentage = debugSysProfile.GetPercentage(systemList[i].first);

            // Change this to system name in the future when max implemented it
            std::string histogramName = systemList[i].second;

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

#endif

#if ENABLE_DEBUG_DIAG

    // For the filter buffer for the console window
    static char filterBuffer[256] = "";
    static char fileNameBuffer[31] = "";
    static bool autoScroll = false;
    // Change the colour for my console window
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Change to your desired color

    // Debug window
    if (consoleWindow) {
        if (ImGui::Begin("Console")) {

            // Push the width of the input text box
            ImGui::PushItemWidth(200);

            /********FILE INPUT TEXT*********/
            ImGui::InputText("File Name", fileNameBuffer, IM_ARRAYSIZE(fileNameBuffer), ImGuiInputTextFlags_CharsNoBlank);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)"); // Label text
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Enter name of file. Eg. 'Good' will create a file called 'Good.log' ");
            }
            /********FILE INPUT TEXT*********/

            // Export to file button
            ImGui::SameLine();
            if (ImGui::Button("Export to File")) {
                // Call a function to export the console content to a file
                ExportConsoleToFile(fileNameBuffer);
            }
            ImGui::Separator();

            /********FILTER INPUT TEXT*********/
            ImGui::InputText("Filter", filterBuffer, IM_ARRAYSIZE(filterBuffer));
            ImGui::SameLine();
            ImGui::TextDisabled("(?)"); // Label text
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Filters console. Is case-insensitive");
            }
            /********FILTER INPUT TEXT*********/

            // Pop the input text box width
            ImGui::PopItemWidth();
            ImGui::Separator();

            // Checkbox for the auto scroll
            ImGui::Checkbox("Auto-Scroll", &autoScroll);
            ImGui::Separator();

            // For clearing buffer
            if (ImGui::Button("Clear")) {
                imguiOutputBuffer.ClearBuffer();
            }
            ImGui::Separator();

            // Create a scrolling region for the content
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            // Display the captured output in the text box
            const std::string& logBuffer = imguiOutputBuffer.GetBuffer();
            size_t pos = 0;
            while (pos < logBuffer.size()) {
                size_t newlinePos = logBuffer.find('\n', pos);
                if (newlinePos != std::string::npos) {
                    std::string logEntry = logBuffer.substr(pos, newlinePos - pos);

                    // Check if the log entry contains the filter string (case-insensitive)
                    std::string logEntryLowerCase = logEntry;
                    std::string filterLowerCase = filterBuffer;

                    // Convert both strings to lower case
                    std::transform(logEntryLowerCase.begin(), logEntryLowerCase.end(), logEntryLowerCase.begin(), ::tolower);
                    std::transform(filterLowerCase.begin(), filterLowerCase.end(), filterLowerCase.begin(), ::tolower);

                    /********CUSTOM COLLOUR OUTPUT*********/
                    ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default to white text color

                    if (logEntryLowerCase.find("[trace]") != std::string::npos) {
                        textColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Grey for trace messages
                    }
                    else if (logEntryLowerCase.find("[debug]") != std::string::npos) {
                        textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow for debug messages
                    }
                    else if (logEntryLowerCase.find("[info]") != std::string::npos) {
                        textColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);  // Blue for info messages
                    }
                    else if (logEntryLowerCase.find("[warning]") != std::string::npos) {
                        textColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);  // Orange for warning messages
                    }
                    else if (logEntryLowerCase.find("[error]") != std::string::npos) {
                        textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red for error messages
                    }
                    else if (logEntryLowerCase.find("[fatal]") != std::string::npos) {
                        textColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);  // Bright red for fatal messages
                    }
                    /********CUSTOM COLLOUR OUTPUT*********/

                    // Set the text color
                    if (logEntryLowerCase.find(filterLowerCase) != std::string::npos) {
                        //ImGui::TextUnformatted(logEntry.c_str());
                        ImGui::TextColored(textColor, logEntry.c_str());
                    }
                    pos = newlinePos + 1;
                }
                else {
                    break;
                }
            }

            // For automatically scrolling if the auto-scroll checkbox is ticked
            if (autoScroll) {
                ImGui::SetScrollHereY(1.0f);
            }

            // End the scrolling region
            ImGui::EndChild();

            // End the console window
            ImGui::End();

        }
    }

    // Stops changing the colour
    ImGui::PopStyleColor();

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

/*******************************************************************/
// ALL THESE IS FOR DEBUGGING PLEASE DO NOT TOUCH AT ALL
int ImGuiOutputBuffer::overflow(int c) {
    if (c != EOF) {
        // Append the character to a buffer
        buffer += static_cast<char>(c);
    }
    return c;
}

const std::string& ImGuiOutputBuffer::GetBuffer() const {
    return buffer;
}

void ImGuiOutputBuffer::ClearBuffer() {
    buffer.clear();
}

// Define the instance
ImGuiOutputBuffer imguiOutputBuffer;

// Redirect std::cout to use the custom stream buffer
std::ostream imguiCout(&imguiOutputBuffer);
std::streambuf* coutBuf = std::cout.rdbuf(imguiCout.rdbuf());

void ExportConsoleToFile(const char* fileName) {
    std::string fullFileName;

    fileName&& fileName[0] != '\0' ? fullFileName = std::string(fileName) + ".log" : fullFileName = "Console.log";;

    std::ofstream outputFile(fullFileName);

    Assert(!outputFile.is_open(), "Unable to open file");

    // Get the console content and write it to the file
    const std::string& logBuffer = imguiOutputBuffer.GetBuffer();
    outputFile << logBuffer;

    // Close the file
    outputFile.close();

    // Optionally, clear the console content after exporting
    imguiOutputBuffer.ClearBuffer();

    // Optionally, display a success message
    std::cout << "Console content exported to '" << fullFileName << "'." << std::endl;
}