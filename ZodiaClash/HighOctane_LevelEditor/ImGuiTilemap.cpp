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



void UpdateTilemap() {

	ImGui::Begin("Tilemap Control");

	ImGui::SeparatorText("Grid Spacing");

	ImGui::Text("X-axis: ");

	int slider_i = 20; // to change
	ImGui::SliderInt("10 - 500", &slider_i, 10, 500, "%d", ImGuiSliderFlags_AlwaysClamp);


	ImGui::Text("Y-axis: ");
	ImGui::SliderInt("10 - 500", &slider_i, 10, 500, "%d", ImGuiSliderFlags_AlwaysClamp);

	ImGui::SeparatorText("Grid Offset");

	ImGui::Text("X-axis: ");
	ImGui::SliderInt("-50% - +50%", &slider_i, 10, 500, "%d%%", ImGuiSliderFlags_AlwaysClamp);

	ImGui::Text("Y-axis: ");
	ImGui::SliderInt("-50% - +50%", &slider_i, 10, 500, "%d%%", ImGuiSliderFlags_AlwaysClamp);

	ImGui::SeparatorText("Snap To");

	ImGui::Text("Corner of grid to snap tiles");

	

	// check box x 9




	


	ImGui::End();




}