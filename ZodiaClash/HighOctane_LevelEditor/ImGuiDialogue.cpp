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
*	@file		ImGuiDialogue.cpp
*
*	@co-author	Chua Zhen Rong
*
*	@email		c.zhenrong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		[M4] 08 February 2024
*               [M5] 15 March 2024
*
* *****************************************************************************
*
*	@brief		Editor Interface for dialogue system
*
*	Interface for easier modifications to scene dialogues
*
*   M4 -    base dialogue system
*   M5 -	enhancements, allow more than 1 dialogue/scene + identifier enums
*
******************************************************************************/

#include "ImGuiDialogue.h"
#include "ImGuiSceneHierarchy.h"
#include "UIComponents.h"
#include "AssetManager.h"

static bool flag{};

/*!
* \brief DialogueWindow
*
* The imgui window for dialogue editor (docked)
*
*/
void DialogueWindow(Entity entity) {
    if (ECS::ecs().HasComponent<DialogueHUD>(entity)) {
        DialogueHUD& dialogueData = ECS::ecs().GetComponent<DialogueHUD>(entity);
        Transform& transformData = ECS::ecs().GetComponent<Transform>(entity);
        DialogueHUD::Dialogue newDialogue{};

        // Button to add a new dialogue line
        if (ImGui::Button("Add New Dialogue##1")) {
            bool result = false;
            dialogueData.AddDialogue(newDialogue, result);
        }
        if (dialogueData.dialogues.size() == dialogueData.dialogues.capacity()) {
            ImGui::SameLine();
            ImGui::Text("Max dialogues reached!");
		}

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adjust the height as needed

        // Iterate through all dialogues
        for (size_t i = 0; i < dialogueData.dialogues.size(); ++i)
        {
            DialogueHUD::Dialogue& dialogue = dialogueData.dialogues[i];

            // replicate single dialogue imgui from m4 for every dialogue in the vector
            // collapsing header for each dialogue
            std::string headerTitle = "Dialogue " + std::to_string(i + 1);
            if (ImGui::CollapsingHeader(headerTitle.c_str())) {
                if (ImGui::BeginTable("DialogueSettingsTable", 3, ImGuiTableFlags_Borders)) {
                    ImGui::TableSetupColumn("Toggles", ImGuiTableColumnFlags_WidthFixed, 400.0f);
                    ImGui::TableSetupColumn("Trigger Condition", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                    ImGui::TableSetupColumn("Has Speaker?", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    

                    // Display headers row
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();

                    // Column 1: Save Settings
                    ImGui::TableSetColumnIndex(0);
                    // Enforce index
                    if (dialogueData.currentDialogue == &dialogue) {
                        int& selectedIndex = dialogueData.currentDialogue->viewingIndex;
                        int maxViewingIndex = static_cast<int>(dialogue.dialogueLines.size()) - 1;
                        ImGui::InputInt("Viewing Index", &selectedIndex);
                        selectedIndex = (maxViewingIndex > 0) ? std::clamp(selectedIndex, 0, maxViewingIndex) : 0;
                    }
                    else {
                        ImGui::Text("Viewing Index: Edit this Dialogue to toggle!");
                    }

                    ImGui::Text("Is Active: %s", (dialogue.isActive) ? "true" : "false");
                    ImGui::SameLine();
                    ImGui::Text("\t Is Triggered: %s", (dialogue.isTriggered) ? "true" : "false");

                    if (ImGui::Button("Edit this Dialogue")) {
                        // reset prev editing dialogue to defaults
                        if (dialogueData.currentDialogue) {
                            dialogueData.currentDialogue->isActive = false;
                            dialogueData.currentDialogue->isTriggered = false;
							dialogueData.currentDialogue->viewingIndex = 0;
                        }
                        dialogueData.currentDialogue = &dialogue;
                        dialogueData.isEditing = true;
                        transformData.position.x = 0.f;
                        transformData.position.y = -350.0f;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Prepare for scene saving")) {
                        dialogue.isActive = false;
                        dialogue.isTriggered = false;
                        dialogue.viewingIndex = 0;
                        dialogueData.isEditing = false;
                        transformData.position.x = 0.f;
                        transformData.position.y = -800.0f;
                    }                 


                    // Column 2: Trigger Condition
                    ImGui::TableSetColumnIndex(1);
                    DIALOGUE_TRIGGER& triggerSetting = dialogue.triggerType;

                    const char* triggerOptions[] = { "Auto", "Event", "Pre-Battle", "Turn", "Health", "Win", "Lose", "Default" };

                    if (ImGui::BeginCombo("##Trigger Condition", triggerOptions[static_cast<int>(triggerSetting)])) {
                        for (int j = 0; j < static_cast<int>(DIALOGUE_TRIGGER::DEFAULT) + 1; ++j) {
                            bool isSelected = (triggerSetting == static_cast<DIALOGUE_TRIGGER>(j));
                            if (ImGui::Selectable(triggerOptions[j], isSelected)) {
                                triggerSetting = static_cast<DIALOGUE_TRIGGER>(j);
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    int& roundTrigger_ = dialogue.roundTrigger;
                    int& healthTrigger_ = dialogue.healthTrigger;
                    BattleSystem* battleSys = events.GetBattleSystem();
                    switch (triggerSetting) {
                    case DIALOGUE_TRIGGER::TURN_BASED:
                        ImGui::InputInt("##Turn Number", &roundTrigger_);
                        roundTrigger_ = std::max(0, roundTrigger_);
                        healthTrigger_ = 0;
                        
                        if (battleSys) {
							ImGui::Text("Current Turn: %d", battleSys->roundManage.roundCounter);
						}
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


                    // Column 3: Has Speaker?
                    ImGui::TableSetColumnIndex(2);
                    bool& toggleSpeaker = dialogue.speakerRequired;
                    ImGui::Checkbox("##Dialogue has speaker?", &toggleSpeaker);

                    

                    ImGui::EndTable();
                    ImGui::Text("NOTE: Ensure \"Prepare for scene saving\" button is clicked when saving scene!");
                }
                if (ImGui::BeginTable("DialogueSettingsTable2", 3, ImGuiTableFlags_Borders)) {
                    ImGui::TableSetupColumn("Set post-dialogue scene?", ImGuiTableColumnFlags_WidthFixed, 400.0f);
                    ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    
                    // Display headers row
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();

                    // Column 4: Set post-dialoogue scene?
                    ImGui::TableSetColumnIndex(0);
                    bool& toggleEvent = dialogue.postDialogueScene;
                    ImGui::Checkbox("##Set post-dialogue scene?", &toggleEvent);
                    if (toggleEvent) {
                        ImGui::SameLine();
                        ImGui::InputText("Target Scene", &dialogue.targetScene);
                    }

                    // Column 5: Delete
                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::Button("Remove")) {
                        dialogueData.RemoveDialogue(static_cast<int>(i));
                    }
                    ImGui::EndTable();
                }
                ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adjust the height as needed


                // Button to add a new dialogue line
                if (ImGui::Button("Add New Line##1")) {
                    dialogue.dialogueLines.emplace_back(DialogueHUD::DialogueLine{}); // Add an empty line
                }

                int tableCols = (dialogue.speakerRequired) ? 4 : 3;

                if (ImGui::BeginTable("DialogueTable", tableCols, ImGuiTableFlags_Borders)) {
                    if (dialogue.speakerRequired) {
                        ImGui::TableSetupColumn("Speaker", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    }
                    ImGui::TableSetupColumn("Line", ImGuiTableColumnFlags_WidthStretch, 0.0f);
                    ImGui::TableSetupColumn("Voiceline", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                    ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthFixed, 75.0f);

                    // Display headers row
                    ImGui::TableHeadersRow();

                    // Render dialogue lines
                    for (size_t k = 0; k < dialogue.dialogueLines.size(); ++k) {
                        ImGui::TableNextRow();

                        if (dialogue.speakerRequired) {
                            // Column 1: InputText for Speaker's name
                            ImGui::TableSetColumnIndex(0);
                            ImGui::InputText(("##Speaker" + std::to_string(k)).c_str(), &dialogue.dialogueLines[k].speaker);
                        }

                        // Column 2: InputTextMultiline for Speaker's line
                        ImGui::TableSetColumnIndex(dialogue.speakerRequired ? 1 : 0);
                        ImGui::InputTextMultiline(("##Line" + std::to_string(k)).c_str(), &dialogue.dialogueLines[k].line);

                        // Column 3: Combo box for voice sound to play
                        ImGui::TableSetColumnIndex(dialogue.speakerRequired ? 2 : 1);
                        //ImGui::InputTextMultiline(("##Voice" + std::to_string(k)).c_str(), &dialogue.dialogueLines[k].voice);
                        ImGui::PushID(static_cast<int>(k));
                        if (ImGui::BeginCombo("##Voice", dialogue.dialogueLines[k].voice.c_str())) {
                            std::vector<std::string> soundPaths{ assetmanager.audio.GetSoundPaths() };
                            for (int n = 0; n < soundPaths.size(); n++) {
                                bool is_selected = (dialogue.dialogueLines[k].voice == soundPaths[n]);
                                if (ImGui::Selectable(soundPaths[n].c_str(), is_selected)) {
                                    dialogue.dialogueLines[k].voice = soundPaths[n];
                                }
                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::PopID();

                        // Column 4: Delete button
                        ImGui::TableSetColumnIndex(dialogue.speakerRequired ? 3 : 2);
                        if (ImGui::Button(("Delete##" + std::to_string(k)).c_str())) {
                            // Delete the line
                            dialogue.dialogueLines.erase(dialogue.dialogueLines.begin() + k);
                            --i; // Adjust the index after removal
                        }
                    }

                    ImGui::EndTable();
                }

                // Button to add a new dialogue line
                if (ImGui::Button("Add New Line##2")) {
                    dialogue.dialogueLines.emplace_back(DialogueHUD::DialogueLine{}); // Add an empty line
                }
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adjust the height as needed

        // Button to add a new dialogue line
        if (ImGui::Button("Add New Dialogue##2")) {
            bool result = false;
            dialogueData.AddDialogue(newDialogue, result);
        }
        if (dialogueData.dialogues.size() == dialogueData.dialogues.capacity()) {
            ImGui::SameLine();
            ImGui::Text("Max dialogues reached!");
        }
    }
}


/*!
* \brief UpdateDialogEditor
*
* Component checker for dialogue editor tab
*
*/
void UpdateDialogEditor() {
	Entity chosenEntity{ currentSelectedEntity };
	if (ECS::ecs().HasComponent<DialogueHUD>(chosenEntity)) {
		ImGui::Begin("Dialogue Editor", nullptr, flag);

		DialogueWindow(chosenEntity);

		ImGui::End();
	}
}
