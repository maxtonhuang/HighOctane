/*
\copyright
        All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
        reserved.Reproduction or disclosure of this file or its contents without the prior
        written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		ImGuiPerformance.cpp
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		23 September 2023
@brief		This file contains the functions definitions for performance window in ImGui

*//*______________________________________________________________________*/

#include "ImGuiPerformance.h"
#include "enginecore.h"

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
extern std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
extern DebugProfiling debugSysProfile;

void InitPerformance() {

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

}

void UpdatePerformance() {
    ImVec2 windowSize(300.f, systemList.size() * 80.f);
    ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);
    ImGui::Begin("Percent Usage");

    /************** FPS ***************/
    ImGui::Text("Average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    /************** FPS ***************/

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

    /************** PERFORMANCE USAGE ***************/
    ImGui::Text("CPU usage: %.3f %%", GetCPUUsage(GetTime()));
    ImGui::Text("Memory usage: %.3f MB", GetMemoryUsage(GetTime()));
    /************** PERFORMANCE USAGE ***************/

    for (const auto& [system, name] : systemList) {
        float percentage = debugSysProfile.GetPercentage(system);

        // Name of the histogram
        std::string histogramName = name;

        // Create a group to hold the horizontal histogram and text side by side
        ImGui::BeginGroup();

        // Calculate the color
        ImVec4 progressBarColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        if (percentage > 70.0f) {
            progressBarColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red if > 70%
        }
        else if (percentage > 30.0f) {
            progressBarColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow if > 30%
        }

        // Horizontal histogram
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, progressBarColor);
        ImGui::ProgressBar(percentage / 100.0f, ImVec2(-1, 30.0f), "");
        ImGui::PopStyleColor();

        // Position for the percentage text
        ImGui::SetCursorPos(ImVec2(20.f, ImGui::GetCursorPosY()));

        // Percentage text
        ImGui::Text("%s %.2f%%", histogramName.c_str(), percentage);

        // End the group
        ImGui::EndGroup();

        // Separate each bar with a separator
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

        // Customize the separator's appearance
        ImGuiStyle& style = ImGui::GetStyle();
        style.ItemSpacing.y = 10.0f;
    }

    ImGui::End();
}

#endif