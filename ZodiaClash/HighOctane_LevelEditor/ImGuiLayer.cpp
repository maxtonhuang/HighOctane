#include "ImGuiLib.h"
#include "ImGuiLayer.h"
#include "EntityFactory.h"
#include "Layering.h"
#include "Editing.h"
#include <iterator>
#include "Global.h"

constexpr float buttonHeight = 40.f;

void UpdateLayer() {

	// retrieving name that has already been deleted.

	// check selection of layer.

	ImGui::Begin("Layers");
	float bh = buttonHeight + ImGui::GetStyle().ItemSpacing.y;  // Button height + spacing
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -bh), false);

	// rewrite to use traditional i

	///// Here

	for (int layer_it = (static_cast<int>(layering.size()) - 1); layer_it >= 0; --layer_it) {
		if (ImGui::TreeNodeEx(layerNames[layer_it].c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (layer_it == currentLayer ? ImGuiTreeNodeFlags_Selected : 0), layerNames[layer_it].c_str())) {
			if (ImGui::IsItemClicked()) {
				UnselectAll();
				for (int entity_it = (static_cast<int>(layering[layer_it].size()) - 1); entity_it >= 0; --entity_it) {
					Name& entityName = ECS::ecs().GetComponent<Name>(layering[layer_it][entity_it]);
					entityName.selected = true;
				}
			}
			for (int entity_it = (static_cast<int>(layering[layer_it].size()) - 1); entity_it >= 0; --entity_it) {
				Name& entityName = ECS::ecs().GetComponent<Name>(layering[layer_it][entity_it]);
				if (entityName.selected) {
					currentLayer = layer_it;
				}
				if (ImGui::TreeNodeEx(entityName.name.c_str(), ImGuiTreeNodeFlags_Leaf | (entityName.selected ? ImGuiTreeNodeFlags_Selected : 0), entityName.name.c_str())) {
					
					if (ImGui::IsItemClicked()) {
						UnselectAll();
						entityName.selected = true;
					}

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}





	///// To here

	ImGui::EndChild();

	float panelWidth = ImGui::GetContentRegionAvail().x;
	if (ImGui::Button("New Layer", {panelWidth / 2.f, buttonHeight})) {

		// Add New Layer
		CreateNewLayer();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Layer", {panelWidth / 2.f, buttonHeight})) {
		// Delete Layer
		DeleteLayer();
	}

	ImGui::End();

	//printf("XXXXXXXXXX--- END GUILAYER SYSTEM ---XXXXXXXXXX");


}