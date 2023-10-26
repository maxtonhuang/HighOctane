#include "ImGuiSceneHierarchy.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Global.h"
#include <string>
#include "WindowsInterlink.h"
#include "AssetManager.h"
#include "vmath.h"
Entity currentSelectedEntity{};

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
		bool opened = ImGui::TreeNodeEx((void*)entity, flags, entityName.c_str());
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
		if (ImGui::TreeNodeEx((void*)typeid(Script).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Scripts")) {
			auto& scriptComponent = ECS::ecs().GetComponent<Script>(entity);

			// Create a combo box to select a script
			int currentScriptIndex = -1; // Initialize with an invalid index
			if (!scriptComponent.scriptNameVec.empty()) {
				const char* currentScriptName = scriptComponent.scriptNameVec[0].c_str();

				// Convert script names to const char*
				std::vector<const char*> scriptNamesCStrings;
				scriptNamesCStrings.reserve(scriptComponent.scriptNameVec.size());
				for (const std::string& scriptName : scriptComponent.scriptNameVec) {
					scriptNamesCStrings.push_back(scriptName.c_str());
				}

				if (ImGui::Combo("Select Script", &currentScriptIndex, scriptNamesCStrings.data(), static_cast<int>(scriptNamesCStrings.size()))) {
					// Update the selected script in the component
					if (currentScriptIndex >= 0) {
						scriptComponent.scriptNameVec[0] = scriptComponent.scriptNameVec[currentScriptIndex];
					}
				}
			}


			ImGui::TreePop();
		}
	}
}