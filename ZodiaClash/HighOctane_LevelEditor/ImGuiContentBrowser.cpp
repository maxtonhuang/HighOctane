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
*	@file		ImGuiContentBrowser.cpp
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file is for the ImGui Content Browser where users can simply drag and drop
*	assets from the content browser to edit assets accordingly.
*
*	!------M2 Milestone------!
*	- Currently only allows scene files to be dragged into the scene and it will change scene
******************************************************************************/
#include <filesystem>
#include "ImGuiContentBrowser.h"
#include "AssetManager.h"
#include <map>

static std::filesystem::path s_AssetPath = "../Assets/";


extern std::map<std::string, ImTextureID> loadedIcons;

void UpdateContentBrowser() {
	static std::filesystem::path currentDirectory = s_AssetPath;
	static bool init{ true }; //THIS IS TEMPORARY PLEASE FIND ANOTHER WAY
	if (init) {
		s_AssetPath = assetmanager.GetDefaultPath();
		currentDirectory = s_AssetPath;
		init = false;
	}

	ImGui::Begin("Content Browser");

	if (currentDirectory != std::filesystem::path(s_AssetPath)) {
		if (ImGui::ImageButton(loadedIcons["backIcon"], ImVec2{ 32,32 })) {
			currentDirectory = currentDirectory.parent_path();
			if (currentDirectory < s_AssetPath) {
				currentDirectory = s_AssetPath;
			}
		}
	}

	static float padding = 16.0f;
	static float thumbnailSize = 64.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1) {
		columnCount = 1;
	}

	ImGui::Columns(columnCount, 0, false);


	for (auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
		const auto& path = entry.path();
		auto relativePath = std::filesystem::relative(path, s_AssetPath);
		std::string filenameString = relativePath.filename().string();
		ImGui::PushID(filenameString.c_str());
		// Check if the entry is a directory or a file
		bool isDirectory = entry.is_directory();
		// Use the appropriate icon based on the entry type
		ImTextureID iconID = isDirectory ? loadedIcons["folderIcon"] : loadedIcons["fileIcon"];

		// Display the icon and name
		ImGui::ImageButton(iconID, { thumbnailSize, thumbnailSize }, { 1, 0 }, { 0, 1 });

		// Handle the directory or file button click
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !isDirectory && relativePath.has_extension() && relativePath.extension() == ".scn") {

				if (ImGui::BeginDragDropSource()) {
					ImGui::SetDragDropPayload("SCENE_ITEM", filenameString.c_str(), (strlen(filenameString.c_str()) + 1));
					ImGui::EndDragDropSource();
				}
			}

		if (ImGui::IsItemClicked(0)) {
			if (isDirectory) {
				// Handle directory click
				std::cout << "Directory selected: " << currentDirectory / path.filename() << std::endl;
				currentDirectory /= path.filename();
			}
		}
		/*if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (entry.is_directory())
				currentDirectory /= path.filename();

		}*/

		// Use the correct file path with extension
		//std::string fullFilePath = (currentDirectory / path.filename()).string();

		// Check if it's a .scn file for drag-and-drop
		ImGui::TextWrapped(filenameString.c_str());
		ImGui::NextColumn();
		ImGui::PopID();
	}
		ImGui::Columns(1);

	
	ImGui::End();
}
