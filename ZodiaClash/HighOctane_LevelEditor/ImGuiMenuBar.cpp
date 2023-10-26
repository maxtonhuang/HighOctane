#include "ImGuiMenuBar.h"
#include "Global.h"
#include "AssetManager.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Serialization.h"
#include "WindowsInterlink.h"
#include "File.h"
void UpdateMenuBar() {
    
    // Create a menu bar for the window
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Files")) {
            if (ImGui::MenuItem("Load Scene")) {
                std::string path{ OpenSingleFileDialog() };
                ASSERT(FilePath::GetFileExtension(path) != ".scn", "Please open a .scn file!");
                size_t pos = path.find_last_of("\\");
                ASSERT(pos == std::string::npos, "File path error!");
                path = path.substr(pos + 1);
                assetmanager.LoadAssets(path);
                //Serializer::LoadEntityFromJson(OpenSingleFileDialog());
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
    }
    ImGui::EndMenuBar();


}
