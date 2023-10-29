#include "ImGuiMenuBar.h"
#include "Global.h"
#include "AssetManager.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Serialization.h"
#include "WindowsInterlink.h"
#include "File.h"
#include "GUIManager.h"

constexpr float fontSizeS = 10.f;
constexpr float fontSizeM = 20.f;
constexpr float fontSizeL = 30.f;

void UpdateMenuBar() {
    
    // Create a menu bar for the window
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Files")) {
            if (ImGui::MenuItem("Load Scene")) {
                std::string path{ OpenSingleFileDialog() };
                if (path != "") {
                    if (FilePath::GetFileExtension(path) == ".scn") {
                        size_t pos = path.find_last_of("\\");
                        //ASSERT(pos == std::string::npos, "File path error!");
                        path = path.substr(pos + 1);
                        assetmanager.LoadAssets(path);
                        //Serializer::LoadEntityFromJson(OpenSingleFileDialog());
                    }
                    else {
                        ASSERT(true, "Please open a .scn file!");
                    }
                }
            }
            if (ImGui::MenuItem("Save Scene")) {
                button_clicked = true;
                saveFile = true;
            }
            if (ImGui::MenuItem("Close Scene")) {

                //EntityFactory::entityFactory().masterEntitiesList.clear();
                //for (const Entity& entity : s_ptr->m_Entities) {
                //    ECS::ecs().DestroyEntity(entity);
                //}
                ////ECS::ecs().DestroyEntity(entity);
                destroyAll = true;
                button_clicked = true;
                assetmanager.UnloadAll();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {

            ImGui::EndMenu();
        }
        //if (ImGui::BeginMenu("Settings")) {
        //    if (ImGui::MenuItem("Increase Font Size")) {

        //        if (fontSizeS == currentFontSize) {
        //            ImGui::PopFont();
        //            ImGui::PushFont(latoMedium);
        //            currentFontSize = fontSizeM;
        //        }
        //        else if (fontSizeM == currentFontSize) {
        //            ImGui::PopFont();
        //            ImGui::PushFont(latoLarge);
        //            currentFontSize = fontSizeL;
        //        }
        //        
        //    }
        //    if (ImGui::MenuItem("Decrease Font Size")) {
        //        if (fontSizeL == currentFontSize) {
        //            ImGui::PopFont();
        //            ImGui::PushFont(latoMedium);
        //            currentFontSize = fontSizeM;
        //        }
        //        else if (fontSizeM == currentFontSize) {
        //            ImGui::PopFont();
        //            ImGui::PushFont(latoSmall);
        //            currentFontSize = fontSizeS;
        //        }
        //    }
        //    ImGui::EndMenu();
        //}
        ImGui::EndMenuBar();
    }
    


}
