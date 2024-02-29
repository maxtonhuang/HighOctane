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
*	@file		ImGuiAssetLibrary.cpp
*
*	@author		Maxton Huang Xinghua (Initial creation as well as image drag and drop)
*
*	@email		m.huang\@digipen.edu
*
*   @co-author	Foong Pun Yuen Nigel (Audio drag and drop and error handling)
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		3 November 2023
*
* *****************************************************************************
*
*	@brief		Functions to update the Asset Library panel in the editor
*
*	The Asset Library panel is used to display all the master entities that
*	are available for the user to use. The user can also create new master
*	entities from this panel by importing images. New Master Entities will
*	automatically be created from the imported images. The user will also be
*	able to choose whether the image to be imported is a static image or a
*	spritesheet.
*
******************************************************************************/


#include "ImGuiTilemap.h"
#include "Global.h"



void UpdateTilemap() {

	ImGui::Begin("Tilemap Control");
	
	if(ImGui::Checkbox("Turn on Tilemap", &tilemapOn)) {
		if (tilemapOn) {
			snappingOn = false;
		}
	}

	if (tilemapOn) {
		ImGui::SeparatorText("Grid Spacing");

		ImGui::Text("X-axis: ");
		ImGui::SliderInt("##XSpacing", &gridSpacingX, 50, 300, "%d", ImGuiSliderFlags_AlwaysClamp);

		ImGui::Text("Y-axis: ");
		ImGui::SliderInt("##Yspacing", &gridSpacingY, 50, 300, "%d", ImGuiSliderFlags_AlwaysClamp);

		ImGui::SeparatorText("Grid Offset");

		ImGui::Text("X-axis: ");
		ImGui::SliderInt("##Xoffset", &gridOffsetX, -50, 50, "%d%%", ImGuiSliderFlags_AlwaysClamp);

		ImGui::Text("Y-axis: ");
		ImGui::SliderInt("##Yoffset", &gridOffsetY, -50, 50, "%d%%", ImGuiSliderFlags_AlwaysClamp);

		ImGui::SeparatorText("Snap To");

		ImGui::Text("Corner of grid to snap tiles");
		ImGui::Dummy(ImVec2(0.f, 5.f));
		for (int y = 0; y < 3; y++) {
			ImGui::Dummy(ImVec2(0.f, 5.f));
			for (int x = 0; x < 3; x++) {
				if (x > 0) ImGui::SameLine();
				ImGui::Dummy(ImVec2(5.f, 0.f));
				ImGui::SameLine();
				ImGui::PushID(3 * y + x);
				bool state = gridAlignment == (3 * y + x);
				if (ImGui::Checkbox("", &state)) {

					gridAlignment = (3 * y + x);
				}
				ImGui::PopID();
			}
		}
	}

	ImGui::End();




}