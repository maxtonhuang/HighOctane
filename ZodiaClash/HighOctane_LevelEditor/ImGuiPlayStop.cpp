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

bool buffer = false;
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

	ImGui::Begin("Controls");


	if (ImGui::Button("Play", {100,50})) {
		playButton = true;
		edit_mode = false;
		button_clicked = true;
	}
	ImGui::SameLine();
	if (buffer == true) {
		std::string loadPath = assetmanager.GetDefaultPath() + "Scenes/tmp.json";
		Serializer::LoadEntityFromJson(loadPath);
		
		buffer = false;
	}
	if (ImGui::Button("Stop", { 100,50 })) {
		events.Call("Change Scene", "tmp.scn");
		edit_mode = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause", { 100,50 })) {
		edit_mode = true;
	}

	ImGui::End();

}