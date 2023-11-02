#include "ImGuiLib.h"
#include "ImGuiLayer.h"
#include "EntityFactory.h"
#include "Layering.h"
#include "Editing.h"
#include <iterator>
#include <limits>
#include "Global.h"

constexpr float buttonHeight = 40.f;
int index{};
//Entity entity{};

void UpdateLayer() {

	// retrieving name that has already been deleted.

	// check selection of layer.

	ImGui::Begin("Layers");
	float bh = buttonHeight + ImGui::GetStyle().ItemSpacing.y;  // Button height + spacing

	ImGui::Text("View");
	ImGui::SameLine();
	ImGui::Text("Lock");

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -bh), false);

	size_t potentialDropTarget = ULLONG_MAX;
	size_t checkboxCounter = 0;

	for (int layer_it = (static_cast<int>(layering.size()) - 1); layer_it >= 0; --layer_it) {

		std::string label1 = "##label" + std::to_string(checkboxCounter++);
		if (ImGui::Checkbox(label1.c_str(), &layersToSkip[layer_it])) {
			UnselectAll();
			SetWholeSkipLayer(static_cast<size_t>(layer_it));
		}
		ImGui::SameLine();
		std::string label2 = "##label" + std::to_string(checkboxCounter++);
		if (ImGui::Checkbox(label2.c_str(), &layersToLock[layer_it])) {
			UnselectAll();
			SetWholeLockLayer(static_cast<size_t>(layer_it));
		}
		ImGui::SameLine();
		if (ImGui::TreeNodeEx(layerNames[layer_it].c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (layer_it == currentLayer ? ImGuiTreeNodeFlags_Selected : 0), layerNames[layer_it].c_str())) {
			
			if (ImGui::IsItemClicked()) {
				UnselectAll();
				for (int entity_it = (static_cast<int>(layering[layer_it].size()) - 1); entity_it >= 0; --entity_it) {
					Name& entityName = ECS::ecs().GetComponent<Name>(layering[layer_it][entity_it]);
					entityName.selected = true;
				}
			}

			if (ImGui::BeginDragDropSource()) {
				index = layer_it;
				ImGui::SetDragDropPayload("LAYER", &index, sizeof(int));
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LAYER")) {
					
					int sourceLayerIndex = *(int*)payload->Data;
					std::deque<Entity> temp = layering[sourceLayerIndex];
					std::string tempName = layerNames[sourceLayerIndex];
					// delete source layer
					layering.erase(layering.begin() + sourceLayerIndex);
					layerNames.erase(layerNames.begin() + sourceLayerIndex);
					// insert source layer into target layer
					layering.insert(layering.begin() + layer_it, temp);
					layerNames.insert(layerNames.begin() + layer_it, tempName);
				}
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY")) {
					
					Entity sourceEntity = *(Entity*)payload->Data;
					// remove entity from source layer
					RemoveEntityFromLayering(sourceEntity);
					// insert entity into target layer
					layering[layer_it].emplace_back(sourceEntity);
				}
				ImGui::EndDragDropTarget();

			}


			for (int entity_it = (static_cast<int>(layering[layer_it].size()) - 1); entity_it >= 0; --entity_it) {
				Name& entityName = ECS::ecs().GetComponent<Name>(layering[layer_it][entity_it]);
				if (entityName.selected) {
					currentLayer = layer_it;
				}
				layersToSkip[layer_it] = CheckSkipLayerAllTrue(static_cast<size_t>(layer_it));
				layersToLock[layer_it] = CheckLockLayerAllTrue(static_cast<size_t>(layer_it));
				std::string label3 = "##label" + std::to_string(checkboxCounter++);
				if (ImGui::Checkbox(label3.c_str(), &entitiesToSkip[static_cast<uint32_t>(layering[layer_it][entity_it])])) {
					UnselectAll();
				}
				ImGui::SameLine();
				std::string label4 = "##label" + std::to_string(checkboxCounter++);
				if (ImGui::Checkbox(label4.c_str(), &entitiesToLock[static_cast<uint32_t>(layering[layer_it][entity_it])])) {
					UnselectAll();
				}
				ImGui::SameLine();
				if (ImGui::TreeNodeEx(entityName.name.c_str(), ImGuiTreeNodeFlags_Leaf | (entityName.selected ? ImGuiTreeNodeFlags_Selected : 0), entityName.name.c_str())) {
					
					if (ImGui::IsItemClicked()) {
						UnselectAll();
						entityName.selected = true;
					}

					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload("ENTITY", &layering[layer_it][entity_it], sizeof(Entity));
						ImGui::EndDragDropSource();
					}

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY")) {
							Entity sourceEntity = *(Entity*)payload->Data;
							// remove entity from source layer
							RemoveEntityFromLayering(sourceEntity);
							// insert entity into target layer
							layering[layer_it].insert(layering[layer_it].begin() + entity_it, sourceEntity);
						}
						ImGui::EndDragDropTarget();
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