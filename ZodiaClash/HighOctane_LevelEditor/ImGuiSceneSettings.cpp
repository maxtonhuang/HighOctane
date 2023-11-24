#include "ImGuiSceneSettings.h"
#include "Global.h"
#include "AssetManager.h"

void UpdateSceneSettingsWindow() {
	static float saveTimer{};

	if (saveTimer > 0.f) {
		saveTimer -= g_dt;
	}

	ImGui::Begin("Scene Settings");

	std::string currentSceneLabel{ "Current Scene: " };
	currentSceneLabel += sceneName;

	ImGui::Text(currentSceneLabel.c_str());
	if (ImGui::Button("Save Scene")) {
		std::string scenePath{ assetmanager.GetDefaultPath() + "Scenes\\" + sceneName };
		assetmanager.SaveScene(scenePath);
		saveTimer = 1.f;
	}
	if (saveTimer > 0.f) {
		ImGui::SameLine();
		ImGui::Text("Scene Saved!");
	}

	ImGui::Separator();
	std::vector<std::string> bgmList{ assetmanager.audio.GetMusicPaths() };
	std::string currentBGM{ assetmanager.audio.GetCurrentBGM() };
	if (currentBGM == "") {
		currentBGM = "None";
	}
	if (ImGui::BeginCombo("Scene BGM", currentBGM.c_str())) {
		bool is_selected = (currentBGM == "None");
		if (ImGui::Selectable("None", is_selected)) {
			assetmanager.audio.SetBGM("");
		}
		if (is_selected) {
			ImGui::SetItemDefaultFocus();
		}
		for (int n = 0; n < bgmList.size(); n++) {
			is_selected = (currentBGM == bgmList[n]);
			if (ImGui::Selectable(bgmList[n].c_str(), is_selected)) {
				assetmanager.audio.SetBGM(bgmList[n].c_str());
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Text("Loaded Assets:");
	ImGui::Text("Textures:");
	std::vector<std::string> textureList{ assetmanager.texture.GetTextureNames() };
	for (std::string t : textureList) {
		ImGui::Text(t.c_str());
	}
	
	ImGui::Text(" ");
	ImGui::Text("Sounds:");
	std::vector<std::string> soundList{ assetmanager.audio.GetSoundNames() };
	for (std::string s : soundList) {
		ImGui::Text(s.c_str());
	}

	ImGui::End();
}
