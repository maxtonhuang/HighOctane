#include "ImGuiComponents.h"
#include "ImGuiSceneHierarchy.h"
#include "ECS.h"
#include <sstream>

void UpdateComponentViewer() {
	static auto& typeMap{ ECS::ecs().GetTypeManager() };
	ImGui::Begin("Components");

	if (currentSelectedEntity) {
		int buttonID{0};

		ImGui::Text("Current Components");

		ImVec2 padding{};
		for (auto& ecsType : typeMap) {
			ImVec2 checkPadding{ ImGui::CalcTextSize(ecsType.first.c_str()) };
			if (checkPadding.x > padding.x) {
				padding = checkPadding;
			}
		}

		for (auto& ecsType : typeMap) {
			if (ecsType.second->HasComponent(currentSelectedEntity)) {
				ImGui::Text(ecsType.first.c_str());
				float paddingSpaceX{ padding.x - ImGui::CalcTextSize(ecsType.first.c_str()).x };
				if (paddingSpaceX > 0) {
					ImGui::SameLine(0, 0);
					ImGui::InvisibleButton("  ", ImVec2{ paddingSpaceX, padding.y });
				}
				ImGui::SameLine();
				ImGui::PushID(buttonID++);
				if (ImGui::Button("Remove")) {
					ecsType.second->RemoveComponent(currentSelectedEntity);
				}
				ImGui::PopID();
			}
		}

		ImGui::NewLine();
		ImGui::Text("New Components");

		for (auto& ecsType : typeMap) {
			if (!ecsType.second->HasComponent(currentSelectedEntity)) {
				ImGui::Text(ecsType.first.c_str());
				float paddingSpaceX{ padding.x - ImGui::CalcTextSize(ecsType.first.c_str()).x };
				if (paddingSpaceX > 0) {
					ImGui::SameLine(0, 0);
					ImGui::InvisibleButton("  ", ImVec2{ paddingSpaceX, padding.y });
				}
				ImGui::SameLine();
				ImGui::PushID(buttonID++);
				if (ImGui::Button("Add")) {
					ecsType.second->AddComponent(currentSelectedEntity);
				}
				ImGui::PopID();
			}
		}
	}

	ImGui::End();
}
