#include <filesystem>
#include "ImGuiContentBrowser.h"
static const std::filesystem::path s_AssetPath = "../Assets/";

void UpdateContentBrowser(){
	static std::filesystem::path currentDirectory = s_AssetPath;
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