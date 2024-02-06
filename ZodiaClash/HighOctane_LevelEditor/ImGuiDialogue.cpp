#include "ImGuiDialogue.h"
#include "ImGuiSceneHierarchy.h"
#include "UIComponents.h"

static bool flag{};

void DialogueWindow(Entity entity) {
	if (ECS::ecs().HasComponent<DialogueHUD>(entity)) {
		DialogueHUD& dialogueData = ECS::ecs().GetComponent<DialogueHUD>(entity);

        // Button to add a new dialogue line
        if (ImGui::Button("Add New Line##1")) {
            dialogueData.dialogueLines.emplace_back("", ""); // Add an empty line
        }

        // Enforce index
        int& selectedIndex = dialogueData.viewingIndex;
        int maxViewingIndex = static_cast<int>(dialogueData.dialogueLines.size()) - 1;
        ImGui::InputInt("Viewing Index", &selectedIndex);
        selectedIndex = std::clamp(selectedIndex, 0, maxViewingIndex);

        // IsActive (ensure this is unchecked when saving scene!)
        bool& toggleActive = dialogueData.isActive;
        ImGui::Checkbox("Is Active", &toggleActive);

        ImGui::Text("Ensure Viewing Index is set to 0, and Is Active is UNCHECKED when saving scene!");

        // Launch dialogue setting
        bool& toggleLaunch = dialogueData.autoLaunch;
        ImGui::Checkbox("Launch dialogue automatically?", &toggleLaunch);

        ImGui::SameLine();
        // Speaker toggle
        bool& toggleSpeaker = dialogueData.speakerRequired;
        ImGui::Checkbox("Dialogue has speaker?", &toggleSpeaker);

        // Post-dialogue event
        bool& toggleEvent = dialogueData.postDialogueScene;
        ImGui::Checkbox("Set post-dialogue scene?", &toggleEvent);
        if (toggleEvent) {
            ImGui::InputText("Post-dialogue target scene", &dialogueData.targetScene);
        }

        int tableCols = (dialogueData.speakerRequired) ? 3 : 2;

        if (ImGui::BeginTable("DialogueTable", tableCols, ImGuiTableFlags_Borders)) {
            if (dialogueData.speakerRequired) {
                ImGui::TableSetupColumn("Speaker", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            }            
            ImGui::TableSetupColumn("Line", ImGuiTableColumnFlags_WidthStretch, 0.0f);
            ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            // Display headers row
            ImGui::TableHeadersRow();

            // Render dialogue lines
            for (size_t i = 0; i < dialogueData.dialogueLines.size(); ++i) {
                ImGui::TableNextRow();

                if (dialogueData.speakerRequired) {
                    // Column 1: InputText for Speaker's name
                    ImGui::TableSetColumnIndex(0);
                    ImGui::InputText(("##Speaker" + std::to_string(i)).c_str(), &dialogueData.dialogueLines[i].first);
                }                

                // Column 2: InputTextMultiline for Speaker's line
                ImGui::TableSetColumnIndex(dialogueData.speakerRequired ? 1 : 0);
                ImGui::InputText(("##Line" + std::to_string(i)).c_str(), &dialogueData.dialogueLines[i].second);

                // Column 3: Delete button
                ImGui::TableSetColumnIndex(dialogueData.speakerRequired ? 2 : 1);
                if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                    // Delete the line
                    dialogueData.dialogueLines.erase(dialogueData.dialogueLines.begin() + i);
                    --i; // Adjust the index after removal
                }
            }

            ImGui::EndTable();
        }

        // Button to add a new dialogue line
        if (ImGui::Button("Add New Line##2")) {
            dialogueData.dialogueLines.emplace_back("", ""); // Add an empty line
        }
	}
}

void UpdateDialogEditor() {
	Entity chosenEntity{ currentSelectedEntity };
	/*static bool dialogPrefab{ false };
	if (dialogPrefab) {
		chosenEntity = currentSelectedPrefab;
	}*/
	if (ECS::ecs().HasComponent<DialogueHUD>(chosenEntity)) {
		ImGui::Begin("Dialogue Editor", nullptr, flag);
		//ImGui::Checkbox("Edit prefab?##dialog", &dialogPrefab);

		DialogueWindow(chosenEntity);

		ImGui::End();
	}
}
