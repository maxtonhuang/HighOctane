#include "ImGuiAssetLibrary.h"
#include "Serialization.h"
#include <Windows.h>
#include <shobjidl.h>

void UpdateAssetLibrary() {

	ImGui::Begin("Asset Library");

	if (ImGui::Button("Import Asset", { ImGui::GetContentRegionAvail().x, 50})) {

		std::vector<std::string> filesToOpen = OpenFileDialog();
		
		if (!filesToOpen.empty()) { // user did not press Cancel button

			for (std::string val : filesToOpen) {
				std::cout << val << std::endl;
			}

		}
	}
	
	ImGui::End();

}