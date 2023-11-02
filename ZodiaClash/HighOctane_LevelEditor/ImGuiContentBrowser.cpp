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

		// Check if the entry is a directory or a file
		bool isDirectory = entry.is_directory();

		// Use the appropriate icon based on the entry type
		ImTextureID iconID = isDirectory ? loadedIcons["folderIcon"] : loadedIcons["fileIcon"];

		// Display the icon and name
		ImGui::ImageButton(iconID, { thumbnailSize, thumbnailSize }, { 1, 0 }, { 0, 1 });

		// Handle the directory or file button click
		if (ImGui::IsItemClicked(0)) {
			if (isDirectory) {
				// Handle directory click
				std::cout << "Directory selected: " << currentDirectory / path.filename() << std::endl;
				currentDirectory /= path.filename();
			}
		}

		// Use the correct file path with extension
		//std::string fullFilePath = (currentDirectory / path.filename()).string();

		// Check if it's a .scn file for drag-and-drop
		if (!isDirectory && relativePath.has_extension() && relativePath.extension() == ".scn") {
			if (ImGui::BeginDragDropSource()) {
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();

				ImGui::SetDragDropPayload("SCENE_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				//wprintf(L"Original Path: %ls\n", itemPath);
				std::wcout << L"Original Path: " << itemPath << std::endl;
				ImGui::EndDragDropSource();
			}
		}
		ImGui::TextWrapped(filenameString.c_str());
		ImGui::NextColumn();
	}
		ImGui::Columns(1);

	
	ImGui::End();
}
