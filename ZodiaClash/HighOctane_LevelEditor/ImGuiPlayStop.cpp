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
* @file		ImGuiPlayStop.cpp
*
* @author		Maxton Huang Xinghua
*
* @email		m.huang\@digipen.edu
*
* @course		CSD 2401 - Software Engineering Project 3
* CSD 2451 - Software Engineering Project 4
*
*@section	Section A
*
*@date		3 November 2023
*
******************************************************************************
*
*@brief		Functions to display the Play, Stop, and Pause buttons
*
* -
*
******************************************************************************/

#include "ImGuiPlayStop.h"
#include "Global.h"
#include "Serialization.h"
#include "AssetManager.h"
#include "Layering.h"
#include "Events.h"
#include "ImGuiIcons.h"
#include "ECS.h"

bool buffer = false;
bool playIcon = false;
bool pausePressed = false;
/******************************************************************************
*
*	@brief Creates the Play, Stop, and Pause Control Panel
*
*	This function creates the Play, Stop, and Pause Buttons. When the Play
*	button is clicked, the game will start. When the Stop button is clicked,
*	the game will stop and the scene will be reset. When the Pause button is
*	clicked, the game will pause.
*
******************************************************************************/

void UpdatePlayStop() {

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize;
    ImGui::Begin("Controls", nullptr, window_flags);
    // Calculate the width of the buttons
    float buttonWidth = 25.0f;

    // Calculate the total width of the buttons
    float totalWidth = 2 * buttonWidth + ImGui::GetStyle().ItemSpacing.x;

    // Calculate the starting X position to center the buttons
    float startPosX = (ImGui::GetWindowWidth() - totalWidth) / 2;

    // Set the cursor position to start drawing buttons
    ImGui::SetCursorPosX(startPosX);
    if (!playIcon) {
        if (ImGui::ImageButton(loadedIcons["playIcon"], ImVec2{ buttonWidth, buttonWidth })) {
            playIcon = true;
            if (GetCurrentSystemMode() != SystemMode::PAUSE && GetCurrentSystemMode() != SystemMode::GAMEHELP) {
                SetCurrentSystemMode(SystemMode::RUN);
            }
            if (!pausePressed) {
                playButton = true;
            }
            button_clicked = true;
        }
    }else {
        if (ImGui::ImageButton(loadedIcons["stopIcon"], ImVec2{ buttonWidth, buttonWidth })) {
            playIcon = false;
            if (!playButton && GetCurrentSystemMode() != SystemMode::PAUSE && GetCurrentSystemMode() != SystemMode::GAMEHELP) {
                stopBuffer = true;
                events.Call("Change Scene", "tmp.scn");
                SetCurrentSystemMode(SystemMode::EDIT);
            }
        }
    }


    ImGui::SameLine();

    if (ImGui::ImageButton(loadedIcons["pauseIcon"], ImVec2{ buttonWidth, buttonWidth })) {
        if (playIcon) {
            playIcon = false;
            pausePressed = true;
            if (GetCurrentSystemMode() != SystemMode::PAUSE && GetCurrentSystemMode() != SystemMode::GAMEHELP) {
                SetCurrentSystemMode(SystemMode::EDIT);
            }
        }
    }

	ImGui::End();

}