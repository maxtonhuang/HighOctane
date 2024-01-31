#include "ImGuiDialogue.h"
#include "ImGuiSceneHierarchy.h"
#include "UIComponents.h"

static bool flag{};

void DialogueWindow(Entity entity) {
	if (ECS::ecs().HasComponent<DialogueHUD>(entity)) {
		DialogueHUD& dialogueData = ECS::ecs().GetComponent<DialogueHUD>(entity);

        if (ImGui::BeginTable("DialogueTable", 3, ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("Speaker", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Line", ImGuiTableColumnFlags_WidthStretch, 0.0f);
            ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            // Display headers row
            ImGui::TableHeadersRow();

            // Render dialogue lines
            for (size_t i = 0; i < dialogueData.dialogueLines.size(); ++i) {
                ImGui::TableNextRow();

                // Column 1: InputText for Speaker's name
                ImGui::TableSetColumnIndex(0);
                ImGui::InputText(("##Speaker" + std::to_string(i)).c_str(), &dialogueData.dialogueLines[i].first);

                // Column 2: InputTextMultiline for Speaker's line
                ImGui::TableSetColumnIndex(1);
                ImGui::InputTextMultiline(("##Line" + std::to_string(i)).c_str(), &dialogueData.dialogueLines[i].second);

                // Column 3: Delete button
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                    // Delete the line
                    dialogueData.dialogueLines.erase(dialogueData.dialogueLines.begin() + i);
                    --i; // Adjust the index after removal
                }
            }

            ImGui::EndTable();
        }

        // Button to add a new dialogue line
        if (ImGui::Button("Add New Line")) {
            dialogueData.dialogueLines.emplace_back("", ""); // Add an empty line
        }
	}
}

void UpdateDialogEditor() {
	Entity chosenEntity{ currentSelectedEntity };
	static bool prefab{ false };
	if (prefab) {
		chosenEntity = currentSelectedPrefab;
	}
	if (ECS::ecs().HasComponent<DialogueHUD>(chosenEntity)) {
		ImGui::Begin("Dialogue Editor", nullptr, flag);
		ImGui::Checkbox("Edit prefab?", &prefab);

		DialogueWindow(chosenEntity);

		ImGui::End();
	}
}
