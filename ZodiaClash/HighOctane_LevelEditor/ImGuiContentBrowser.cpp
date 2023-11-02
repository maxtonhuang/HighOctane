#include <filesystem>
#include "ImGuiContentBrowser.h"
#include "AssetManager.h"
#include <map>

static std::filesystem::path s_AssetPath = "../Assets/";
std::string folderIconPath = "../Assets/Textures/Icons/foldericon.png";
std::string fileIconPath = "../Assets/Textures/Icons/fileicon.png";
static std::map<std::string, ImTextureID> loadedIcons;

void LoadIcons() {
	//loadedIcons["folderIcon"] = assetmanager.LoadTexture(folderIconPath);
	//loadedIcons["fileIcon"] = assetmanager.LoadTexture(fileIconPath);

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
		if (ImGui::Button("<-")) {
			currentDirectory = currentDirectory.parent_path();
			if (currentDirectory < s_AssetPath) {
				currentDirectory = s_AssetPath;
			}
		}
	}

	for (auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
		const auto& path = entry.path();
		auto relativePath = std::filesystem::relative(path, s_AssetPath);
		std::string filenameString = relativePath.filename().string();
		if (entry.is_directory()) {
			ImGui::SameLine();
			if (ImGui::Button(filenameString.c_str())) {
				std::cout << "Before navigation: " << currentDirectory << std::endl;
				currentDirectory /= path.filename();
				std::cout << "After navigation: " << currentDirectory << std::endl;
			}
		}
		else {
			if(ImGui::Button(filenameString.c_str())){
				std::cout << "File selected: " << currentDirectory / path.filename() << std::endl;
			}
		}
	}

	
	ImGui::End();
}
