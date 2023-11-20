#include "ImGuiComponents.h"
#include "ImGuiSceneHierarchy.h"
#include "ECS.h"
#include <sstream>

void UpdateComponentViewer() {
	
	ImGui::Begin("Components");

	ComponentBrowser(currentSelectedEntity);

	ImGui::End();
}

void ComponentBrowser(Entity currentEntity) {
	static auto& typeMap{ ECS::ecs().GetTypeManager() };

	if (currentEntity) {
		int buttonID{ 0 };
		bool hasPrefab{ false };
		std::unordered_set<std::string>* uniqueComponents{};

		if (ECS::ecs().HasComponent<Clone>(currentEntity)) {
			if (ECS::ecs().GetComponent<Clone>(currentEntity).prefab != "") {
				hasPrefab = true;
				uniqueComponents = &ECS::ecs().GetComponent<Clone>(currentEntity).unique_components;
			}
		}

		ImVec2 padding{};
		for (auto& ecsType : typeMap) {
			std::string typeString{ ecsType.first.substr(ecsType.first.find_first_of(" ") + 1) };
			ImVec2 checkPadding{ ImGui::CalcTextSize(typeString.c_str()) };
			if (checkPadding.x > padding.x) {
				padding = checkPadding;
			}
		}

		ImGui::Text("Current Components");

		for (auto& ecsType : typeMap) {
			if (ecsType.second->HasComponent(currentEntity)) {
				std::string typeString{ ecsType.first.substr(ecsType.first.find_first_of(" ") + 1) };
				ImGui::Text(typeString.c_str());
				float paddingSpaceX{ padding.x - ImGui::CalcTextSize(typeString.c_str()).x };
				if (paddingSpaceX > 0) {
					ImGui::SameLine(0, 0);
					ImGui::InvisibleButton("  ", ImVec2{ paddingSpaceX, padding.y });
				}
				ImGui::SameLine();
				ImGui::PushID(buttonID++);
				if (ImGui::Button("Remove")) {
					ecsType.second->RemoveComponent(currentEntity);
				}
				ImGui::PopID();

				if (hasPrefab) {
					bool isUnique{ (bool)(uniqueComponents->count(ecsType.first)) };
					bool checkbox{ isUnique };
					ImGui::SameLine();
					ImGui::PushID(buttonID++);
					ImGui::Checkbox("Unique", &checkbox);
					ImGui::PopID();
					if (checkbox != isUnique && checkbox == false) {
						uniqueComponents->erase(ecsType.first);
					}
					else if (checkbox != isUnique && checkbox == true) {
						uniqueComponents->insert(ecsType.first);
					}
				}
			}
		}

		ImGui::Separator();
		ImGui::Text("New Components");

		for (auto& ecsType : typeMap) {
			if (!ecsType.second->HasComponent(currentEntity)) {
				std::string typeString{ ecsType.first.substr(ecsType.first.find_first_of(" ") + 1) };
				ImGui::Text(typeString.c_str());
				float paddingSpaceX{ padding.x - ImGui::CalcTextSize(typeString.c_str()).x };
				if (paddingSpaceX > 0) {
					ImGui::SameLine(0, 0);
					ImGui::InvisibleButton("  ", ImVec2{ paddingSpaceX, padding.y });
				}
				ImGui::SameLine();
				ImGui::PushID(buttonID++);
				if (ImGui::Button("Add")) {
					ecsType.second->AddComponent(currentEntity);
				}
				ImGui::PopID();
			}
		}
	}
}
