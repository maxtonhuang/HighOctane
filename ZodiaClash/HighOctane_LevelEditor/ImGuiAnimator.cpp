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
*	@file		ImGuiAnimator.cpp
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		11 November 2023
*
* *****************************************************************************
*
*	@brief		Animation editor window
*
*   Contains function for the game animation window
*
******************************************************************************/

#include "ImGuiAnimator.h"
#include "ImGuiSceneHierarchy.h"
#include "Animation.h"
#include "AssetManager.h"
#include "Global.h"

//Updates animation window for the input entity
void AnimatorWindow(Entity entity) {
	const ImVec2 buttonsize{ 15,15 };
	const ImVec4 selectedCol{ 0.f,1.f,0.f,1.f };
	const ImVec4 containsCol{ 1.f,1.f,0.f,1.f };
	const ImVec4 playingCol{ 1.f,0.f,0.f,1.f };

	const std::vector<const char*> animTypeNames{ "Sprite","TextureChange","Sound","Fade","Color","TransformAttach","TransformDirect",
		"CameraZoom","CameraTarget","CameraReset","CreatePrefab", "DamageImpact", "Event", "Swap", "SelfDestruct"};

	static std::string selectedType{};
	static std::string selectedAnim{};
	static int selectedFrame{ -1 };
	static std::shared_ptr<Animation> selectedAnimation{};
	static AnimationGroup* selectedAnimGroup{};
	static Entity previousEntity;

	if (ECS::ecs().HasComponent<AnimationSet>(entity)) {
		AnimationSet& animationSet = ECS::ecs().GetComponent<AnimationSet>(entity);

		//Reset for new entity
		if (previousEntity != entity) {
			previousEntity = entity;
			selectedType.clear();
			selectedAnim.clear();
			selectedFrame = -1;
			selectedAnimation = nullptr;
			selectedAnimGroup = nullptr;
		}

		//Animation preview buttons
		if (selectedAnimGroup != nullptr) {
			if (ImGui::Button("Play")) {
				animationSet.Start(selectedAnimGroup->name, entity);
			}
			ImGui::SameLine();
			if (animationSet.paused) {
				if (ImGui::Button("Resume")) {
					animationSet.paused = false;
				}
			}
			else {
				if (ImGui::Button("Pause")) {
					animationSet.paused = true;
				}
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Advance Frame")) {
				if (selectedAnimGroup->active == false) {
					selectedAnimGroup->Start(entity);
				}
				else {
					selectedAnimGroup->Update(entity);
				}
			}
		}

		//Initialize selected animation
		if (selectedAnim == "" && animationSet.animationSet.size() > 0) {
			selectedAnim = animationSet.animationSet[0].name;
		}

		//Dropdown for available animations
		if (ImGui::BeginCombo("Animations Available", selectedAnim.c_str())) {
			for (int n = 0; n < animationSet.animationSet.size(); n++) {
				bool is_selected = (selectedAnim == animationSet.animationSet[n].name);
				if (ImGui::Selectable(animationSet.animationSet[n].name.c_str(), is_selected)) {
					selectedAnimGroup = &animationSet.animationSet[n];
					selectedAnim = selectedAnimGroup->name;
					selectedFrame = -1;
					selectedType.clear();
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			bool is_selected = (selectedAnim == "New Animation");
			if (ImGui::Selectable("New Animation", is_selected)) {
				std::stringstream newAnimGroupName{};
				AnimationGroup newAnimGroup{};
				newAnimGroupName << "Animation " << animationSet.animationSet.size();
				selectedAnim = newAnimGroupName.str();
				newAnimGroup.name = selectedAnim;
				animationSet.animationSet.push_back(newAnimGroup);
				selectedFrame = -1;
				selectedType.clear();
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		//Assign pointer for animation group
		for (int n = 0; n < animationSet.animationSet.size(); n++) {
			if (selectedAnim == animationSet.animationSet[n].name) {
				selectedAnimGroup = &animationSet.animationSet[n];
				break;
			}
		}

		//Entity default animation
		if (ImGui::BeginCombo("Default Animation", animationSet.defaultAnimation.c_str())) {
			for (int n = 0; n < animationSet.animationSet.size(); n++) {
				bool is_selected = (animationSet.defaultAnimation == animationSet.animationSet[n].name);
				if (ImGui::Selectable(animationSet.animationSet[n].name.c_str(), is_selected)) {
					animationSet.defaultAnimation = animationSet.animationSet[n].name;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		//Input text for animation name
		if (selectedAnimGroup != nullptr) {
			ImGui::InputText("Animation Name", &selectedAnim);
			if (selectedAnim != "") {
				selectedAnimGroup->name = selectedAnim;
			}
		}

		if (selectedAnimGroup != nullptr) {
			//If animation loops or not
			ImGui::Checkbox("Animation Loop", &selectedAnimGroup->loop);

			//Time per animation frame
			if (selectedAnimGroup->frametime == 0.f) {
				selectedAnimGroup->frametime = FIXED_DT;
			}
			ImGui::InputFloat("Frame Time", &selectedAnimGroup->frametime);

			//Total frames for the animation
			ImGui::InputInt("Total Frames", &selectedAnimGroup->totalFrames);

			//Combo box for available animation types
			if (ImGui::BeginCombo("Types Available", selectedType.c_str())) {
				for (int n = 0; n < animTypeNames.size(); n++) {
					bool is_selected = (selectedType.c_str() == animTypeNames[n]);
					if (ImGui::Selectable(animTypeNames[n], is_selected)) {
						selectedType = animTypeNames[n];
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			//Check if animation type is already added
			bool hasType{ false };
			for (auto& t : selectedAnimGroup->animations) {
				if (selectedType == t->GetType()) {
					hasType = true;
					break;
				}
			}

			//To add animation types
			if (selectedType != "") {
				if (!hasType) {
					ImGui::SameLine();
					if (ImGui::Button("Add Animation Type")) {
						if (selectedType == "Sprite") {
							selectedAnimGroup->animations.push_back(std::make_shared<SpriteAnimation>());
						}
						else if (selectedType == "TextureChange") {
							selectedAnimGroup->animations.push_back(std::make_shared<ChangeTexAnimation>());
						}
						else if (selectedType == "Sound") {
							selectedAnimGroup->animations.push_back(std::make_shared<SoundAnimation>());
						}
						else if (selectedType == "Fade") {
							selectedAnimGroup->animations.push_back(std::make_shared<FadeAnimation>());
						}
						else if (selectedType == "Color") {
							selectedAnimGroup->animations.push_back(std::make_shared<ColorAnimation>());
						}
						else if (selectedType == "TransformAttach") {
							selectedAnimGroup->animations.push_back(std::make_shared<TransformAttachAnimation>());
						}
						else if (selectedType == "TransformDirect") {
							selectedAnimGroup->animations.push_back(std::make_shared<TransformDirectAnimation>());
						}
						else if (selectedType == "Swap") {
							selectedAnimGroup->animations.push_back(std::make_shared<SwapAnimation>());
						}
						else if (selectedType == "SelfDestruct") {
							selectedAnimGroup->animations.push_back(std::make_shared<SelfDestructAnimation>());
						}
						else if (selectedType == "DamageImpact") {
							selectedAnimGroup->animations.push_back(std::make_shared<DamageImpactAnimation>());
						}
						else if (selectedType == "CameraZoom") {
							selectedAnimGroup->animations.push_back(std::make_shared<CameraZoomAnimation>());
						}
						else if (selectedType == "CameraTarget") {
							selectedAnimGroup->animations.push_back(std::make_shared<CameraTargetAnimation>());
						}
						else if (selectedType == "CameraReset") {
							selectedAnimGroup->animations.push_back(std::make_shared<CameraResetAnimation>());
						}
						else if (selectedType == "CreatePrefab") {
							selectedAnimGroup->animations.push_back(std::make_shared<CreatePrefabAnimation>());
						}
						else if (selectedType == "Event") {
							selectedAnimGroup->animations.push_back(std::make_shared<EventAnimation>());
						}
					}
				}
				else {
					ImGui::SameLine();
					if (ImGui::Button("Remove Animation Type")) {
						std::vector<std::shared_ptr<Animation>> newAnimationVector{};
						for (auto& a : selectedAnimGroup->animations) {
							if (a->GetType() != selectedType) {
								newAnimationVector.push_back(a);
							}
						}
						selectedAnimGroup->animations = newAnimationVector;
					}
				}
			}

			//Selected animation frame
			if (selectedFrame >= 0) {
				ImGui::InputInt("Selected frame", &selectedFrame);
			}

			//Change current keyframe details
			if (selectedAnimation != nullptr) {
				if (!selectedAnimation->HasKeyFrame(selectedFrame)) {
					if (ImGui::Button("Add new keyframe")) {
						selectedAnimation->AddKeyFrame(selectedFrame, nullptr);
					}
				}
				else {
					if (selectedAnimation->GetType() == "Sprite") {
						std::shared_ptr<SpriteAnimation> sprite{ std::dynamic_pointer_cast<SpriteAnimation>(selectedAnimation) };
						Keyframe<bool>* keyframe{ nullptr };
						for (auto& k : sprite->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							ImGui::Checkbox("Reverse", &keyframe->data);
						}
					}
					else if (selectedAnimation->GetType() == "TextureChange") {
						std::shared_ptr<ChangeTexAnimation> changetex{ std::dynamic_pointer_cast<ChangeTexAnimation>(selectedAnimation) };
						Keyframe<std::string>* keyframe{ nullptr };
						for (auto& k : changetex->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							ImGui::InputText("Texture to change", &keyframe->data);
						}
					}
					else if (selectedAnimation->GetType() == "Sound") {
						std::shared_ptr<SoundAnimation> sound{ std::dynamic_pointer_cast<SoundAnimation>(selectedAnimation) };
						Keyframe<std::string>* keyframe{ nullptr };
						for (auto& k : sound->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							if (ImGui::BeginCombo("Sounds Available", keyframe->data.c_str())) {
								std::vector<std::string> soundPaths{ assetmanager.audio.GetSoundPaths() };
								for (int n = 0; n < soundPaths.size(); n++) {
									bool is_selected = (keyframe->data == soundPaths[n]);
									if (ImGui::Selectable(soundPaths[n].c_str(), is_selected)) {
										keyframe->data = soundPaths[n];
									}
									if (is_selected) {
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
						}
					}
					else if (selectedAnimation->GetType() == "TransformAttach") {
						std::shared_ptr<TransformAttachAnimation> transDirect{ std::dynamic_pointer_cast<TransformAttachAnimation>(selectedAnimation) };
						Keyframe<std::string>* keyframe{ nullptr };
						for (auto& k : transDirect->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							if (ImGui::BeginCombo("Entities Available", keyframe->data.c_str())) {
								auto nameList{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Name>().GetPairArray() };
								for (int n = 0; n < nameList.size(); n++) {
									if (ECS::ecs().HasComponent<Clone>(nameList[n].first)) {
										bool is_selected = (keyframe->data == nameList[n].second->name);
										if (ImGui::Selectable(nameList[n].second->name.c_str(), is_selected)) {
											keyframe->data = nameList[n].second->name;
										}
										if (is_selected) {
											ImGui::SetItemDefaultFocus();
										}
									}
								}
								ImGui::EndCombo();
							}
						}
					}
					else if (selectedAnimation->GetType() == "TransformDirect") {
						std::shared_ptr<TransformDirectAnimation> transDirect{ std::dynamic_pointer_cast<TransformDirectAnimation>(selectedAnimation) };
						Keyframe<Transform>* keyframe{ nullptr };
						for (auto& k : transDirect->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							ImGui::InputFloat2("Position", &keyframe->data.position[0]);
							ImGui::InputFloat("Rotation", &keyframe->data.rotation);
							ImGui::InputFloat("Scale", &keyframe->data.scale);
						}
					}
					else if (selectedAnimation->GetType() == "Fade") {
						std::shared_ptr<FadeAnimation> fade{ std::dynamic_pointer_cast<FadeAnimation>(selectedAnimation) };
						Keyframe<float>* keyframe{ nullptr };
						for (auto& k : fade->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							ImGui::InputFloat("Alpha to change to", &keyframe->data);
						}
					}
					else if (selectedAnimation->GetType() == "Color") {
						std::shared_ptr<ColorAnimation> fade{ std::dynamic_pointer_cast<ColorAnimation>(selectedAnimation) };
						Keyframe<glm::vec3>* keyframe{ nullptr };
						for (auto& k : fade->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							ImGui::InputFloat3("Color to change to", &keyframe->data[0]);
						}
					}
					else if (selectedAnimation->GetType() == "CameraZoom") {
						std::shared_ptr<CameraZoomAnimation> zoom{ std::dynamic_pointer_cast<CameraZoomAnimation>(selectedAnimation) };
						Keyframe<float>* keyframe{ nullptr };
						for (auto& k : zoom->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							ImGui::InputFloat("Zoom to set to", &keyframe->data);
						}
					}
					else if (selectedAnimation->GetType() == "CreatePrefab") {
						std::shared_ptr<CreatePrefabAnimation> changetex{ std::dynamic_pointer_cast<CreatePrefabAnimation>(selectedAnimation) };
						Keyframe<std::string>* keyframe{ nullptr };
						for (auto& k : changetex->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							if (ImGui::BeginCombo("Prefabs Available", keyframe->data.c_str())) {
								std::vector<std::string> prefabPaths{ assetmanager.GetPrefabPaths() };
								for (int n = 0; n < prefabPaths.size(); n++) {
									bool is_selected = (keyframe->data == prefabPaths[n]);
									if (ImGui::Selectable(prefabPaths[n].c_str(), is_selected)) {
										keyframe->data = prefabPaths[n];
									}
									if (is_selected) {
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
						}
					}
					else if (selectedAnimation->GetType() == "Event") {
						std::shared_ptr<EventAnimation> eventAnim{ std::dynamic_pointer_cast<EventAnimation>(selectedAnimation) };
						Keyframe<std::pair<std::string,std::string>>* keyframe{ nullptr };
						for (auto& k : eventAnim->keyframes) {
							if (k.frameNum == selectedFrame) {
								keyframe = &k;
								break;
							}
						}
						if (keyframe != nullptr) {
							if (ImGui::BeginCombo("Events Available", keyframe->data.first.c_str())) {
								std::vector<const char*> eventNames{ events.GetFunctionNames() };
								for (int n = 0; n < eventNames.size(); n++) {
									bool is_selected = (keyframe->data.first == eventNames[n]);
									if (ImGui::Selectable(eventNames[n], is_selected)) {
										keyframe->data.first = eventNames[n];
									}
									if (is_selected) {
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
							ImGui::InputText("Event Input", &keyframe->data.second);
						}
					}
					else if (selectedAnimation->GetType() == "DamageImpact") {
					std::shared_ptr<DamageImpactAnimation> changetex{ std::dynamic_pointer_cast<DamageImpactAnimation>(selectedAnimation) };
					Keyframe<std::string>* keyframe{ nullptr };
					for (auto& k : changetex->keyframes) {
						if (k.frameNum == selectedFrame) {
							keyframe = &k;
							break;
						}
					}
					if (keyframe != nullptr) {
						if (ImGui::BeginCombo("Effect Prefab", keyframe->data.c_str())) {
							std::vector<std::string> prefabPaths{ assetmanager.GetPrefabPaths() };
							for (int n = 0; n < prefabPaths.size(); n++) {
								bool is_selected = (keyframe->data == prefabPaths[n]);
								if (ImGui::Selectable(prefabPaths[n].c_str(), is_selected)) {
									keyframe->data = prefabPaths[n];
								}
								if (is_selected) {
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
					}
					}
					else if (selectedAnimation->GetType() == "Swap") {
						std::shared_ptr<SwapAnimation> swap{ std::dynamic_pointer_cast<SwapAnimation>(selectedAnimation) };
						if (swap->keyframes.frameNum != -1) {
							if (ImGui::BeginCombo("Animation to swap to", swap->keyframes.data.c_str())) {
								for (int n = 0; n < animationSet.animationSet.size(); n++) {
									bool is_selected = (swap->keyframes.data == animationSet.animationSet[n].name);
									if (ImGui::Selectable(animationSet.animationSet[n].name.c_str(), is_selected)) {
										swap->keyframes.data = animationSet.animationSet[n].name;
									}
									if (is_selected) {
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
						}
					}
					if (ImGui::Button("Remove keyframe")) {
						selectedAnimation->RemoveKeyFrame(selectedFrame);
						selectedAnimGroup->active = false;
						selectedAnimGroup->currentFrame = -1;
						animationSet.paused = false;
					}
				}
			}

			//Padding for animation timeline later
			int buttonID{ 0 };
			ImVec2 padding{};
			for (auto& animation : selectedAnimGroup->animations) {
				ImVec2 checkPadding{ ImGui::CalcTextSize(animation->GetType().c_str()) };
				if (checkPadding.x > padding.x) {
					padding = checkPadding;
				}
			}

			//Animation timeline
			for (auto& animation : selectedAnimGroup->animations) {
				ImGui::Text(animation->GetType().c_str());
				float paddingSpaceX{ padding.x - ImGui::CalcTextSize(animation->GetType().c_str()).x };
				if (paddingSpaceX > 0) {
					ImGui::SameLine(0, 0);
					ImGui::InvisibleButton("  ", ImVec2{ paddingSpaceX, padding.y });
				}


				ImGui::SameLine();
				for (int i = 0; i < selectedAnimGroup->totalFrames; i++) {
					bool colorChanged{ true };
					if (i == selectedAnimGroup->currentFrame && selectedAnimGroup->active) {
						ImGui::PushStyleColor(ImGuiCol_Button, playingCol);
					}
					else if (selectedFrame == i && selectedAnimation->GetType() == animation->GetType()) {
						ImGui::PushStyleColor(ImGuiCol_Button, selectedCol);
					}
					else if (animation->HasKeyFrame(i)) {
						ImGui::PushStyleColor(ImGuiCol_Button, containsCol);
					}
					else {
						colorChanged = false;
					}
					ImGui::PushID(buttonID + i);
					if (ImGui::Button(" ", buttonsize)) {
						selectedAnimation = animation;
						selectedFrame = i;
					}
					ImGui::PopID();
					if (colorChanged) {
						ImGui::PopStyleColor();
					}
					if (i != selectedAnimGroup->totalFrames - 1) {
						ImGui::SameLine(0, 0);
					}
				}
				buttonID += 100;
			}
		}
	}
}

void UpdateAnimator() {
	const ImGuiWindowFlags flag{ ImGuiWindowFlags_HorizontalScrollbar };
	static bool prefab{ false };
	ImGui::Begin("Animator Window",nullptr,flag);

	Entity toAnimate{ currentSelectedEntity };

	//Checkbox to determine if using selected scene entity or selected scene prefab
	ImGui::Checkbox("Edit prefab?", &prefab);

	//Label for current prefab
	std::string editingLabel{ "Editing: " };
	if (prefab) {
		toAnimate = currentSelectedPrefab;
	}
	if (ECS::ecs().HasComponent<AnimationSet>(toAnimate)) {
		if (prefab) {
			editingLabel += assetmanager.GetPrefabName(currentSelectedPrefab);
		}
		else {
			editingLabel += ECS::ecs().GetComponent<Name>(currentSelectedEntity).name;
		}
		ImGui::Text(editingLabel.c_str());
	}
	else if (ECS::ecs().EntityExists(toAnimate)) {
		//If selected entity has no animation component, add a button to add animation set
		ImGui::Text("Entity has no animation component.");
		if (ImGui::Button("Add Animation Component")) {
			ECS::ecs().AddComponent(toAnimate, AnimationSet{});
		}
	}

	//Calls the main window loop
	AnimatorWindow(toAnimate);
	
	//Real-time prefab updating
	if (prefab && GetCurrentSystemMode() == SystemMode::EDIT && ImGui::IsWindowFocused()) {
		auto& cloneArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Clone>() };
		auto& typeManager{ ECS::ecs().GetTypeManager() };
		auto cloneIDArray{ cloneArray.GetEntityArray() };
		std::string prefabName{ assetmanager.GetPrefabName(currentSelectedPrefab) };
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
