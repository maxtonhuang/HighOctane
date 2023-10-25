#include "ImGuiMenuBar.h"
#include "Global.h"
#include "AssetManager.h"

void UpdateMenuBar() {

    // Create a menu bar for the window
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Files")) {
            if (ImGui::MenuItem("Load Scene")) {

            }
            if (ImGui::MenuItem("Save Scene")) {
                button_clicked = true;
            }
            if (ImGui::MenuItem("Close Scene")) {
                //if ()
                    assetmanager.UnloadAll();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();

}
