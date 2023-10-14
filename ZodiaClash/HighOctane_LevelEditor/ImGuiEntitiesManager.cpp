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
        if (ImGui::Button("Test Loader")) {
            Serializer::LoadEntityFromJson("../Assets/Scenes/SceneEntities.json");
            entityAddedFlag = true;

        } 
        if (ImGui::Button("Save Scene")) {
            button_clicked = true;
        }
        if (ImGui::Button("Clone Entity")) {
            CloneMasterModel(0.f, 0.f, false);
        }



       /*************************Testing for ImGui creating of entities*************************************************/
        Entity testEntity;
        if (ImGui::Button("Create Entity by Button Test: Cat")) {
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
            ECS::ecs().AddComponent<physics::Body>(testEntity, physics::Body{0,0});
            ECS::ecs().AddComponent<Collider>(testEntity, Collider{});
            entityAddedFlag = true;
            counter++;
            DEBUG_PRINT("entity ID: %i", testEntity);
        }
        ImGui::Text("Entities added = %d", counter);
        if (ImGui::Button("Clone Cat Entity")) {
            if (!entityAddedFlag) {
                ASSERT(!entityAddedFlag, "Entity has not been added. Click NO and click on: \"Create Entity by Button Test: Cat\" ");
            }
            else {
                CloneMasterModel2(0.f, 0.f, false);
            }
        }
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