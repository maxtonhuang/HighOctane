#include "ImGuiMenuBar.h"
#include "Global.h"
#include "AssetManager.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Serialization.h"
#include "WindowsInterlink.h"
void UpdateMenuBar() {
    
    // Create a menu bar for the window
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Files")) {
            if (ImGui::MenuItem("Load Scene")) {
                Serializer::LoadEntityFromJson(OpenSingleFileDialog());
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
                //assetmanager.UnloadAll();
                    
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMenuBar();


}
