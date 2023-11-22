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
#include "UIComponents.h"
#include "EntityFactory.h"


/****************************FOR FONTS***********************************/
// Define the preset colors
const char* presetColors[] = {
    "red", "green", "blue", "white", "black",
    "primary", "secondary", "success", "danger", "fatal", "warning", "info"
};

float presetColorValues[][4] = {
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.48f, 1.0f, 1.0f},
    {0.42f, 0.45f, 0.49f, 1.0f},
    {0.15f, 0.65f, 0.27f, 1.0f},
    {0.86f, 0.2f, 0.27f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.75f, 0.02f, 1.0f},
    {0.09f, 0.63f, 0.72f, 1.0f}
};

bool showColorPicker = false;
int selectedColorIndex = -1; // Index of the selected preset color

/****************************FOR UI***********************************/
bool showFontEntityConfig = false;
bool showHealthBarEntityConfig = false;

/****************************FOR AUDIO***********************************/
bool showAudioEntityConfig = false;

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

        if (ImGui::Button("Create Entity")) {
            ImGui::OpenPopup("Entity Type");
        }

        // Handle entity type selection in a centered popup modal
        if (ImGui::BeginPopupModal("Entity Type", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::MenuItem("Audio Entity")) {
                showFontEntityConfig = false;
                showAudioEntityConfig = true;
                showHealthBarEntityConfig = false;
                showColorPicker = false;
            }
            if (ImGui::MenuItem("Text Label Entity")) {
                showFontEntityConfig = true;
                showAudioEntityConfig = false;
                showHealthBarEntityConfig = false;
                showColorPicker = false;
            }
            if (ImGui::MenuItem("Health Bar Entity")) {
                showFontEntityConfig = false;
                showAudioEntityConfig = false;
                showHealthBarEntityConfig = true;
                showColorPicker = false;
            }
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        //*****************************TEXT LABELS********************//
        // Display the font entity configuration options if selected
        if (showFontEntityConfig) {
            //Entity fontEntity = ECS::ecs().CreateEntity();
            static std::string entityName; //For name
            static std::string inputText; //For input text

            static std::string presetSelected;

            static glm::vec4 chosenColor{ 0.f,0.f,0.f,1.f };

            static glm::vec4 clr;

            static bool presetColorSelected = false;
            static bool selfChosenColor = false;


            ImGui::OpenPopup("Text Label Entity");
            // Create the centered popup modal for font entity configuration
            if (ImGui::BeginPopupModal("Text Label Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                // Add your font entity configuration UI here
                ImGui::Text("Text Label Entity Configuration");
                /*********************ENTER NAME********************************/
              
                {
                    char buffer[256];
                    memset(buffer, 0, sizeof(buffer));
                    /*if (entityName.empty()) {
                        entityName = "Font Entity";
                    }*/
                    strcpy_s(buffer, sizeof(buffer), entityName.c_str());
                    if (ImGui::InputText("Entity Name", buffer, sizeof(buffer))) {
                        entityName = std::string(buffer);
                    }
                }
                {
                    char buffer[256];
                    memset(buffer, 0, sizeof(buffer));
                    /*if (inputText.empty()) {
                        inputText = "Input Text Here";
                    }*/
                    strcpy_s(buffer, sizeof(buffer), inputText.c_str());
                    if (ImGui::InputText("Input Text", buffer, sizeof(buffer))) {
                        inputText = std::string(buffer);

                    }
                }
                
                /*********************CHOOSE COLOR********************************/
                if (ImGui::Button("Choose Color of Text")) {
                    showColorPicker = true;
                }
                // Show the color picker in a centered popup modal
                if (showColorPicker) {
           
                        ImGui::Text("Choose a Preset Color OR create your own!");
                        if (ImGui::Combo("##ColorCombo", &selectedColorIndex, presetColors, 12)) {
                            // Handle the color selection here
                            // 'selectedColorIndex' will contain the index of the selected color
                            //ECS::ecs().AddComponent<TextLabel>(fontEntity, TextLabel{ inputText, presetColors[selectedColorIndex]});
                            presetSelected = presetColors[selectedColorIndex];
                            presetColorSelected = true;
                            
                        }
                        ImGui::Text("OR");
                        if (ImGui::ColorEdit4("Select Color", (float*)&chosenColor)) {
                            clr = chosenColor;
                            selfChosenColor = true;
                        }
                }

                if (ImGui::Button("Close")) {
                    showFontEntityConfig = false;
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Create")) {
                    Entity createFontEntity = ECS::ecs().CreateEntity();
                    ECS::ecs().AddComponent<Name>(createFontEntity, Name{ entityName});
                    if (presetColorSelected) {
                        ECS::ecs().AddComponent<TextLabel>(createFontEntity, TextLabel{ inputText, presetColors[selectedColorIndex] });
                        presetColorSelected = false;
                    }
                    if (selfChosenColor) {
                        ECS::ecs().AddComponent<TextLabel>(createFontEntity, TextLabel{ inputText, clr });
                        selfChosenColor = false;
                    }
                    ECS::ecs().AddComponent<Transform>(createFontEntity, Transform{ });
                    ECS::ecs().AddComponent<Size>(createFontEntity, Size{100.f,100.f});
                    ECS::ecs().AddComponent<Model>(createFontEntity, Model{ ModelType::UI });
                    ECS::ecs().AddComponent<Clone>(createFontEntity, Clone{});
                    ECS::ecs().AddComponent<Movable>(createFontEntity, Movable{});
                    layering[selectedLayer].emplace_back(createFontEntity);
                    EntityFactory::entityFactory().cloneCounter++;
                    showFontEntityConfig = false;
                    ImGui::CloseCurrentPopup();
                    entityName.clear();
                    inputText.clear();
                    presetSelected.clear();
                    chosenColor = { 0.f,0.f,0.f,1.f };
                }
                ImGui::EndPopup();
            }
        }
        
        //************************HEALTH BAR******************************************//
        // Display the hp bar entity configuration options if selected
        if (showHealthBarEntityConfig) {
            // Add your audio entity configuration UI here
            ImGui::OpenPopup("Health Bar Entity");
            // Create the centered popup modal for hp bar entity configuration
            if (ImGui::BeginPopupModal("Health Bar Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                // Add your hp bar entity configuration UI here
                Entity createHealthBarEntity = ECS::ecs().CreateEntity();
                ECS::ecs().AddComponent<Name>(createHealthBarEntity, Name{ "healthBar"});
                ECS::ecs().AddComponent<Transform>(createHealthBarEntity, Transform{ });
                ECS::ecs().AddComponent<Size>(createHealthBarEntity, Size{ 100.f,100.f });
                ECS::ecs().AddComponent<Model>(createHealthBarEntity, Model{ ModelType::UI });
                ECS::ecs().AddComponent<Clone>(createHealthBarEntity, Clone{});
                ECS::ecs().AddComponent<Movable>(createHealthBarEntity, Movable{});

                ECS::ecs().AddComponent<CharacterStats>(createHealthBarEntity, CharacterStats{});
                ECS::ecs().GetComponent<CharacterStats>(createHealthBarEntity).stats.maxHealth = 1000.f;
                ECS::ecs().AddComponent<TextLabel>(createHealthBarEntity, TextLabel{});
                ECS::ecs().GetComponent<TextLabel>(createHealthBarEntity).hasBackground = 1;
                ECS::ecs().AddComponent<HealthBar>(createHealthBarEntity, HealthBar{ ECS::ecs().GetComponent<CharacterStats>(createHealthBarEntity) });
                layering[selectedLayer].emplace_back(createHealthBarEntity);
                EntityFactory::entityFactory().cloneCounter++;
                showHealthBarEntityConfig = false;
                ImGui::EndPopup();
            }
        }

        
        //************************AUDIO**********************************************//
        // Display the audio entity configuration options if selected
        if (showAudioEntityConfig) {
            // Add your audio entity configuration UI here
            ImGui::OpenPopup("AudioEntityPopup");
            // Create the centered popup modal for audio entity configuration
            if (ImGui::BeginPopupModal("AudioEntityPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                // Add your audio entity configuration UI here
                ImGui::Text("Audio is present, however creating as an entitiy is still WIP");
                if (ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                    showAudioEntityConfig = false;
                }
                ImGui::EndPopup();
            }
        }


    }
    ImGui::End();
}