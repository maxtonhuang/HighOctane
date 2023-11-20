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

/*----------For the scripting, may move somewhere else in the future---------*/
struct EntityFieldKey {
	Entity entity;
	std::string fieldName;

	EntityFieldKey(Entity e, const std::string& fName) : entity(e), fieldName(fName) {}

	bool operator==(const EntityFieldKey& other) const {
		return entity == other.entity && fieldName == other.fieldName;
	}
};

template <>
struct std::hash<EntityFieldKey> {
	std::size_t operator()(const EntityFieldKey& key) const {

		// Compute individual hash values for entity and fieldName
		// and combine them using a bitwise operation (e.g., XOR)
		std::size_t h1 = std::hash<Entity>()(key.entity);
		std::size_t h2 = std::hash<std::string>()(key.fieldName);
		return h1 ^ (h2 << 1);
	}
};
/*----------For the scripting, may move somewhere else in the future---------*/

/*--------Variables for scripting--------*/
using FieldValue = std::variant<int, float, bool>;  // Define a variant type to hold int, float, and bool

std::unordered_map<EntityFieldKey, FieldValue> fieldValues;  // Map to hold the current values of each field

Entity currentSelectedEntity{};

extern std::vector<std::string> fullNameVecImGUI;
/*--------Variables for scripting--------*/

static bool check;

//FOR PREFAB HIERACHY
std::string prefabName{};

void DrawScriptTreeWithImGui(const std::string& className, Entity entity) {
	ScriptEngineData* scriptData = ScriptEngine::GetInstance();

	if (ImGui::TreeNodeEx(className.c_str())) {
		for (auto& classEntry : scriptData->ScriptInfoVec) {
			//std::cout << scriptData->ScriptInfoVec.size() << std::endl;
			//std::cout << classEntry.typeName << std::endl;
			if (classEntry.className != className) {
				continue;
			}

			if (classEntry.fieldType != MONO_FIELD_ATTR_PUBLIC) {
				continue;
			}

			std::string fieldInfo = classEntry.variableName;

			// Initialize the field value in the map if it doesn't exist
			if (fieldValues.find(EntityFieldKey(entity, fieldInfo)) == fieldValues.end()) {
				switch (classEntry.typeName) {
				case MONO_TYPE_I4: fieldValues[EntityFieldKey(entity, fieldInfo)] = 0; break;
				case MONO_TYPE_R4: fieldValues[EntityFieldKey(entity, fieldInfo)] = 0.0f; break;
				case MONO_TYPE_BOOLEAN: fieldValues[EntityFieldKey(entity, fieldInfo)] = false; break;
				}
			}

			FieldValue& currentValue = fieldValues[EntityFieldKey(entity, fieldInfo)];


			switch (classEntry.typeName) {
			case MONO_TYPE_I4: // int
				ImGui::DragInt(fieldInfo.c_str(), &std::get<int>(currentValue), 1.0f);

				break;

			case MONO_TYPE_R4: // float
				ImGui::DragFloat(fieldInfo.c_str(), &std::get<float>(currentValue), 0.1f);
				break;

			case MONO_TYPE_BOOLEAN: // bool
				ImGui::Checkbox(fieldInfo.c_str(), &std::get<bool>(currentValue));
				break;
			}

			// Update the script property
			ScriptEngine::SetScriptProperty(entity, classEntry.className, fieldInfo, &currentValue);
		}
		ImGui::TreePop();
	}
}

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
	//if (currentSelectedEntity && selectedEntities.size() == 1) {
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

void UpdatePrefabHierachy() {
	ImGui::Begin("Prefab Editor");

	auto prefabList{ assetmanager.GetPrefabPaths() };
	Entity prefabID{ assetmanager.GetPrefab(prefabName) };

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

	if (prefabID) {
		if (ImGui::Button("Save Prefab")) {
			std::string prefabPath{ assetmanager.GetDefaultPath() + "Prefabs/" + prefabName};
			std::set<Entity> entityToSave{ prefabID };
			Serializer::SaveEntityToJson(prefabPath, entityToSave);
		}

		if (ImGui::Button("Create Instance")) {
			Entity clone = EntityFactory::entityFactory().CloneMaster(prefabID);
			ECS::ecs().GetComponent<Clone>(clone).prefab = prefabName;
		}

		SceneEntityComponents(prefabID);
		ImGui::Separator();
		ComponentBrowser(prefabID);
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
	if (ECS::ecs().HasComponent<Clone>(entity)) {
		auto& entityClone{ ECS::ecs().GetComponent<Clone>(entity) };
		if (entityClone.prefab == "") {
			ImGui::Text("Entity has no prefabs");
		}
		else {
			std::string text{ "Prefab: " + entityClone.prefab };
			ImGui::Text(text.c_str());
			ImGui::SameLine();
			if (ImGui::Button("Select in Prefab Editor")) {
				prefabName = entityClone.prefab;
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
	if (ECS::ecs().HasComponent<Model>(entity)) {
		if (ImGui::TreeNodeEx((void*)typeid(Model).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Color")) {
			auto& colorComponent = ECS::ecs().GetComponent<Model>(entity).GetColorRef();
			//ImVec4 imColor = ((ImVec4)color.color);
			// note: switch to color edit4 for A value?
			ImGui::ColorEdit3("Edit Color", (float*)&colorComponent);

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
				ImGui::DragFloat2("Label Size", lblDims, 0.5f);
				lblDims[0] = std::max(lblDims[0], 0.f);
				lblDims[1] = std::max(lblDims[1], 0.f);
				sizeData.height = lblDims[0];
				sizeData.width = lblDims[1];
			}

			// font properties
			std::vector<std::string> ftFamilyList = fonts.GetFontFamilyList();
			std::vector<std::string> ftVariantList = fonts.GetFontVariantList(fontInfo.first);
			const char* selectedFamily = fontInfo.first.c_str();
			const char* selectedVariant = fontInfo.second.c_str();

			if (ECS::ecs().HasComponent<Button>(entity)) {
				Button& button{ ECS::ecs().GetComponent<Button>(entity) };

				auto& txtColor = button.GetDefaultTextColor();
				ImGui::ColorEdit3("Color", (float*)&txtColor);

				//button.UpdateColorSets(button.GetDefaultButtonColor(), txtColor);
				textlabel.textColor = txtColor;
			}
			else {
				auto& txtColor = textlabel.GetTextColor();
				ImGui::ColorEdit3("Color", (float*)&txtColor);
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
			if (ImGui::InputText("Text", &textlabel.textString)) {
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

			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<Button>(entity)) {
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
			auto& btnColor = button.GetDefaultButtonColor();
			ImGui::ColorEdit3("Color", (float*)&btnColor);

			ImGui::TreePop();

		}
	}

	if (ECS::ecs().HasComponent<HealthBar>(entity)) {
		Size& sizeData{ ECS::ecs().GetComponent<Size>(entity) };
		HealthBar& hpBar{ ECS::ecs().GetComponent<HealthBar>(entity) };

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

			//TODO: change max HP only?
			if (ImGui::DragFloat("Current HP", &currentHp, 0.5f)) {
				hpBar.SetCurrentHealth(currentHp);
			}
			hpBar.SetMaxHealth(maxHp);

			ImGui::Text("%.2f/%.2f (%.2f%%)", currentHp, maxHp, hpPct);
			ImGui::SameLine(260); //to seek alternatives
			ImGui::Text("HP Percentage");

			ImGui::Text("%.2f, %.2f", hpBar.barWidth, hpBar.barHeight);
			ImGui::SameLine(260); //to seek alternatives
			ImGui::Text("Current HP dimensions");

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
			if (ImGui::BeginCombo("Points Balance", std::to_string(spHUD.skillPointBalance).c_str())) {
				std::vector<std::string> skillPointItems;
				for (int i = 0; i <= spHUD.maxSkillPoints; ++i) {
					skillPointItems.push_back(std::to_string(i));
				}
				for (int n = 0; n < skillPointItems.size(); n++) {
					bool is_selected = (spHUD.skillPointBalance == std::stoi(skillPointItems[n]));
					if (ImGui::Selectable(skillPointItems[n].c_str(), is_selected)) {
						spHUD.skillPointBalance = std::stoi(skillPointItems[n]);
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

	if (ECS::ecs().HasComponent<Script>(entity)) {

		// If master entity is selected, do not allow editing of scripts
		if (ECS::ecs().HasComponent<Master>(entity)) {
			return;
		}
		
		if (ImGui::TreeNodeEx((void*)typeid(Script).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Scripts")) {
			if (!fullNameVecImGUI.empty()) {
				
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

			if (ImGui::Button("Add Script")) {
				if (currentScriptForIMGUI == NULL) {
					DEBUG_PRINT("No script selected");
				}
				else {
					ScriptEngine::RunTimeAddScript(entity, currentScriptForIMGUI);
					currentScriptForIMGUI = NULL;
					
				}
			}

			// This part is for the scripts that are already attached to the entity
			if (ImGui::BeginCombo("Scripts Attached", currentScriptAttachedForIMGUI.c_str())) {
				for (int n = 0; n < scriptNamesAttachedforIMGUI[entity].size(); n++) {
					bool is_selected = (currentScriptAttachedForIMGUI.c_str() == scriptNamesAttachedforIMGUI[entity][n]);
					if (ImGui::Selectable(scriptNamesAttachedforIMGUI[entity][n].c_str(), is_selected)) {
						currentScriptAttachedForIMGUI = scriptNamesAttachedforIMGUI[entity][n].c_str();
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			//ImGui::SameLine();
			if (ImGui::Button("Delete Script")) {
				if (currentScriptAttachedForIMGUI == "") {
					DEBUG_PRINT("No script selected");
				}
				else {
					ScriptEngine::RunTimeRemoveScript(entity, currentScriptAttachedForIMGUI.c_str());
					currentScriptAttachedForIMGUI = "";
				}
			}

			ImGui::TreePop();
			}
		}
		
		// For everything in the vector, draw the tree
		for (auto& scriptNaming : scriptNamesAttachedforIMGUI[entity]) {
			DrawScriptTreeWithImGui(scriptNaming, entity);
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
