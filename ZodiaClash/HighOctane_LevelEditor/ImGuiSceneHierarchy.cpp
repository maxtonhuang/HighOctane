#include "ImGuiSceneHierarchy.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Global.h"
#include <string>
#include "WindowsInterlink.h"
#include "AssetManager.h"
#include "vmath.h"
#include "ScriptEngine.h"

Entity currentSelectedEntity{};
static bool check;
extern std::vector<std::string> fullNameVecImGUI;

// Helper function declaration
void AddScriptToEntity(Entity entity, const char* scriptName);
void RemoveScriptFromEntity(Entity entity, const char* scriptName);


void UpdateSceneHierachy() {
	ImGui::Begin("Scene Hierarchy");
	for (const Entity& entity : s_ptr->m_Entities) {
		if (ECS::ecs().HasComponent<Name>(entity)) {
			SceneEntityNode(entity);
		}
	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		currentSelectedEntity = {};
	}
	ImGui::End();

	ImGui::Begin("Properties");
	if (currentSelectedEntity) {
		SceneEntityComponents(currentSelectedEntity);
	}
	/*Entity entity;
	
	if(ECS::ecs().HasComponent<Name>(entity)) {
		if()
		SceneEntityComponents(entity);
	}*/
	ImGui::End();
}

void SceneEntityNode(Entity entity) {
	if (ECS::ecs().HasComponent<Name>(entity)) {
		auto& entityName = ECS::ecs().GetComponent<Name>(entity).name;
		auto& entitySelected = ECS::ecs().GetComponent<Name>(entity).selected;

		ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_OpenOnArrow;
		if (entity == currentSelectedEntity) {
			flags |= ImGuiTreeNodeFlags_Selected; // Add the selected flag for the current entity
		}
		bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uintptr_t>(entity)), flags, entityName.c_str());
		if (ImGui::IsItemClicked() || entitySelected == true) {
			currentSelectedEntity = entity;
		}

		if (opened) {
			ImGui::TreePop();
		}

	}
}

void SceneEntityComponents(Entity entity) {
	if (ECS::ecs().HasComponent<Name>(entity)) {
		auto& entityName = ECS::ecs().GetComponent<Name>(entity).name;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), entityName.c_str());
		if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
			entityName = std::string(buffer);
		}
	}

	if (ImGui::Checkbox("Movement",&check)) {
		
	}
	if (ECS::ecs().HasComponent<Color>(entity)) {
		if (ImGui::TreeNodeEx((void*)typeid(Color).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Color")) {
			auto& colorComponent = ECS::ecs().GetComponent<Color>(entity);
			//ImVec4 imColor = ((ImVec4)color.color);
			ImGui::ColorEdit3("Edit Color", (float*)&colorComponent.color);

			ImGui::TreePop();
		}
	}
	if (ECS::ecs().HasComponent<Transform>(entity)) {
		if (ImGui::TreeNodeEx((void*)typeid(Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
			auto& positionComponent = ECS::ecs().GetComponent<Transform>(entity).position;
			auto& rotationComponent = ECS::ecs().GetComponent<Transform>(entity).rotation;
			auto& scaleComponent = ECS::ecs().GetComponent<Transform>(entity).scale;
			ImGui::DragFloat2("Position", &positionComponent[0], 0.5f);
			ImGui::DragFloat("Rotation", &rotationComponent, 0.01f, -(vmath::PI), vmath::PI);
			ImGui::DragFloat("Scale", &scaleComponent,0.5f,1.f,100.f);
			/*const char* rotationOptions[] = { "0 degrees", "90 degrees", "180 degrees", "270 degrees" };
			int currentRotationIndex = static_cast<int>(rotationComponent / 90.0f);
			if (ImGui::Combo("Rotation", &currentRotationIndex, rotationOptions, IM_ARRAYSIZE(rotationOptions))) {
				rotationComponent = static_cast<float>(currentRotationIndex) * 90.0f;
			}*/

			ImGui::TreePop();
		}
	}
	if (ECS::ecs().HasComponent<Tex>(entity)) {
		if (ImGui::TreeNodeEx((void*)typeid(Tex).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Texture")) {
			auto& textureComponent = ECS::ecs().GetComponent<Tex>(entity);
			auto& sizeComponent = ECS::ecs().GetComponent<Size>(entity);
			if (ImGui::Button("Edit Current Texture")) {

				std::string fullFilePath =	OpenSingleFileDialog();
				std::string selectedFile = fullFilePath.substr(fullFilePath.find_last_of("\\")+1);
				if (!selectedFile.empty()) {
					textureComponent.tex = assetmanager.texture.Add(fullFilePath.c_str(), selectedFile.c_str());
					sizeComponent.width = (float)textureComponent.tex->GetWidth();
					sizeComponent.height = (float)textureComponent.tex->GetHeight();
				}
			}
			
			//if(ImGui)
			

			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<Script>(entity)) {

		// If master entity is selected, do not allow editing of scripts
		if (ECS::ecs().HasComponent<Master>(entity)) {
			return;
		}
		
		if (ImGui::TreeNodeEx((void*)typeid(Script).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Scripts")) {
			//auto& scriptComponent = ECS::ecs().GetComponent<Script>(entity);
			//static const char* currentScriptForIMGUI = NULL;
			// Create a combo box to select a script
			//int currentScriptIndex = -1; // Initialize with an invalid index
			if (!fullNameVecImGUI.empty()) {
				//const char* currentScriptName = fullNameVecImGUI[0].c_str();

				// Convert script names to const char*
				std::vector<const char*> scriptNamesCStrings;
				scriptNamesCStrings.reserve(fullNameVecImGUI.size());
				for (const std::string& scriptName : fullNameVecImGUI) {
					scriptNamesCStrings.push_back(scriptName.c_str());
				}


				if (ImGui::BeginCombo("Scripts Available", currentScriptForIMGUI)) {
					for (int n = 0; n < scriptNamesCStrings.size(); n++) {
						bool is_selected = (currentScriptForIMGUI == scriptNamesCStrings[n]);
						if (ImGui::Selectable(scriptNamesCStrings[n], is_selected)) {
							currentScriptForIMGUI = scriptNamesCStrings[n];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			//printf("CurrentScriptfor IMGUI : %s\n", currentScriptForIMGUI);
			//ImGui::SameLine();
			if (ImGui::Button("Add Script")) {
				if (currentScriptForIMGUI == NULL) {
					DEBUG_PRINT("No script selected");
					//printf("No script selected\n");
				}
				else {
					//AddScriptToEntity(entity, currentScriptForIMGUI);
					ScriptEngine::RunTimeAddScript(entity, currentScriptForIMGUI);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete Script")) {
				if (currentScriptForIMGUI == NULL) {
					DEBUG_PRINT("No script selected");
					//printf("No script selected\n");
				}
				else {
					RemoveScriptFromEntity(entity, currentScriptForIMGUI);
				}
			}

			ImGui::TreePop();
			
		}
	}
}


// Helper functions
void AddScriptToEntity(Entity entity, const char* scriptName) {
	Script* s = &ECS::ecs().GetComponent<Script>(entity);

	// Checks if the currentScriptForIMGUI is already in scriptNameVec
	for (int i = 0; i < s->scriptNameVec.size(); i++) {
		if (s->scriptNameVec[i] == scriptName) {
			DEBUG_PRINT("Script %s already exists in entity %d", scriptName, entity);
			//printf("Script %s already exists in entity %d\n", scriptName, entity);
			return;
		}

		else {
			continue;
		}
	}

	std::cout << "AddScriptToEntity::ADD SCRIPT TO ENTITY FUNCTION" << std::endl;
	// If not, add it to the vector
	s->scriptNameVec.push_back(scriptName);

	for (auto& scriptName : s->scriptNameVec) {
		std::cout << "Script name: " << scriptName << std::endl;
	}

	scriptAdded = true;

}

void RemoveScriptFromEntity(Entity entity, const char* scriptName) {
	std::cout << "RemoveScriptFromEntity::REMOVE SCRIPT FROM ENTITY FUNCTION" << std::endl;
	Script* s = &ECS::ecs().GetComponent<Script>(entity);

	// If the scriptNameVec is empty,5 return
	if (s->scriptNameVec.size() <= 0) {
		return;
	}

	// This is here for now to delete the script, but next time it won't be this

	// For every entity, only clear the vec that the entity is 
	s->scriptNameVec.clear();
	//s->scriptNameVecForImGui.clear();
	scriptRemoved = true;

	// Search for the script in scriptNameVec
	//auto it = std::find(s->scriptNameVec.begin(), s->scriptNameVec.end(), scriptName);
	//std::cout << "Script name: " << scriptName << std::endl;
	//scriptRemoved = true;

	// If found, remove
	//if (it != s->scriptNameVec.end()) {
	//	s->scriptNameVec.erase(it);
	//	//DEBUG_PRINT("REMOVESCRIPT: Removing script %s from entity %d", scriptName, entity);
	//	scriptRemoved = true;
	//	return;
	//}
	//else {
	//	//DEBUG_PRINT("REMOVESCRIPT: Script %s not found in entity %d", scriptName, entity)
	//}

	// If the script is found, remove it
	//s->scriptNameVec.erase(it);
	//DEBUG_PRINT("REMOVESCRIPT: Removing script %s from entity %d", scriptName, entity);
	//scriptRemoved = true;
	// If the script is not found, print a message
	//DEBUG_PRINT("REMOVESCRIPT: Script %s not found in entity %d", scriptName, entity);
}