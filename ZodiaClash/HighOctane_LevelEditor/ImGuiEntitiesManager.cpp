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
*	@file		ImGuiEntitiesManager.cpp
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
*	@date		10 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains all the definitions for the panel that handles entities and scenes
******************************************************************************/

#include "ImGuiEntitiesManager.h"
#include "EntityFactory.h"
#include "enginecore.h"
#include "physics.h"
#include "Serialization.h"
#include "ECS.h"
#include "Global.h"
#include "AssetManager.h"

void InitEntitiesManager() {
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();
}

void UpdateEntitiesManager() {
    {
        static int counter = 0;
        static bool entityAddedFlag = false;

        ImGui::Begin("Creating Entity");

        /*************************Testing for Serialization and Deserialization*************************************************/
        
        if (ImGui::Button("Clone Duck Entity")) {
            EntityFactory::entityFactory().CloneMasterModel(0.f, 0.f, false);
        }


        if (ImGui::Button("Create new entity")) {
            // Show an input dialog for the entity name
            ImGui::OpenPopup("Create Entity");
        }

        // Step 2: Input dialog for the entity name
        if (ImGui::BeginPopupModal("Create Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char entityName[128] = "";
            ImGui::InputText("Name", entityName, IM_ARRAYSIZE(entityName));

            if (ImGui::Button("Add Component")) {
                ImGui::OpenPopup("Add Component");
            }

            if (ImGui::Button("Create")) {
                // Step 4: Create the entity with the given name
                //Entity newEntity = EntityFactory::CreateEntity(entityName);

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // Step 5: Add Component dialog
        if (ImGui::BeginPopupModal("Add Component", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static int selectedComponent = -1;
            const char* componentNames[] = { "Transform", "Texture", /* Add more components here */ };

            if (ImGui::Combo("Select Component", &selectedComponent, componentNames, IM_ARRAYSIZE(componentNames))) {
                // Step 6: Based on the selected component, display relevant configuration fields
                if (selectedComponent == 0) { // Transform
                    // Show input fields for position, rotation, scale, etc.
                    // Retrieve user inputs and create the component.
                }
                // Implement similar logic for other components.

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

       /*************************Testing for ImGui creating of entities*************************************************/
        //Entity testEntity;
        //if (ImGui::Button("Create Entity by Button Test: Cat")) {
        //    testEntity = ECS::ecs().CreateEntity();
        //    EntityFactory::entityFactory().masterEntitiesList["CatTest"] = testEntity;
        //    ECS::ecs().AddComponent(testEntity, Color{ glm::vec4{ 1,1,1,1 } });
        //    ECS::ecs().AddComponent(testEntity, Transform{ Vec2{ 0.f,0.f }, 0.f, 1.f, vmath::Vector2{ 0,0 } });
        //    ECS::ecs().AddComponent(testEntity, Visible{ false });
        //    ECS::ecs().AddComponent(testEntity, Tex{ assetmanager.texture.Get("cat.png") });
        //    Tex* t = &ECS::ecs().GetComponent<Tex>(testEntity);
        //    t->texVariants.push_back(assetmanager.texture.Get("cat.png"));
        //    t->texVariants.push_back(assetmanager.texture.Get("cat.png"));
        //    t->tex = t->texVariants.at(0);
        //    ECS::ecs().AddComponent(testEntity, Animator{ Animator::ANIMATION_TIME_BASED, 0.1f });
        //    ECS::ecs().AddComponent(testEntity, Size{ static_cast<float>(t->tex->GetWidth()), static_cast<float>(t->tex->GetHeight()) });
        //    ECS::ecs().AddComponent(testEntity, Model{});
        //    //ECS::ecs().AddComponent<physics::Body>(testEntity, physics::Body{0,0});
        //    ECS::ecs().AddComponent<Collider>(testEntity, Collider{});
        //    entityAddedFlag = true;
        //    counter++;
        //    DEBUG_PRINT("entity ID: %i", testEntity);
        //}
        //ImGui::Text("Entities added = %d", counter);
        //if (ImGui::Button("Clone Cat Entity")) {
        //    if (!entityAddedFlag) {
        //        ASSERT(!entityAddedFlag, "Entity has not been added. Click NO and click on: \"Create Entity by Button Test: Cat\" ");
        //    }
        //    else {
        //        EntityFactory::entityFactory().CloneMasterModel2(0.f, 0.f, false);
        //    }
        //}
        /*******************************Testing Feature: For Future saving of master entities************************************/
        /*if (ImGui::Button("Save Master Entity")) {
            if (!entityAddedFlag) {
                ASSERT(!entityAddedFlag, "Entity has not been added.");
            }
            else {
               Serializer::SaveEntityToJson("../Assets/CatTestEntity.json", masterEntitiesList["CatTest"]);
            }
        }*/

    }
    ImGui::End();
}