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

	auto& cloneArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Clone>() };
	auto& typeManager{ ECS::ecs().GetTypeManager() };
	auto cloneIDArray{ cloneArray.GetEntityArray() };
	
	//Real-time prefab updating, very unoptimised
	for (auto& cloneEntity : cloneIDArray) {
		Clone clone{ cloneArray.GetData(cloneEntity) };
		if (clone.prefab == prefabName) {
			for (auto& ecsType : typeManager) {
				if (ecsType.second->HasComponent(prefabID) && !(bool)(clone.unique_components.count(ecsType.second->name))) {
					ecsType.second->CopyComponent(cloneEntity, prefabID);
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
	if (ECS::ecs().HasComponent<Clone>(entity)) {
		auto& entityClone{ ECS::ecs().GetComponent<Clone>(entity) };
		if (entityClone.prefab == "") {
			ImGui::Text("Entity has no prefabs");
			if (ImGui::Button("Save as prefab")) {
				std::string prefabPath{ SaveFileDialog("*.prefab","Prefab") };
				std::set<Entity> entityToSave{ entity };
				if (prefabPath != "") {
					Serializer::SaveEntityToJson(prefabPath, entityToSave);
					std::string prefabName{ prefabPath.substr(prefabPath.find_last_of("\\") + 1) };
					ECS::ecs().GetComponent<Clone>(entity).prefab = prefabName;
				}
				assetmanager.UpdatePrefabPaths();
			}
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
		if (ImGui::TreeNodeEx((void*)typeid(TextLabel).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Text Label")) {
			std::pair<std::string, std::string> fontInfo = textlabel.font->GetInfo();

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


			// text box to reflect current string
			ImGui::InputText("Text", &textlabel.textString);

			ImGui::TreePop();
		}
	}

	if (ECS::ecs().HasComponent<Button>(entity)) {
		Button& button{ ECS::ecs().GetComponent<Button>(entity) };
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

			auto& btnColor = button.GetDefaultButtonColor();
			ImGui::ColorEdit3("Color", (float*)&btnColor);
			//if (button.currentState == STATE::FOCUSED) {
				//button.UpdateColorSets(btnColor, button.GetDefaultTextColor());
			//}

			ImGui::TreePop();

		}
	}

	if (ECS::ecs().HasComponent<Script>(entity)) {

		// If master entity is selected, do not allow editing of scripts
		if (ECS::ecs().HasComponent<Master>(entity)) {
			return;
		}
		
		// For everything in the vector, draw the tree
		for (auto& scriptNaming : scriptNamesAttachedforIMGUI[entity]) {
			DrawScriptTreeWithImGui(scriptNaming, entity);
		}

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

			// This part is for the scripts that are already attached to the entity
			if (ImGui::BeginCombo("Scripts Attached", currentScriptAttachedForIMGUI.c_str())) {
				for (int n = 0; n < scriptNamesAttachedforIMGUI[entity].size(); n++) {
					bool is_selected = (currentScriptAttachedForIMGUI == scriptNamesAttachedforIMGUI[entity][n]);
					if (ImGui::Selectable(scriptNamesAttachedforIMGUI[entity][n].c_str(), is_selected)) {
						currentScriptAttachedForIMGUI = scriptNamesAttachedforIMGUI[entity][n];
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			//ImGui::SameLine();
			if (ImGui::Button("Delete Script")) {
				if (currentScriptAttachedForIMGUI.empty()) {
					DEBUG_PRINT("No script selected");
				}
				else {
					ScriptEngine::RemoveScriptFromEntity(entity, currentScriptAttachedForIMGUI);
					currentScriptAttachedForIMGUI = "";
				}
			}

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
