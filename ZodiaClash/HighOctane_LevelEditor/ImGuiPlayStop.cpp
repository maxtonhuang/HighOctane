

#include "ImGuiPlayStop.h"
#include "Global.h"
#include "Serialization.h"

bool buffer = false;

void UpdatePlayStop() {

	ImGui::Begin("Controls");


	if (ImGui::Button("Play", {100,50})) {
		playButton = true;
		edit_mode = false;
		button_clicked = true;
	}
	ImGui::SameLine();
	if (buffer == true) {
		Serializer::LoadEntityFromJson("../Assets/Scenes/tmp.json");
		buffer = false;
	}
	if (ImGui::Button("Stop", { 100,50 })) {
		destroyAll = true;
		button_clicked = true;
		edit_mode = true;
		buffer = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause", { 100,50 })) {
		edit_mode = true;
	}

	ImGui::End();

}