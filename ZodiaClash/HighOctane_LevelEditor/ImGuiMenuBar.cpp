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
*	@file		ImGuiMenuBar.h
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file is the definition for Menu Bar of the editor
*
******************************************************************************/
#include "ImGuiMenuBar.h"
#include "Global.h"
#include "AssetManager.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Serialization.h"
#include "WindowsInterlink.h"
#include "File.h"
#include "GUIManager.h"
#include "graphics.h"
#include "Layering.h"

constexpr float fontSizeS = 10.f;
constexpr float fontSizeM = 20.f;
constexpr float fontSizeL = 30.f;

#include "Events.h"
void UpdateMenuBar() {
    
    // Create a menu bar for the window
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load Scene")) {
                std::string path{ OpenSingleFileDialog("*.scn","Scene File") };
                if (path != "") {
                    if (FilePath::GetFileExtension(path) == ".scn") {
                        size_t pos = path.find_last_of("\\");
                        path = path.substr(pos + 1);
                        events.Call("Change Scene",path);
                    }
                    else {
                        ASSERT(true, "Please open a .scn file!");
                    }
                }
            }
            if (ImGui::MenuItem("Save Scene")) {
                //PrepareLayeringForSerialization();
                //EmbedSkipLockForSerialization();
                button_clicked = true;
                saveFile = true;
            }
            if (ImGui::MenuItem("Close Scene")) {
                events.Call("Change Scene", "");
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit Program")) {
                EngineCore::engineCore().setGameActive(false);
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {

            if (ImGui::MenuItem(snappingOn ? "Turn Off Snapping Guides" : "Turn On Snapping Guides", NULL, snappingOn)) {
                if (snappingOn) {
                    snappingOn = false;
                }
                else {
                    snappingOn = true;
                    tilemapOn = false;
                }
            }

            if (ImGui::MenuItem(tilemapOn ? "Turn Off Tile Map Grid" : "Turn On Tile Map Grid", NULL, tilemapOn)) {
                if (tilemapOn) {
                    tilemapOn = false;
                }
                else {
                    tilemapOn = true;
                    snappingOn = false;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem(fullscreen ? "Exit Fullscreen" : "Enter Fullscreen", "F11", &fullscreen)) {
                graphics.Fullscreen(fullscreen);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();



    }
    


}
