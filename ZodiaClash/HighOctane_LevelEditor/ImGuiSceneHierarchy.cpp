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
*	@file		ImGuiSceneHiearchy.cpp
*
*	@author		Kai Alexander Van Adrichem Boogaert (Scene Hiaerarchy and Properties panel)
*
*	@email		kaialexander.v\@digipen.edu
*
*
*	@co-author	Foong Pun Yuen Nigel (Prefabs editor)
*
*	@email		p.foong\@digipen.edu
*
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		09 November 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains all the definitions for scene hierarchy, prefabs editor, 
*	and properties editor panel.
******************************************************************************/
#include "ImGuiSceneHierarchy.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Global.h"
#include <string>
#include "WindowsInterlink.h"
#include "AssetManager.h"
#include "vmath.h"
#include <variant>
#include "model.h"
#include "UIComponents.h"
#include "ScriptEngine.h"
#include "CharacterStats.h"
#include "Scripting.h"
#include <sstream>
#include "ImGuiComponents.h"
#include "Serialization.h"
#include "UndoRedo.h"
#include "Selection.h"


Entity currentSelectedPrefab;

/*--------Forward declaration for scripting--------*/
void DrawScriptTreeWithImGui(const std::string& className, Entity entity, int i);

/*--------Forward declaration for scripting--------*/

/*--------Variables for scripting--------*/

Entity currentSelectedEntity{};

extern std::vector<std::string> fullNameVecImGUI;
/*--------Variables for scripting--------*/

static bool check;

//FOR PREFAB HIERACHY
std::string prefabName{};

void UpdateSceneHierachy() {
	ImGui::Begin("Scene Hierarchy");
	auto& nameArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Name>();
	auto& cloneArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Clone>();
	for (const Entity& entity : s_ptr->m_Entities) {
		if (cloneArray.HasComponent(entity)) {
			if (nameArray.HasComponent(entity)) {
				SceneEntityNode(entity);
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
					UnselectAll();
					currentSelectedEntity = entity;
					nameArray.GetData(entity).selected = true;
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
					UnselectAll();
					currentSelectedEntity = entity;
					nameArray.GetData(entity).selected = true;
					ImGui::OpenPopup("EntityContextMenu");
				}
			}
		}
	}
	if (ImGui::BeginPopup("EntityContextMenu")) {
		if (ImGui::MenuItem("Delete")) {
			toDestroy = true;
		}
		if (ImGui::MenuItem("Copy")) {
			toCopy = true;
		}
		ImGui::EndPopup();
	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		UnselectAll();
		currentSelectedEntity = {};
	}
	ImGui::End();

	ImGui::Begin("Properties");
	//if (currentSelectedEntity && selectedEntities.size() == 1) {
	if (ECS::ecs().EntityExists(currentSelectedEntity) && ECS::ecs().HasComponent<Clone>(currentSelectedEntity)) {
		SceneEntityComponents(currentSelectedEntity);
	}
	/*Entity entity;
	
	if(ECS::ecs().HasComponent<Name>(entity)) {
		if()
		SceneEntityComponents(entity);
	}*/
	ImGui::End();
}

void UpdatePrefabHierachy() {
	static float saveTimer{};
	ImGui::Begin("Prefab Editor");

	auto prefabList{ assetmanager.GetPrefabPaths() };
	currentSelectedPrefab =  assetmanager.GetPrefab(prefabName);

	if (ImGui::BeginCombo("Prefabs Available", prefabName.c_str())) {
		for (int n = 0; n < prefabList.size(); n++) {
			bool is_selected = (prefabName == prefabList[n]);
			if (ImGui::Selectable(prefabList[n].c_str(), is_selected)) {
				prefabName = prefabList[n];
				if (assetmanager.GetPrefab(prefabName) == 0) {
					assetmanager.LoadPrefab(prefabName);
				}
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB_ITEM")) {
			std::string droppedItemPath = (const char*)payload->Data;
			prefabName = droppedItemPath;
			if (assetmanager.GetPrefab(prefabName) == 0) {
				assetmanager.LoadPrefab(prefabName);
			}
			ImGui::SetItemDefaultFocus();
		}
		ImGui::EndDragDropTarget();
	}
	if (currentSelectedPrefab) {
		if (ImGui::Button("Save Prefab")) {
			std::string prefabPath{ assetmanager.GetDefaultPath() + "Prefabs/" + prefabName};
			SaveAsPrefab(prefabPath, currentSelectedPrefab);
			saveTimer = 1.f;
		}

		if (saveTimer > 0.f) {
			ImGui::SameLine();
			ImGui::Text("Prefab saved!");
		}

		if (ImGui::Button("Create Instance")) {
			Entity clone = EntityFactory::entityFactory().CloneMaster(currentSelectedPrefab);
			ECS::ecs().GetComponent<Clone>(clone).prefab = prefabName;
		}

		SceneEntityComponents(currentSelectedPrefab);
		ImGui::Separator();
		ComponentBrowser(currentSelectedPrefab);
	}

	if (saveTimer > 0.f) {
		saveTimer -= g_dt;
	}

	auto& cloneArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Clone>() };
	auto& typeManager{ ECS::ecs().GetTypeManager() };
	auto cloneIDArray{ cloneArray.GetEntityArray() };
	
	//Real-time prefab updating
	if (GetCurrentSystemMode() == SystemMode::EDIT && ImGui::IsWindowFocused()) {
		for (auto& cloneEntity : cloneIDArray) {
			Clone clone{ cloneArray.GetData(cloneEntity) };
			if (clone.prefab == prefabName && prefabName != "") {
				for (auto& ecsType : typeManager) {
					if (ecsType.second->HasComponent(currentSelectedPrefab) && !(bool)(clone.unique_components.count(ecsType.second->name))) {
						ecsType.second->CopyComponent(cloneEntity, currentSelectedPrefab);
					}
				}
			}
		}
	}
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
	auto& componentManager{ ECS::ecs().GetComponentManager() };

	if (ECS::ecs().HasComponent<Clone>(entity)) {
		if (!ECS::ecs().HasComponent<Child>(entity)) {
			auto& entityClone{ ECS::ecs().GetComponent<Clone>(entity) };
			if (entityClone.prefab == "") {
				ImGui::Text("Entity has no prefabs");
				if (ImGui::Button("Save as prefab")) {
					std::string prefabPath{ SaveFileDialog("*.prefab","Prefab") };
					SaveAsPrefab(prefabPath, entity);
				}
			}
			else {
				std::string text{ "Prefab: " + entityClone.prefab };
				ImGui::Text(text.c_str());
				if (ImGui::Button("Select in Prefab Editor")) {
					prefabName = entityClone.prefab;
				}
				if (ImGui::Button("Save as new prefab")) {
					std::string prefabPath{ SaveFileDialog("*.prefab","Prefab") };
					SaveAsPrefab(prefabPath, entity);
				}
			}
		}
		else {
			Child& entityChild{ ECS::ecs().GetComponent<Child>(entity) };
			std::string parentlabel{ "Parent: " };
			parentlabel += ECS::ecs().GetComponent<Name>(entityChild.parent).name;
			ImGui::Text(parentlabel.c_str());
			if (ImGui::Button("Select parent")) {
				currentSelectedEntity = entityChild.parent;
			}
		}
	}
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

	if (ECS::ecs().HasComponent<Parent>(entity)) {
		if (ImGui::TreeNodeEx((void*)typeid(Parent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Parent")) {

			auto& entityParent = ECS::ecs().GetComponent<Parent>(entity);
			auto& nameArray{ componentManager.GetComponentArrayRef<Name>() };

			if (entityParent.children.size() > 0) {
				ImGui::Text("Children:");
				for (auto& child : entityParent.children) {
					ImGui::Text(nameArray.GetData(child).name.c_str());
				}
			}

			auto namePairArray{ nameArray.GetPairArray() };
			std::vector<std::pair<Entity, Name*>> validChildArray{};
			for (auto& namePair : namePairArray) {
				if (ECS::ecs().HasComponent<Child>(namePair.first)) {
					continue;
				}
				if (!ECS::ecs().HasComponent<Clone>(namePair.first)) {
					continue;
				}
				if (namePair.second->name == "") {
					continue;
				}
				validChildArray.push_back(namePair);
			}

			static std::pair<Entity, std::string> preview{ validChildArray[0].first, validChildArray[0].second->name.c_str() };
			if (validChildArray.size() > 0) {
				if (ImGui::BeginCombo("Choose child to add", preview.second.c_str())) {
					for (int c = 0; c < validChildArray.size(); c++) {
						std::string childName{ validChildArray[c].second->name };
						bool is_selected = (preview.second == childName);
						if (ImGui::Selectable(childName.c_str(), is_selected)) {
							preview.first = validChildArray[c].first;
							preview.second = childName;
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			if (ImGui::Button("Add as child")) {
				ECS::ecs().AddComponent<Child>(preview.first, Child{ entity });
				entityParent.children.push_back(preview.first);
			}

			ImGui::TreePop();
		}

	}

	if (ECS::ecs().HasComponent<Model>(entity)) {
		if (ImGui::TreeNodeEx((void*)typeid(Model).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Color")) {
			auto& modelComponent = ECS::ecs().GetComponent<Model>(entity);
			//ImVec4 imColor = ((ImVec4)color.color);
			// note: switch to color edit4 for A value?

			ModelType& modelType{ modelComponent.type };
			std::string modelTypeName{ modelTypeMap.at(modelType)};

			if (ImGui::BeginCombo("Model Type", modelTypeName.c_str())) {
				for (auto& n : modelTypeMap) {
					bool is_selected = (modelTypeName == n.second);
					if (ImGui::Selectable(n.second.c_str(), is_selected)) {
						modelType = n.first;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::ColorEdit4("Edit Color", (float*)&modelComponent.GetColorRef());

			ImGui::TreePop();
		}
	}
	if (ECS::ecs().HasComponent<Transform>(entity)) {
		Transform* entityTransform{ &ECS::ecs().GetComponent<Transform>(entity) };
		if (ECS::ecs().HasComponent<Child>(entity)) {
			entityTransform = &ECS::ecs().GetComponent<Child>(entity).offset;
		}
		if (ImGui::TreeNodeEx((void*)typeid(Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
			auto& positionComponent = entityTransform->position;
			auto& rotationComponent = entityTransform->rotation;
			auto& scaleComponent = entityTransform->scale;
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
	if (ECS::ecs().HasComponent<Size>(entity)) {
		Size* entitySize{ &ECS::ecs().GetComponent<Size>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Size")) {
			auto& widthComponent = entitySize->width;
			auto& heightComponent = entitySize->height;
			ImGui::DragFloat("Width", &widthComponent);
			ImGui::DragFloat("Height", &heightComponent);

			ImGui::TreePop();
		}
	}
	if (ECS::ecs().HasComponent<Collider>(entity)) {
		Collider* entityCollider{ &ECS::ecs().GetComponent<Collider>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(Collider).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Collider")) {
			//auto& positionComponent = entityCollider->position;
			//auto& rotationComponent = entityCollider->rotation;
			//auto& scaleComponent = entityCollider->scale;
			auto& dimensionComponent = entityCollider->dimension;
			//auto& radiusComponent = entityCollider->radius;
			//ImGui::DragFloat2("Position", &positionComponent[0], 0.5f);
			//ImGui::DragFloat("Rotation", &rotationComponent, 0.01f, -(vmath::PI), vmath::PI);
			//ImGui::DragFloat("Scale", &scaleComponent, 0.5f, 1.f, 100.f);
			ImGui::DragFloat2("Dimension", &dimensionComponent[0], 0.5f);
			//ImGui::DragFloat("Radius", &radiusComponent, 0.01f, 0.0f, 100.f, "%.3f");

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

			if (textureComponent.tex != nullptr) {
				ImGui::Text(textureComponent.tex->GetName().c_str());
			}
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

	if (ECS::ecs().HasComponent<TextLabel>(entity)) {
		TextLabel& textlabel{ ECS::ecs().GetComponent<TextLabel>(entity) };
		Size& sizeData{ ECS::ecs().GetComponent<Size>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(TextLabel).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Text Label")) {
			std::pair<std::string, std::string> fontInfo = textlabel.font->GetInfo();

			//note: to consider alt method to check entity has no other UI components?
			if (!ECS::ecs().HasComponent<Button>(entity) && !ECS::ecs().HasComponent<HealthBar>(entity)) {
				// size adjustments
				float& lblHeight = sizeData.height;
				float& lblWidth = sizeData.width;
				float lblDims[2] = { lblHeight, lblWidth };
				if (ImGui::DragFloat2("Label Size", lblDims, 0.5f)) {
					lblDims[0] = std::max(lblDims[0], 0.f);
					lblDims[1] = std::max(lblDims[1], 0.f);
					sizeData.height = lblDims[0];
					sizeData.width = lblDims[1];

					switch (textlabel.textWrap) {
					case(UI_TEXT_WRAP::AUTO_WIDTH):
						if (sizeData.width != textlabel.textWidth) {
							textlabel.textWrap = UI_TEXT_WRAP::AUTO_HEIGHT;
							break;
						}
						else if (sizeData.height != textlabel.textHeight) {
							textlabel.textWrap = UI_TEXT_WRAP::FIXED_SIZE;
						}
						break;
					case(UI_TEXT_WRAP::AUTO_HEIGHT):
						if (sizeData.height != textlabel.textHeight) {
							textlabel.textWrap = UI_TEXT_WRAP::FIXED_SIZE;
						}
						break;
					default:
						textlabel.textWrap = UI_TEXT_WRAP::FIXED_SIZE;
						break;
					}
				}				
			}

			// font properties
			std::vector<std::string> ftFamilyList = fonts.GetFontFamilyList();
			std::vector<std::string> ftVariantList = fonts.GetFontVariantList(fontInfo.first);
			const char* selectedFamily = fontInfo.first.c_str();
			const char* selectedVariant = fontInfo.second.c_str();

			if (!ECS::ecs().HasComponent<Button>(entity)) {
				auto& txtColor = textlabel.GetTextColor();
				ImGui::ColorEdit4("Color", (float*)&txtColor);
				textlabel.textColor = txtColor;
			}

			bool& lblBackground = textlabel.hasBackground;
			ImGui::Checkbox("Has Background", &lblBackground);

			// combo box for font family
			if (!ftFamilyList.empty()) {
				if (ImGui::BeginCombo("Font", selectedFamily)) {
					for (int n = 0; n < ftFamilyList.size(); n++) {
						bool is_selected = (selectedFamily == ftFamilyList[n]);
						if (ImGui::Selectable(ftFamilyList[n].c_str(), is_selected)) {
							selectedFamily = ftFamilyList[n].c_str();
							// trigger set font family
							textlabel.SetFontFamily(selectedFamily);
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			// combo box for font variant
			if (!ftFamilyList.empty()) {
				if (ImGui::BeginCombo("Variant", selectedVariant)) {
					for (int n = 0; n < ftVariantList.size(); n++) {
						bool is_selected = (selectedVariant == ftVariantList[n]);
						if (ImGui::Selectable(ftVariantList[n].c_str(), is_selected)) {
							selectedVariant = ftVariantList[n].c_str();
							// trigger set font variant
							textlabel.SetFontVariant(selectedFamily, selectedVariant);
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			// font size adjustment
			int fontSizeDisplayVal = static_cast<int>(textlabel.relFontSize * 100);
			if (ImGui::DragInt("Size", &fontSizeDisplayVal, 1, 0, 100)) {
				fontSizeDisplayVal = std::clamp(fontSizeDisplayVal, 0, 100);
				textlabel.relFontSize = static_cast<float>(fontSizeDisplayVal) / 100.0f;
			}

			// text box to reflect current string
			if (ImGui::InputTextMultiline("Text", &textlabel.textString)) {
				textlabel.CalculateOffset();
			}

			// alignment grid
			static int selectedIdx = (3 * static_cast<int>(textlabel.vAlignment) + static_cast<int>(textlabel.hAlignment));
			static UI_HORIZONTAL_ALIGNMENT horizontalAlignmentGrid[3][3];
			static UI_VERTICAL_ALIGNMENT verticalAlignmentGrid[3][3];
			const float itemWidthFraction = 0.3f;
			float panelWidth = ImGui::GetContentRegionAvail().x * 0.6f;

			// Calculate the responsive itemWidth
			float itemWidth = panelWidth * itemWidthFraction;

			for (int y = 0; y < 3; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					// Display horizontal and vertical alignment labels centered on the selectable item
					char label[16];
					sprintf_s(label, "%c/%c",
						(y == 0) ? 'T' : (y == 1) ? 'M' : 'B',
						(x == 0) ? 'L' : (x == 1) ? 'M' : 'R');

					bool isSelected = selectedIdx == (3 * y + x);
					if (isSelected)
					{
						horizontalAlignmentGrid[y][x] = static_cast<UI_HORIZONTAL_ALIGNMENT>(x);
						verticalAlignmentGrid[y][x] = static_cast<UI_VERTICAL_ALIGNMENT>(y);
					}

					if (x > 0) ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);  // Adjust for spacing

					if (ImGui::Selectable(label, isSelected, ImGuiSelectableFlags_None, ImVec2(itemWidth, 50.f)))
					{
						selectedIdx = (3 * y + x);
						textlabel.hAlignment = static_cast<UI_HORIZONTAL_ALIGNMENT>(x);
						textlabel.vAlignment = static_cast<UI_VERTICAL_ALIGNMENT>(y);
					}
				}
			}
			ImGui::SameLine();
			ImGui::Text("Alignment");


			// text wrap setting
			int selectedWrapIdx = static_cast<int>(textlabel.textWrap);
			DEBUG_PRINT("selectedWrapIdx: %d, textWrap: %d", selectedWrapIdx, static_cast<int>(textlabel.textWrap));
			static UI_TEXT_WRAP textWrapGrid[3];

			const char* textWrapLabels[] = { "Auto\nWidth", "Auto\nHeight", "Fixed\nSize" };

			for (int i = 0; i < 3; ++i)
			{
				bool textWrapSelected = selectedWrapIdx == i;
				if (textWrapSelected) {
					textWrapGrid[i] = static_cast<UI_TEXT_WRAP>(i);
				}

				if (i > 0) ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);  // Adjust for spacing

				if (ImGui::Selectable(textWrapLabels[i], textWrapSelected, ImGuiSelectableFlags_None, ImVec2(itemWidth, 50.f)))
				{
					selectedWrapIdx = i;
					// If a new button is selected, update the sizeMode accordingly
					textlabel.textWrap = static_cast<UI_TEXT_WRAP>(i);
				}
			}

			ImGui::SameLine();
			ImGui::Text("Text Wrap");

			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<Button>(entity) && ECS::ecs().HasComponent<TextLabel>(entity)) {
		Button& button{ ECS::ecs().GetComponent<Button>(entity) };
		TextLabel& textlabel{ ECS::ecs().GetComponent<TextLabel>(entity) };
		Size& sizeData{ ECS::ecs().GetComponent<Size>(entity) };
		const char* currentEvent{ button.eventName.c_str() };
		if (ImGui::TreeNodeEx((void*)typeid(Button).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Button Event")) {
			std::vector<const char*> functionNames{ events.GetFunctionNames() };
			if (!functionNames.empty()) {
				if (ImGui::BeginCombo("Events Available", currentEvent)) {
					for (int n = 0; n < functionNames.size(); n++) {
						bool is_selected = (currentEvent == functionNames[n]);
						if (ImGui::Selectable(functionNames[n], is_selected)) {
							button.eventName = functionNames[n];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			if (button.eventName == "Play Sound") {
				if (ImGui::BeginCombo("Event Input", button.eventInput.c_str())) {
					std::vector<std::string> soundNames{ assetmanager.audio.GetSoundPaths() };
					for (int n = 0; n < soundNames.size(); n++) {
						bool is_selected = (button.eventInput == soundNames[n]);
						if (ImGui::Selectable(soundNames[n].c_str(), is_selected)) {
							button.eventInput = soundNames[n];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			else if (button.eventName == "Play Music") {
				if (ImGui::BeginCombo("Event Input", button.eventInput.c_str())) {
					std::vector<std::string> soundNames{ assetmanager.audio.GetMusicPaths() };
					for (int n = 0; n < soundNames.size(); n++) {
						bool is_selected = (button.eventInput == soundNames[n]);
						if (ImGui::Selectable(soundNames[n].c_str(), is_selected)) {
							button.eventInput = soundNames[n];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			else {
				ImGui::InputText("Event Input", &button.eventInput);
			}

			// size adjustments
			float& btnHeight = sizeData.height;
			float& btnWidth = sizeData.width;
			float btnDims[2] = { btnHeight, btnWidth };
			ImGui::DragFloat2("Button Size", btnDims, 0.5f);
				btnDims[0] = std::max(btnDims[0], 0.f);
				btnDims[1] = std::max(btnDims[1], 0.f);
				sizeData.height = btnDims[0];
				sizeData.width = btnDims[1];			
			

			// padding adjustments
			float& btnPadTop = button.padding.top;
			float& btnPadBtm = button.padding.bottom;
			float& btnPadLeft = button.padding.left;
			float& btnPadRight = button.padding.right;
			int& btnPadSetting = button.padding.setting;

			const char* paddingOptions[] = { "Uniform", "Balanced (Vertical, Horizontal)", "Custom (Top, Bottom, Left, Right)" };
			static int item_current = 0;
			ImGui::Combo("Padding Setting", &btnPadSetting, paddingOptions, IM_ARRAYSIZE(paddingOptions));

			float btnBalancedPad[2] = { btnPadTop, btnPadLeft };
			float btnCustomPad[4] = { btnPadTop, btnPadBtm, btnPadLeft, btnPadRight };

			//note: consider drawing padding to reflect the change?
			switch (btnPadSetting) {
			case(0):
				// uniform, show 1 value
				if (ImGui::DragFloat("Padding Value", &btnPadLeft, 0.5f)) {
					btnPadLeft = std::clamp(btnPadLeft, 0.f, 0.5f * textlabel.textWidth);
					btnPadTop = btnPadLeft;
					btnPadBtm = btnPadLeft;
					btnPadRight = btnPadLeft;
				}
				break;
			case(1):
				// balanced, show 2 values
				if (ImGui::DragFloat2("Padding Value", btnBalancedPad, 0.5f)) {
					btnBalancedPad[0] = std::clamp(btnBalancedPad[0], 0.f, 0.5f * textlabel.textWidth);
					btnBalancedPad[1] = std::clamp(btnBalancedPad[1], 0.f, 0.5f * textlabel.textWidth);

					button.padding.top = btnBalancedPad[0];
					button.padding.bottom = btnBalancedPad[0];
					button.padding.left = btnBalancedPad[1];
					button.padding.right = btnBalancedPad[1];
				}
				break;
			default:
				// custom, show 4 values
				if (ImGui::DragFloat4("Padding Value", btnCustomPad, 0.5f)) {
					btnCustomPad[0] = std::clamp(btnCustomPad[0], 0.f, 0.5f * textlabel.textWidth);
					btnCustomPad[1] = std::clamp(btnCustomPad[1], 0.f, 0.5f * textlabel.textWidth);
					btnCustomPad[2] = std::clamp(btnCustomPad[2], 0.f, 0.5f * textlabel.textWidth);
					btnCustomPad[3] = std::clamp(btnCustomPad[3], 0.f, 0.5f * textlabel.textWidth);

					button.padding.top = btnCustomPad[0];
					button.padding.bottom = btnCustomPad[1];
					button.padding.left = btnCustomPad[2];
					button.padding.right = btnCustomPad[3];
				}
				break;
			}

			// color properties
			int& btnColorSetting = button.colorSetting;
			auto btnColor = button.GetDefaultButtonColor();
			auto txtColor = button.GetDefaultTextColor();

			const char* btnColorSetOptions[] = { "Default", "Hovered", "Focused" };
			ImGui::Combo("Edit Color Set", &btnColorSetting, btnColorSetOptions, IM_ARRAYSIZE(btnColorSetOptions));

			switch (btnColorSetting) {
			case(1):
				// edit hovered color set
				btnColor = button.hoveredColor.buttonColor;
				txtColor = button.hoveredColor.textColor;
				if (ImGui::ColorEdit4("Button Color", (float*)&btnColor)
					|| ImGui::ColorEdit4("Text Color", (float*)&txtColor)) {
					button.UpdateColorSets(STATE::HOVERED, btnColor, txtColor);
				}
				break;
			case(2):
				// edit focused color set
				btnColor = button.focusedColor.buttonColor;
				txtColor = button.focusedColor.textColor;
				if (ImGui::ColorEdit4("Button Color", (float*)&btnColor)
					|| ImGui::ColorEdit4("Text Color", (float*)&txtColor)) {
					button.UpdateColorSets(STATE::FOCUSED, btnColor, txtColor);
				}
				break;
			default:
				// edit default color set
				btnColor = button.defaultColor.buttonColor;
				txtColor = button.defaultColor.textColor;
				if (ImGui::ColorEdit4("Button Color", (float*)&btnColor)
					|| ImGui::ColorEdit4("Text Color", (float*)&txtColor)) {
					button.UpdateColorSets(STATE::NONE, btnColor, txtColor);
				}
				break;
			}

			ImGui::TreePop();

		}
	}

	if (ECS::ecs().HasComponent<HealthBar>(entity)) {
		Size& sizeData{ ECS::ecs().GetComponent<Size>(entity) };
		HealthBar& hpBar{ ECS::ecs().GetComponent<HealthBar>(entity) };
		CharacterStats& charaStatsData{ *hpBar.charaStatsRef };

		if (ImGui::TreeNodeEx((void*)typeid(HealthBar).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Health Bar")) {
			float currentHp = hpBar.currentHealth;
			float maxHp = hpBar.maxHealth;
			float hpPct = hpBar.healthPct;

			// size adjustments
			float& barHeight = sizeData.height;
			float& barWidth = sizeData.width;
			float barDims[2] = { barHeight, barWidth };
			ImGui::DragFloat2("HP Bar Size", barDims, 0.5f);
			barDims[0] = std::max(barDims[0], 0.f);
			barDims[1] = std::max(barDims[1], 0.f);
			sizeData.height = barDims[0];
			sizeData.width = barDims[1];

			//slider for HP percentage
			if (ImGui::DragFloat("HP Percentage", &hpPct, 0.5f)) {
				hpPct = std::clamp(hpPct, 0.f, 100.f);
				hpBar.healthPct = hpPct;
				hpBar.currentHealth = hpPct / 100.f * maxHp;
				if (&charaStatsData) {
					charaStatsData.stats.health = hpPct / 100.f * charaStatsData.stats.maxHealth;
				}
			}
			ImGui::Text("%.2f/%.2f (%.2f%%)", currentHp, maxHp, hpPct);

			// display settings
			bool& hpShowHealth = hpBar.showHealthStat;
			ImGui::Checkbox("Show Health", &hpShowHealth);
			int hpShowValOrPct = (int)hpBar.showValOrPct;
			ImGui::RadioButton("Show Value", &hpShowValOrPct, 0); ImGui::SameLine();
			ImGui::RadioButton("Show HP percentage", &hpShowValOrPct, 1);
			hpBar.showValOrPct = (bool)hpShowValOrPct;
			
			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<SkillPointHUD>(entity)) {
		SkillPointHUD& spHUD{ ECS::ecs().GetComponent<SkillPointHUD>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(SkillPointHUD).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "SkillPointHUD")) {
			// input setting for skillPointBalance
			ImGui::InputInt("Skill Point Balance", &spHUD.skillPointBalance);
			spHUD.skillPointBalance = std::clamp(spHUD.skillPointBalance, 0, spHUD.maxSkillPoints);

			ImGui::TreePop();
		}		
	}

	if (ECS::ecs().HasComponent<SkillPoint>(entity)) {
		SkillPoint& sp{ ECS::ecs().GetComponent<SkillPoint>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(SkillPoint).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "SkillPoint")) {
			// display active state
			sp.isActive ? ImGui::Text("State: Active") : ImGui::Text("State: Not Active");

			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<AttackSkill>(entity)) {
		AttackSkill& atkSkill{ ECS::ecs().GetComponent<AttackSkill>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(AttackSkill).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "AttackSkill")) {
			// skill index selector
			int& setIndex = atkSkill.skillIndex;
			const char* indexOptions[] = { "1", "2", "3" };
			if (ImGui::BeginCombo("Select Skill", indexOptions[setIndex], 0)) {
				for (int i = 0; i < IM_ARRAYSIZE(indexOptions); ++i) {
					bool isSelected = (setIndex == i);
					if (ImGui::Selectable(indexOptions[i], isSelected)) {
						setIndex = i;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			atkSkill.skillIndex = setIndex;

			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<AllyHUD>(entity)) {
		AllyHUD& allyHud{ ECS::ecs().GetComponent<AllyHUD>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(AllyHUD).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "AllyHUD")) {
			// ally index selector (validity checks handled in system)
			int& setIndex = allyHud.allyIndex;
			ImGui::DragInt("Ally Index", &setIndex, 1);
			allyHud.allyIndex = setIndex;
			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<EnemyHUD>(entity)) {
		EnemyHUD& enemyHud{ ECS::ecs().GetComponent<EnemyHUD>(entity) };
		if (ImGui::TreeNodeEx((void*)typeid(EnemyHUD).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "EnemyHUD")) {
			// enemy index selector (validity checks handled in system)
			int& setIndex = enemyHud.enemyIndex;
			ImGui::DragInt("Enemy Index", &setIndex, 1);
			enemyHud.enemyIndex = setIndex;
			ImGui::TreePop();
		}
	}

	// if (ECS::ecs().HasComponent<Script>(entity)) {

	// 	// If master entity is selected, do not allow editing of scripts
	// 	if (ECS::ecs().HasComponent<Master>(entity)) {
	// 		return;
	// 	}
	if (ECS::ecs().HasComponent<Script>(entity) && !ECS::ecs().HasComponent<Master>(entity)) {
		




		if (ImGui::TreeNodeEx((void*)typeid(Script).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Scripts")) {
			if (!fullNameVecImGUI.empty()) {
		
				if (ImGui::BeginCombo("Scripts Available", currentScriptForIMGUI.c_str())) {
					for (int n = 0; n < fullNameVecImGUI.size(); n++) {
						bool is_selected = (currentScriptForIMGUI == fullNameVecImGUI[n]);
						if (ImGui::Selectable(fullNameVecImGUI[n].c_str(), is_selected)) {
							currentScriptForIMGUI = fullNameVecImGUI[n];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

			if (ImGui::Button("Add Script")) {
				if (currentScriptForIMGUI.empty()) {
					DEBUG_PRINT("No script selected");
				}
				else {
					ScriptEngine::AttachScriptToEntity(entity, currentScriptForIMGUI);
					currentScriptForIMGUI = "";
				}
			}

			// For every new script in the vector, draw the tree
			for (int i = 0; i < scriptNamesAttachedforIMGUI[entity].size(); /* no increment here */) {
				const std::string& scriptNaming = scriptNamesAttachedforIMGUI[entity][i];

				// Create a unique identifier for each button to avoid conflicts
				std::string buttonId = "Delete Script##" + std::to_string(i);
				if (ImGui::SmallButton(buttonId.c_str())) {
					ScriptEngine::RemoveScriptFromEntity(entity, scriptNaming);

					// Do not increment i, as the next element has shifted into the current position
					continue;
				}

				ImGui::SameLine();
				DrawScriptTreeWithImGui(scriptNaming, entity, i);

				// Increment only if an element was not removed
				++i;
			}


			// This part is for the scripts that are already attached to the entity (delete the script)
			//if (ImGui::BeginCombo("Scripts Attached", currentScriptAttachedForIMGUI.c_str())) {
			//	for (int n = 0; n < scriptNamesAttachedforIMGUI[entity].size(); n++) {
			//		bool is_selected = (currentScriptAttachedForIMGUI == scriptNamesAttachedforIMGUI[entity][n]);
			//		if (ImGui::Selectable(scriptNamesAttachedforIMGUI[entity][n].c_str(), is_selected)) {
			//			currentScriptAttachedForIMGUI = scriptNamesAttachedforIMGUI[entity][n];
			//		}
			//		if (is_selected) {
			//			ImGui::SetItemDefaultFocus();
			//		}
			//	}
			//	ImGui::EndCombo();
			//}

			//ImGui::SameLine();
			//if (ImGui::Button("Delete Script")) {
			//	if (currentScriptAttachedForIMGUI.empty()) {
			//		DEBUG_PRINT("No script selected");
			//	}
			//	else {
			//		ScriptEngine::RemoveScriptFromEntity(entity, currentScriptAttachedForIMGUI);
			//		currentScriptAttachedForIMGUI = "";
			//	}
			//}

			ImGui::TreePop();
			}
		}
		

	}

	if (ECS::ecs().HasComponent<CharacterStats>(entity)) {
		if (ImGui::TreeNodeEx((void*)typeid(CharacterStats).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Character Stats")) {
			auto& charstatsComponent = ECS::ecs().GetComponent<CharacterStats>(entity);
			std::unordered_map<const char*, CharacterType> charTypeMap{};
			charTypeMap.emplace("Player", CharacterType::PLAYER);
			charTypeMap.emplace("Enemy", CharacterType::ENEMY);
			std::vector<const char*> charTypeNames{};
			std::string currentType{};
			for (auto& c : charTypeMap) {
				if (c.second == charstatsComponent.tag) {
					currentType = c.first;
				}
				charTypeNames.push_back(c.first);
			}
			if (!charTypeNames.empty()) {
				if (ImGui::BeginCombo("Types Available", currentType.c_str())) {
					for (int n = 0; n < charTypeNames.size(); n++) {
						bool is_selected = (currentType.c_str() == charTypeNames[n]);
						if (ImGui::Selectable(charTypeNames[n], is_selected)) {
							charstatsComponent.tag = charTypeMap[charTypeNames[n]];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			ImGui::InputFloat("Max Health", &charstatsComponent.stats.maxHealth);
			ImGui::InputFloat("Health", &charstatsComponent.stats.health);
			ImGui::InputFloat("Attack", &charstatsComponent.stats.attack);
			ImGui::InputFloat("Defense", &charstatsComponent.stats.defense);
			ImGui::InputInt("Speed", &charstatsComponent.stats.speed);

			std::vector<std::string> attackNames{ assetmanager.attacks.GetAttackNames() };
			for (std::string& s : attackNames) {
				s = s.substr(0, s.find(".skill"));
			}
			for (size_t a = 0; a < charstatsComponent.action.skills.size(); a++) {
				std::string currentAttack{ charstatsComponent.action.skills[a].attackName };
				std::stringstream header{};
				header << "Skill " << a + 1;
				if (ImGui::BeginCombo(header.str().c_str(), currentAttack.c_str())) {
					for (int n = 0; n < attackNames.size(); n++) {
						if (attackNames[n] == "") {
							continue;
						}
						bool is_selected = (currentAttack == attackNames[n]);
						if (ImGui::Selectable(attackNames[n].c_str(), is_selected)) {
							charstatsComponent.action.skills[a] = assetmanager.attacks.data[attackNames[n]];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			std::string currentAttack{ "None"};
			if (ImGui::BeginCombo("New Skill", currentAttack.c_str())) {
				for (int n = 0; n < attackNames.size(); n++) {
					if (attackNames[n] == "") {
						continue;
					}
					bool is_selected = (currentAttack == attackNames[n]);
					if (ImGui::Selectable(attackNames[n].c_str(), is_selected)) {
						charstatsComponent.action.skills.push_back(assetmanager.attacks.data[attackNames[n]]);
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}
	}
}

// Macro for easier use
#define DRAW_FIELD(TYPE, IMGUI_FUNCTION) \
    TYPE data = scriptInstance->GetFieldValue<TYPE>(name); \
    if (IMGUI_FUNCTION(name.c_str(), &data)) { \
        scriptInstance->SetFieldValue(name, data); \
    }

void DrawScriptTreeWithImGui(const std::string& className, Entity entity, int i) 
{
	if (ImGui::TreeNodeEx(className.c_str())) 
	{
		auto scriptInstance = ScriptEngine::GetEntityScriptInstance(entity, i);
		if (scriptInstance) 
		{
			const auto& fields = scriptInstance->GetScriptClass()->GetFields();
			for (const auto& [name, field] : fields) 
			{
				switch (field.Type)
				{
				case ScriptFieldType::Float:
				{
					DRAW_FIELD(float, ImGui::DragFloat)
						break;
				}
				case ScriptFieldType::Int:
				{
					DRAW_FIELD(int, ImGui::DragInt)
						break;
				}
				case ScriptFieldType::Bool:
				{
					DRAW_FIELD(bool, ImGui::Checkbox)
						break;
				}
				case ScriptFieldType::Vector2:
				{
					vmath::Vector2 dataVec2 = scriptInstance->GetFieldValue<vmath::Vector2>(name);
					if (ImGui::DragFloat2(name.c_str(), &dataVec2[0], 0.5f)) 
					{
						scriptInstance->SetFieldValue(name, dataVec2);
					}
					break;
				}
				case ScriptFieldType::Vector3: {
					vmath::Vector3 dataVec3 = scriptInstance->GetFieldValue<vmath::Vector3>(name);
					float tempArray[3] = { dataVec3.x, dataVec3.y, dataVec3.z };

					if (ImGui::DragFloat3(name.c_str(), tempArray, 0.5f)) {
						dataVec3.x = tempArray[0];
						dataVec3.y = tempArray[1];
						dataVec3.z = tempArray[2];
						scriptInstance->SetFieldValue(name, dataVec3);
					}
					break;
				}
				}
			}
		}
		ImGui::TreePop();
	}
}
#undef DRAW_FIELD

