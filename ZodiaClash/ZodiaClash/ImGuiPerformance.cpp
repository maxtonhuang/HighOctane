#include "ImGuiPerformance.h"
#include <iostream>
#include <vector>
#include "enginecore.h"
#include "ecs.h"
#include "DebugProfile.h"

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
extern std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
extern DebugProfiling debugSysProfile;

void InitPerformance(GLFWwindow* window) {
    const char* glsl_version = "#version 450";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

}

void UpdatePerformance(GLFWwindow* window) {

    // Number of data points
    size_t valuesCount = systemList.size();
    float progressbarHeight = 30.0f;
    
    ImVec2 windowSize(300.f, valuesCount * 80.f);
    //ImVec2 windowPos(100, 100);
    
    // For setting a fixed size for the window
    //ImGui::SetNextWindowPos(windowPos, 0); // Set the position
    ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);
    ImGui::Begin("Percent Usage");
    
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