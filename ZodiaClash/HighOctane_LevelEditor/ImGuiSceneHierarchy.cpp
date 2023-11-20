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


Entity currentSelectedPrefab;

/*--------Variables for scripting--------*/

Entity currentSelectedEntity{};

extern std::vector<std::string> fullNameVecImGUI;
/*--------Variables for scripting--------*/

static bool check;

//FOR PREFAB HIERACHY
std::string prefabName{};

// Function to draw a new tree for the script
void DrawScriptTreeWithImGui(const std::string& className, Entity entity, int i) {
	if (ImGui::TreeNodeEx(className.c_str())) {
		auto scriptInstance = ScriptEngine::GetEntityScriptInstance(entity, i);
		if (scriptInstance) {
			const auto& fields = scriptInstance->GetScriptClass()->GetFields();
			for (const auto& [name, field] : fields) {
				switch (field.Type) {
				case ScriptFieldType::Float:
					float data = scriptInstance->GetFieldValue<float>(name);
					if (ImGui::DragFloat(name.c_str(), &data)) {
						scriptInstance->SetFieldValue(name, data);
					}
					break;
				}
			}
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

	if (currentSelectedPrefab) {
		if (ImGui::Button("Save Prefab")) {
			std::string prefabPath{ assetmanager.GetDefaultPath() + "Prefabs/" + prefabName};
			SaveAsPrefab(prefabPath, currentSelectedPrefab);
		}

		if (ImGui::Button("Create Instance")) {
			Entity clone = EntityFactory::entityFactory().CloneMaster(currentSelectedPrefab);
			ECS::ecs().GetComponent<Clone>(clone).prefab = prefabName;
		}

		SceneEntityComponents(currentSelectedPrefab);
		ImGui::Separator();
		ComponentBrowser(currentSelectedPrefab);
	}

	auto& cloneArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Clone>() };
	auto& typeManager{ ECS::ecs().GetTypeManager() };
	auto cloneIDArray{ cloneArray.GetEntityArray() };
	
	//Real-time prefab updating, very unoptimised
	if (edit_mode) {
		for (auto& cloneEntity : cloneIDArray) {
			Clone clone{ cloneArray.GetData(cloneEntity) };
			if (clone.prefab == prefabName) {
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
			auto& colorComponent = ECS::ecs().GetComponent<Model>(entity).GetColorRef();
			//ImVec4 imColor = ((ImVec4)color.color);
			ImGui::ColorEdit3("Edit Color", (float*)&colorComponent);

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

	if (ECS::ecs().HasComponent<Script>(entity) && !ECS::ecs().HasComponent<Master>(entity)) {
		
		// For everything in the vector, draw the tree
		int indexForScript {0 };
		for (auto& scriptNaming : scriptNamesAttachedforIMGUI[entity]) {

			DrawScriptTreeWithImGui(scriptNaming, entity, indexForScript);

			// Increment each time a script is added
			indexForScript++;
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
