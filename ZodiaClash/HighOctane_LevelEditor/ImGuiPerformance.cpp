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
*	@file		ImGuiPerformance.cpp
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief		Performance window for ImGui
*
*	This file contains all the definition of the functions for the performance window
*
******************************************************************************/

#include "ImGuiPerformance.h"
#include "DebugProfile.h"
#include "GUIManager.h"


#if ENABLE_DEBUG_PROFILE
extern std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
extern DebugProfiling debugSysProfile;

constexpr float spacerHeight = 50.f;
constexpr int numOfFrames = 100;

float circularFPS[numOfFrames];

float currFPS{ 0 };
float minFPS{ std::numeric_limits<float>::max() };
float maxFPS{ 0 };
float avgFPS{ 0 };
float timerFPS{ 0 };
int tracker{ 0 };


/*!
* \brief Init the performance window
*
* This function is responsible for initialising the performance window
* It will be called in ImGuiManager.cpp
*
*/

void InitPerformance() {

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

}

/*!
* \brief Update the performance window
*
* This function is responsible for updating the performance window
* It will be called in ImGuiManager.cpp
*
*/
void UpdatePerformance() {
    //ImVec2 windowSize(300.f, systemList.size() * 80.f);
    //ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);

    ImGui::Begin("Percent Usage");

    /************** FPS ***************/
    currFPS = 1.f / g_dt; // Calculates the current FPS
    circularFPS[tracker++] = currFPS; // Store the current FPS in the circular array (overwrites the oldest FPS value)
    tracker %= numOfFrames; // Reset the tracker to 0 if it exceeds the number of frames
    minFPS = std::numeric_limits<float>::max();
    maxFPS = 0;
    avgFPS = 0;
    for (float val : circularFPS) { // Finds the min, max and average FPS
		minFPS = std::min(val, minFPS);
		maxFPS = std::max(val, maxFPS);
        avgFPS += val;
	}
    avgFPS /= numOfFrames;
    
    ImGui::PushFont(latoLargeBold);
    ImGui::Text("FPS: %.1f", currFPS);
    ImGui::PopFont();
    ImGui::Text("(%.3fms per Frame)", g_dt * 1000.0f);
    ImGui::Text("ImGUI Internal FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("In the last %d frames:", numOfFrames);
    ImGui::Text("   - Lowest FPS: %.1f", minFPS);
    ImGui::Text("   - Highest FPS: %.1f", maxFPS);
    ImGui::Text("   - Average FPS: %.1f", avgFPS);
    /************** FPS ***************/

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

    /************** PERFORMANCE USAGE ***************/
    ImGui::Text("CPU usage: %.3f %%", GetCPUUsage());
    ImGui::Text("Memory usage: %.3f MB", GetMemoryUsage());
    /************** PERFORMANCE USAGE ***************/

    /************** LEVEL EDITOR USAGE ***************/
    // Separate each bar with a separator
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

    float barHeight = std::max(10.f, (ImGui::GetContentRegionAvail().y / (systemList.size() + 1)) - spacerHeight);


    std::string LEName = "Level Editor";
    float LEPercentage = debugSysProfile.GetPercentage(LEName);

    // Name of the histogram
    std::string LEhistogramName = LEName;

    ImVec4 progressBarColorLE = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    if (LEPercentage > 70.0f) {
        progressBarColorLE = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red if > 70%
    }
    else if (LEPercentage > 30.0f) {
        progressBarColorLE = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow if > 30%
    }

    // Horizontal histogram

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, progressBarColorLE);
    ImGui::ProgressBar(LEPercentage / 100.0f, ImVec2(-1, barHeight), "");
    ImGui::PopStyleColor();

    // Position for the percentage text
    ImGui::SetCursorPos(ImVec2(20.f, ImGui::GetCursorPosY()));

    // Percentage text
    ImGui::Text("%s %.2f%%", LEhistogramName.c_str(), LEPercentage);

    // Separate each bar with a separator
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

    // Customize the separator's appearance
    ImGuiStyle& styleLE = ImGui::GetStyle();
    styleLE.ItemSpacing.y = 10.0f;

    /************** LEVEL EDITOR USAGE ***************/
    for (const auto& [system, sysName] : systemList) {

        float percentage = debugSysProfile.GetPercentage(sysName);

        // Name of the histogram
        std::string histogramName = sysName;

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
        ImGui::ProgressBar(percentage / 100.f, ImVec2(-1, barHeight), "");
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

    debugSysProfile.ResetTimers();

    ImGui::End();
}

#endif