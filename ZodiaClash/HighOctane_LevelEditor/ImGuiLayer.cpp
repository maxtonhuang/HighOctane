#include "ImGuiLib.h"
#include "ImGuiLayer.h"
#include "EntityFactory.h"
#include "Layering.h"
#include <iterator>
#include "Global.h"

constexpr float buttonHeight = 40.f;

void UpdateLayer() {




	// retrieving name that has already been deleted.

	// check selection of layer.






	ImGui::Begin("Layers");
	float bh = buttonHeight + ImGui::GetStyle().ItemSpacing.y;  // Button height + spacing
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -bh), false);





	for (auto layer_it = layering.rbegin(); layer_it != layering.rend(); ++layer_it) {

		size_t idx = std::distance(layer_it, layering.rend()) - 1;
		if ( ImGui::TreeNodeEx(layerNames[idx].c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (idx == currentLayer ? ImGuiTreeNodeFlags_Selected : 0), layerNames[idx].c_str()) ) {

			for (auto entity_it = layer_it->rbegin(); entity_it != layer_it->rend(); ++entity_it) {
				auto& entityName = ECS::ecs().GetComponent<Name>(*entity_it);
				if (entityName.selected) {
					currentLayer = idx;
				}
				if ( ImGui::TreeNodeEx((std::to_string(*entity_it)).c_str(), ImGuiTreeNodeFlags_Leaf | (entityName.selected ? ImGuiTreeNodeFlags_Selected : 0), entityName.name.c_str())) {
					// keep the node selected
					//entityName.selected = true;
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

	}
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



}