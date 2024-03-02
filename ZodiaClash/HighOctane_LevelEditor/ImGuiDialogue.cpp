#include "ImGuiDialogue.h"
#include "ImGuiSceneHierarchy.h"
#include "UIComponents.h"

static bool flag{};

void DialogueWindow(Entity entity) {
    if (ECS::ecs().HasComponent<DialogueHUD>(entity)) {
        DialogueHUD& dialogueData = ECS::ecs().GetComponent<DialogueHUD>(entity);
        DialogueHUD::Dialogue newDialogue{};

        // Button to add a new dialogue line
        if (ImGui::Button("Add New Dialogue##1")) {
            //dialogueData.dialogues.push_back(newDialogue);
            dialogueData.AddDialogue(newDialogue);
        }

        for (size_t i = 0; i < dialogueData.dialogues.size(); ++i)
        {
            DialogueHUD::Dialogue& dialogue = dialogueData.dialogues[i];

            // replicate single dialogue imgui from m4 for every dialogue in the vector
            // collapsing header for each dialogue
            std::string headerTitle = "Dialogue " + std::to_string(i + 1);
            if (ImGui::CollapsingHeader(headerTitle.c_str())) {
                // Enforce index
                int& selectedIndex = dialogue.viewingIndex;
                int maxViewingIndex = static_cast<int>(dialogue.dialogueLines.size()) - 1;
                ImGui::InputInt("Viewing Index", &selectedIndex);
                selectedIndex = (maxViewingIndex > 0) ? std::clamp(selectedIndex, 0, maxViewingIndex) : 0;

                // IsActive (ensure this is unchecked when saving scene!)
                ImGui::SameLine();
                bool& toggleActive = dialogue.isActive;
                ImGui::Checkbox("Is Active", &toggleActive);

                // IsTriggered (ensure this is unchecked when saving scene!)
                ImGui::SameLine();
                bool& toggleTriggered = dialogue.isTriggered;
                ImGui::Checkbox("Is Triggered", &toggleTriggered);

                ImGui::Text("NOTE: Ensure Viewing Index is set to 0, Is Active and Is Triggered is UNCHECKED when saving scene!");

                // trigger type
                DIALOGUE_TRIGGER& triggerSetting = dialogue.triggerType;

                const char* triggerOptions[] = { "Auto", "Event", "Pre-Battle", "Turn", "Health", "Post-Battle", "Default" };

                if (ImGui::BeginCombo("Trigger Condition", triggerOptions[static_cast<int>(triggerSetting)])) {
                    for (int i = 0; i < static_cast<int>(DIALOGUE_TRIGGER::DEFAULT) + 1; ++i) {
                        bool isSelected = (triggerSetting == static_cast<DIALOGUE_TRIGGER>(i));
                        if (ImGui::Selectable(triggerOptions[i], isSelected)) {
                            triggerSetting = static_cast<DIALOGUE_TRIGGER>(i);
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                int& roundTrigger_ = dialogue.roundTrigger;
                int& healthTrigger_ = dialogue.healthTrigger;
                switch (triggerSetting) {
                case DIALOGUE_TRIGGER::TURN_BASED:
                    ImGui::InputInt("##Turn Number", &roundTrigger_);
                    roundTrigger_ = std::max(0, roundTrigger_);
                    healthTrigger_ = 0;
                    break;
                case DIALOGUE_TRIGGER::HEALTH_BASED:
                    ImGui::InputInt("##Health Breakpoint", &healthTrigger_);
                    ImGui::SameLine();
                    ImGui::Text("%%");
                    healthTrigger_ = std::clamp(healthTrigger_, 0, 100);
                    roundTrigger_ = 0;
                    break;
                default:
                    // zero out roundTrigger_ and healthTrigger_ if not turn or health based?
                    roundTrigger_ = 0;
                    healthTrigger_ = 0;
                    break;
                }

                // Speaker toggle
                bool& toggleSpeaker = dialogue.speakerRequired;
                ImGui::Checkbox("Dialogue has speaker?", &toggleSpeaker);

                ImGui::SameLine();
                // Post-dialogue event
                bool& toggleEvent = dialogue.postDialogueScene;
                ImGui::Checkbox("Set post-dialogue scene?", &toggleEvent);
                if (toggleEvent) {
                    ImGui::InputText("Post-dialogue target scene", &dialogue.targetScene);
                }

                // Button to add a new dialogue line
                if (ImGui::Button("Add New Line##1")) {
                    dialogue.dialogueLines.emplace_back("", ""); // Add an empty line
                }

                int tableCols = (dialogue.speakerRequired) ? 3 : 2;

                if (ImGui::BeginTable("DialogueTable", tableCols, ImGuiTableFlags_Borders)) {
                    //ImGui::TableSetupColumn("Trigger Condition", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    if (dialogue.speakerRequired) {
                        ImGui::TableSetupColumn("Speaker", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    }
                    ImGui::TableSetupColumn("Line", ImGuiTableColumnFlags_WidthStretch, 0.0f);
                    ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthFixed, 100.0f);

                    // Display headers row
                    ImGui::TableHeadersRow();

                    // Render dialogue lines
                    for (size_t i = 0; i < dialogue.dialogueLines.size(); ++i) {
                        ImGui::TableNextRow();

                        if (dialogue.speakerRequired) {
                            // Column 1: InputText for Speaker's name
                            ImGui::TableSetColumnIndex(0);
                            ImGui::InputText(("##Speaker" + std::to_string(i)).c_str(), &dialogue.dialogueLines[i].first);
                        }

                        // Column 2: InputTextMultiline for Speaker's line
                        ImGui::TableSetColumnIndex(dialogue.speakerRequired ? 1 : 0);
                        ImGui::InputTextMultiline(("##Line" + std::to_string(i)).c_str(), &dialogue.dialogueLines[i].second);

                        // Column 3: Delete button
                        ImGui::TableSetColumnIndex(dialogue.speakerRequired ? 2 : 1);
                        if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                            // Delete the line
                            dialogue.dialogueLines.erase(dialogue.dialogueLines.begin() + i);
                            --i; // Adjust the index after removal
                        }
                    }

                    ImGui::EndTable();
                }

                // Button to add a new dialogue line
                if (ImGui::Button("Add New Line##2")) {
                    dialogue.dialogueLines.emplace_back("", ""); // Add an empty line
                }
            }
        }

        // Button to add a new dialogue line
        if (ImGui::Button("Add New Dialogue##2")) {
            //dialogueData.dialogues.push_back(newDialogue);
            dialogueData.AddDialogue(newDialogue);
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
