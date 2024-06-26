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
*	@file		ImGuiTileMap.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
**
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		29 February 2024
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
#include "ECS.h"
#include "model.h"


/******************************************************************************
*
*	@brief Provides the options for the Grid Layout for Tilemap Editing
*
*	Allows the user to turn on the tilemap, set the grid spacing, grid offset,
*	and the grid alignment. The user can also choose to lock the grid spacing
*	and grid offset. The user can also choose the corner of the grid to snap
* 	tiles to.
*
******************************************************************************/
void UpdateTilemap() {

	static bool lockSpacing = false;
	static bool lockOffset = false;

	ImGui::Begin("Tilemap Control");
	
	if(ImGui::Checkbox("Turn on Tilemap", &tilemapOn)) {
		if (tilemapOn) {
			snappingOn = false;
		}
	}

	if (tilemapOn) {
		ImGui::SeparatorText("Grid Spacing");

		ImGui::Text("X-axis: ");
		if (ImGui::SliderFloat("##XSpacing", &gridSpacingX, 50.f, 300.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
			if (lockSpacing) {
				gridSpacingY = gridSpacingX;
			}
		}

		ImGui::SameLine();

		ImGui::PushID("R_SizeX");
		if (ImGui::Button("Reset")) {
			gridSpacingX = 200.f;
			if (lockSpacing) {
				gridSpacingY = 200.f;
			}
		}
		ImGui::PopID();

		ImGui::Text("Y-axis: ");
		if (ImGui::SliderFloat("##Yspacing", &gridSpacingY, 50.f, 300.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
			if (lockSpacing) {
				gridSpacingX = gridSpacingY;
			}
		}

		ImGui::SameLine();

		ImGui::PushID("R_SizeY");
		if (ImGui::Button("Reset")) {
			gridSpacingY = 200.f;
			if (lockSpacing) {
				gridSpacingX = 200.f;
			}
		}
		ImGui::PopID();

		if (ImGui::Checkbox("Lock Spacing X and Y", &lockSpacing)) {
			gridSpacingY = gridSpacingX;
		}

		ImGui::SeparatorText("Grid Offset");

		ImGui::Text("X-axis: ");
		if (ImGui::SliderFloat("##Xoffset", &gridOffsetX, -50.f, 50.f, "%.3f%%", ImGuiSliderFlags_AlwaysClamp)) {
			if (lockOffset) {
				gridOffsetY = gridOffsetX;
			}
		}

		ImGui::SameLine();

		ImGui::PushID("R_OffsetX");
		if (ImGui::Button("Reset")) {
			gridOffsetX = 0.f;
			if (lockOffset) {
				gridOffsetY = 0.f;
			}
		}
		ImGui::PopID();

		ImGui::Text("Y-axis: ");
		if (ImGui::SliderFloat("##Yoffset", &gridOffsetY, -50.f, 50.f, "%.3f%%", ImGuiSliderFlags_AlwaysClamp)) {
			if (lockOffset) {
				gridOffsetX = gridOffsetY;
			}
		}

		ImGui::SameLine();

		ImGui::PushID("R_OffsetY");
		if (ImGui::Button("Reset")) {
			gridOffsetY = 0.f;
			if (lockOffset) {
				gridOffsetX = 0.f;
			}
		}
		ImGui::PopID();

		if (ImGui::Checkbox("Lock Offset X and Y", &lockOffset)) {
			gridOffsetY = gridOffsetX;
		}

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

		ImGui::SeparatorText("Resize Grid to Single Selected Object");

		if (ImGui::Button("Resize Width", ImVec2(125.f, 35.f))) {
			if (selectedEntities.size() == 1) {
				Entity e = selectedEntities.front();
				if (ECS::ecs().HasComponent<Model>(e)) {
					gridSpacingX = ECS::ecs().GetComponent<Model>(e).GetMax().x - ECS::ecs().GetComponent<Model>(e).GetMin().x;
					if (lockSpacing) {
						gridSpacingY = gridSpacingX;
					}
				}
			}
		}
		if (ImGui::Button("Resize Height", ImVec2(125.f, 35.f))) {
			if (selectedEntities.size() == 1) {
				Entity e = selectedEntities.front();
				if (ECS::ecs().HasComponent<Model>(e)) {
					gridSpacingY = ECS::ecs().GetComponent<Model>(e).GetMax().y - ECS::ecs().GetComponent<Model>(e).GetMin().y;
					if (lockSpacing) {
						gridSpacingX = gridSpacingY;
					}
				}
			}
		}
	}

	ImGui::End();

}
