#include <filesystem>
#include "ImGuiContentBrowser.h"
#include "AssetManager.h"
#include <map>

static std::filesystem::path s_AssetPath = "../Assets/";
std::string folderIconPath = "foldericon.png";
std::string fileIconPath = "fileicon.png";
std::string backIconPath = "backicon.png";
static std::map<std::string, ImTextureID> loadedIcons;

ImTextureID LoadIconTexture(const std::string& iconPath) {
	// Create an instance of Texture
	Texture iconTexture;

	// Define a path to your icons directory in the AssetManager
	std::string pathToIcons = assetmanager.GetDefaultPath() + "Textures/Icons/";

	// Construct the full path to the icon within the icons directory
	std::string fullPath = pathToIcons + iconPath;

	// Initialize the icon texture
	iconTexture.Init(fullPath.c_str(), iconPath.c_str());

	// Check if the icon texture was loaded successfully
	if (iconTexture.IsActive()) {
		// Return the ImTextureID from the loaded texture
		return reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(iconTexture.GetID()));
	}

	else {
		// Return nullptr if loading failed (you may want to handle errors differently)
		return nullptr;
	}
}

void LoadIcons() {
	loadedIcons["folderIcon"] = LoadIconTexture("foldericon.png");
	loadedIcons["fileIcon"] = LoadIconTexture("fileicon.png");
	loadedIcons["backIcon"] = LoadIconTexture("backicon.png");
}

void UnloadIcons() {
	for (const auto& iconPair : loadedIcons) {
		ImTextureID imTextureID = iconPair.second;
		Texture* iconTexture = reinterpret_cast<Texture*>(imTextureID);

		/*if (iconTexture) {
			GLuint textureID = imTextureID->GetID();
			glDeleteTextures(1, &textureID);
		}*/
		glDeleteTextures(1, reinterpret_cast<GLuint*>(&imTextureID));
	}

	loadedIcons.clear();
}

void UpdateContentBrowser(){
	static std::filesystem::path currentDirectory = s_AssetPath;
	static bool init{ true }; //THIS IS TEMPORARY PLEASE FIND ANOTHER WAY
	if (init) {
		s_AssetPath = assetmanager.GetDefaultPath();
		currentDirectory = s_AssetPath;
		init = false;
	}

	ImGui::Begin("Content Browser");
	
	if (currentDirectory != std::filesystem::path(s_AssetPath)) {
		if (ImGui::ImageButton(loadedIcons["backIcon"],ImVec2{32,32})) {
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
			else {
				// Handle file click
				std::cout << "File selected: " << currentDirectory / path.filename() << std::endl;
			}
		}
		ImGui::TextWrapped(filenameString.c_str());
		ImGui::NextColumn();
	}
		ImGui::Columns(1);

	
	ImGui::End();
}
