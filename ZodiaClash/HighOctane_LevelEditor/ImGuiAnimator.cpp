#include "ImGuiAnimator.h"
#include "ImGuiSceneHierarchy.h"
#include "Animation.h"

void UpdateAnimator() {
	const ImVec2 buttonsize{ 15,15 };
	const ImVec4 selectedCol{ 0.f,1.f,0.f,1.f };
	const ImVec4 containsCol{ 1.f,1.f,0.f,1.f };
	const ImVec4 playingCol{ 1.f,0.f,0.f,1.f };
	const ImGuiWindowFlags flag{ ImGuiWindowFlags_HorizontalScrollbar };

	const std::vector<const char*> animTypeNames{ "Sprite","TextureChange","Sound"};
	
	static std::string selectedType{};
	static std::string selectedAnim{};
	static int selectedFrame{-1};
	static std::shared_ptr<Animation> selectedAnimation{};
	static AnimationGroup* selectedAnimGroup{};
	static Entity previousEntity;

	ImGui::Begin("Animator Window",nullptr,flag);

	if (ECS::ecs().HasComponent<AnimationSet>(currentSelectedEntity)) {
		AnimationSet& animationSet = ECS::ecs().GetComponent<AnimationSet>(currentSelectedEntity);

		if (previousEntity != currentSelectedEntity) {
			previousEntity = currentSelectedEntity;
			selectedType.clear();
			selectedAnim.clear();
			selectedFrame = -1;
			selectedAnimation = nullptr;
			selectedAnimGroup = nullptr;
		}

		if (selectedAnimGroup != nullptr) {
			if (ImGui::Button("Advance Frame")) {
				if (selectedAnimGroup->active == false) {
					selectedAnimGroup->Start();
				}
				else {
					selectedAnimGroup->Update();
				}
			}
		}

		if (ImGui::BeginCombo("Animations Available", selectedAnim.c_str())) {
			for (int n = 0; n < animationSet.animationSet.size(); n++) {
				bool is_selected = (selectedAnim == animationSet.animationSet[n].name);
				if (ImGui::Selectable(animationSet.animationSet[n].name.c_str(), is_selected)) {
					selectedAnimGroup = &animationSet.animationSet[n];
					selectedAnim = selectedAnimGroup->name;
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
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		for (int n = 0; n < animationSet.animationSet.size(); n++) {
			if (selectedAnim == animationSet.animationSet[n].name) {
				selectedAnimGroup = &animationSet.animationSet[n];
				break;
			}
		}

		if (selectedAnim != "" && selectedAnimGroup != nullptr) {
			ImGui::InputText("Animation Name", &selectedAnim);
			selectedAnimGroup->name = selectedAnim;
		}

		if (selectedAnimGroup != nullptr) {
			ImGui::InputInt("Total Frames", &selectedAnimGroup->totalFrames);

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
			
			bool hasType{ false };
			for (auto& t : selectedAnimGroup->animations) {
				if (selectedType == t->GetType()) {
					hasType = true;
					break;
				}
			}

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
				}
			}

			if (selectedFrame >= 0) {
				std::stringstream frameNum{};
				frameNum << "Selected frame: " << selectedFrame;
				ImGui::Text(frameNum.str().c_str());
			}

			if (selectedAnimation != nullptr) {
				if (!selectedAnimation->HasKeyFrame(selectedFrame)) {
					if (ImGui::Button("Add new keyframe")) {
						selectedAnimation->AddKeyFrame(selectedFrame, nullptr);
					}
				}
				else {
					if (selectedAnimation->GetType() == "TextureChange") {
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
							ImGui::InputText("Sound to play", &keyframe->data);
						}
					}
					if (ImGui::Button("Remove keyframe")) {
						selectedAnimation->RemoveKeyFrame(selectedFrame);
					}
				}
			}

			int buttonID = 0;
			for (auto& animation : selectedAnimGroup->animations) {
				ImGui::Text(animation->GetType().c_str());
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
	ImGui::End();
}
