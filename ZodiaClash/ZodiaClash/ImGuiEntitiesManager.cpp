

#include "ImGuiEntitiesManager.h"
//#include "debugdiagnostic.h"
//#include "DebugProfile.h"
//#include "enginecore.h"
//#include "debuglog.h"
//#include "ECS.h"
//#include "Components.h"
#include "EntityFactory.h"
#include "physics.h"
#include "Serialization.h"
//#include "EntityFactory.h"

void InitEntitiesManager() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

void UpdateEntitiesManager() {
    {
        static int counter = 0;
        static bool entityAddedFlag = false;
        ImGui::Begin("Creating Entity");

        //Entity testEntity;
        Entity testEntity;
        if (ImGui::Button("Create Entity")) {
            testEntity = ECS::ecs().CreateEntity();
            masterEntitiesList["CatTest"] = testEntity;
            ECS::ecs().AddComponent(testEntity, Color{ glm::vec4{ 1,1,1,1 } });
            ECS::ecs().AddComponent(testEntity, Transform{ Vec2{ 0.f,0.f }, 0.f, Vec2{ 1.f, 1.f }, vmath::Vector2{ 0,0 } });
            ECS::ecs().AddComponent(testEntity, Visible{ false });
            ECS::ecs().AddComponent(testEntity, Tex{ texList.Add("cat.png") });
            Tex* t = &ECS::ecs().GetComponent<Tex>(testEntity);
            t->texVariants.push_back(texList.Add("cat.png"));
            t->texVariants.push_back(texList.Add("cat.png"));
            t->tex = t->texVariants.at(0);
            ECS::ecs().AddComponent(testEntity, Animation{});
            Animation* a = &ECS::ecs().GetComponent<Animation>(testEntity);
            a->animationType = Animation::ANIMATION_TIME_BASED;
            a->frameDisplayDuration = 0.1f;
            ECS::ecs().AddComponent(testEntity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
            ECS::ecs().AddComponent(testEntity, Model{});
            ECS::ecs().AddComponent<physics::Body>(testEntity, physics::Body{});
            ECS::ecs().AddComponent<Collider>(testEntity, Collider{});
            entityAddedFlag = true;
            counter++;
            DEBUG_PRINT("entity ID: %i", testEntity)
        }
        ImGui::Text("Entities added = %d", counter);
        if (ImGui::Button("Clone Entity")) {
            if (!entityAddedFlag) {
                ASSERT(!entityAddedFlag, "Entity has not been added.");
            }
            else {
                CloneMasterModel2(0.f, 0.f, true);
            }
        }
        if (ImGui::Button("Save my Cat")) {
            if (!entityAddedFlag) {
                ASSERT(!entityAddedFlag, "Entity has not been added.");
            }
            else {
                Serializer::SaveEntityToJson("../Assets/CatTestEntity.json", masterEntitiesList["CatTest"]);
            }
        }
        if (ImGui::Button("Load my Cat")) {
            Serializer::LoadEntityFromJson("../Assets/CatTestEntity.json");
            entityAddedFlag = true;

        }
        ImGui::SameLine();

    }
    ImGui::End();
}