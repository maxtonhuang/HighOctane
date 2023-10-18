

#include "ImGuiPlayStop.h"
#include "Global.h"


void UpdatePlayStop() {

	ImGui::Begin("Controls");

	if (ImGui::Button("Play", {100,50})) {
		edit_mode = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", { 100,50 })) {
		edit_mode = true;
		//reset scene
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause", { 100,50 })) {
		edit_mode = true;
	}

	ImGui::End();

}