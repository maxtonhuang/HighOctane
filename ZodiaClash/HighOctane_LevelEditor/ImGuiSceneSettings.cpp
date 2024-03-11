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
*	@file		ImGuiSceneSettings.cpp
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		11 November 2023
*
* *****************************************************************************
*
*	@brief		Scene settings window
*
*   Contains function for the gscene settings window which allows
*	for showing the scene name and saving the current scene as well as
*	changing the BGM and showing loaded assets
*
******************************************************************************/

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
	if (ImGui::Button("Save Scene Assets")) {
		std::string scenePath{ assetmanager.GetDefaultPath() + "Scenes\\" + sceneName };
		assetmanager.SaveSceneAssets(scenePath);
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

	std::vector<std::string> ambienceList{ assetmanager.audio.GetAmbiencePaths() };
	std::string currentAmbience{ assetmanager.audio.GetCurrentAmbience() };
	if (currentAmbience == "") {
		currentAmbience = "None";
	}
	if (ImGui::BeginCombo("Scene Ambience", currentAmbience.c_str())) {
		bool is_selected = (currentAmbience == "None");
		if (ImGui::Selectable("None", is_selected)) {
			assetmanager.audio.SetAmbience("");
		}
		if (is_selected) {
			ImGui::SetItemDefaultFocus();
		}
		for (int n = 0; n < ambienceList.size(); n++) {
			is_selected = (currentAmbience == ambienceList[n]);
			if (ImGui::Selectable(ambienceList[n].c_str(), is_selected)) {
				assetmanager.audio.SetAmbience(ambienceList[n].c_str());
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	float masterVolume{ assetmanager.audio.GetGroupVolume("Master") };
	float sfxVolume{ assetmanager.audio.GetGroupVolume("SFX") };
	float bgmVolume{ assetmanager.audio.GetGroupVolume("BGM") };
	float envVolume{ assetmanager.audio.GetGroupVolume("ENV") };
	float vocVolume{ assetmanager.audio.GetGroupVolume("VOC") };

	ImGui::Text("Volume Settings");
	ImGui::DragFloat("Master", &masterVolume, 0.1f, 0.f, 1.f);
	ImGui::DragFloat("Game Sounds", &sfxVolume, 0.1f, 0.f, 1.f);
	ImGui::DragFloat("Music", &bgmVolume, 0.1f, 0.f, 1.f);
	ImGui::DragFloat("Environmental", &envVolume, 0.1f, 0.f, 1.f);
	ImGui::DragFloat("Voice", &vocVolume, 0.1f, 0.f, 1.f);

	assetmanager.audio.SetGroupVolume("Master", masterVolume);
	assetmanager.audio.SetGroupVolume("SFX", sfxVolume);
	assetmanager.audio.SetGroupVolume("BGM", bgmVolume);
	assetmanager.audio.SetGroupVolume("ENV", envVolume);
	assetmanager.audio.SetGroupVolume("VOC", vocVolume);

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
