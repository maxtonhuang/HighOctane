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
*	@file		Animation.cpp
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
*	@brief		Animations
*
*   Contains ECS animation component (AnimationSet)
*	Animation groups (AnimationGroup) as well as
*	individual animation types (Animation base class as well as other
*	inherited animation classes)
*
******************************************************************************/

#include "Animation.h"
#include "AssetManager.h"
#include "Events.h"
#include "EntityFactory.h"
#include "Global.h"
#include "Camera.h"
#include "Layering.h"
#include <random>

void AnimationSet::Initialise(Entity entity) {
	Start(defaultAnimation, entity);
	initialised = true;
}

void AnimationSet::Start(std::string animationName, Entity entity) {
	// Set active animation
	bool found{ false };
	initialised = true;
	for (auto& a : animationSet) {
		if (a.name == animationName) {
			activeAnimation = &a;
			found = true;
			break;
		}
	}
	if (!found) {
		return;
	}
	if (activeAnimation != nullptr) {
		activeAnimation->Start(entity);
	}
}

void AnimationSet::Queue(std::string animationName, Entity entity) {
	for (auto& a : animationSet) {
		if (a.name == animationName) {
			animationQueue.push(&a);
			break;
		}
	}
	if (!animationQueue.empty() && (activeAnimation == nullptr || activeAnimation->active == false || activeAnimation->loop == true)) {
		activeAnimation = animationQueue.front();
		activeAnimation->Start(entity);
		animationQueue.pop();
	}
}

void AnimationSet::Update(Entity entity) {
	if (!initialised) {
		Initialise(entity);
	}
	if (activeAnimation != nullptr && !paused) {
		activeAnimation->Update(entity);
	}
	if (!animationQueue.empty() && (activeAnimation == nullptr || activeAnimation->active == false || activeAnimation->loop == true)) {
		activeAnimation = animationQueue.front();
		activeAnimation->Start(entity);
		animationQueue.pop();
	}
}

void AnimationSet::InstantFinish(Entity entity) {
	while (!animationQueue.empty() || !(activeAnimation == nullptr || activeAnimation->active == false || activeAnimation->loop == true)) {
		Update(entity);
	}
}

void AnimationSet::Stop() {
	std::queue<AnimationGroup*> newQueue{};
	animationQueue = newQueue;
	activeAnimation = nullptr;
}

AnimationSet::AnimationSet(const AnimationSet& copy) {
	*this = copy;
}

AnimationSet& AnimationSet::operator= (const AnimationSet& copy) {
	initialised = copy.initialised;
	animationSet = copy.animationSet;
	defaultAnimation = copy.defaultAnimation;
	animationQueue = copy.animationQueue;
	activeAnimation = nullptr;
	if (copy.activeAnimation != nullptr) {
		for (auto& animation : animationSet) {
			if (animation.name == copy.activeAnimation->name) {
				activeAnimation = &animation;
				break;
			}
		}
	}
	return *this;
}

void AnimationGroup::Start(Entity entity) {
	active = true;
	currentFrame = 0;
	parent = entity;
	if (frametime == 0.f) {
		frametime = FIXED_DT;
	}
	for (auto& a : animations) {
		if (a->HasKeyFrame(-1)) {
			a->RemoveKeyFrame(-1);
		}
		a->SetFrameTime(frametime);
		a->SetParent(parent);
		a->Start();
	}
}

void AnimationGroup::Update(Entity entity) {
	if (active == false) {
		return;
	}

	updatetime += FIXED_DT;
	if (frametime <= 0.f) {
		frametime = FIXED_DT;
	}
	while (updatetime >= frametime) {
		updatetime -= frametime;
		currentFrame++;
	}

	for (auto& a : animations) {
		if (a->IsActive()) {
			a->SetFrameTime(frametime);
			a->SetParent(entity);
			a->Update(currentFrame);
		}
	}
	
	if (currentFrame >= totalFrames) {
		if (loop) {
			//Restart animation
			Start(parent);
		}
		else {
			active = false;
		}
	}
}

AnimationGroup& AnimationGroup::operator= (const AnimationGroup& copy) {
	animations.clear();
	parent = copy.parent;
	for (auto& animation : copy.animations) {
		if (animation->GetType() == "Sprite") {
			std::shared_ptr<SpriteAnimation> ptr{ std::make_shared<SpriteAnimation>() };
			std::shared_ptr<SpriteAnimation> copyptr{ std::dynamic_pointer_cast<SpriteAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "TextureChange") {
			std::shared_ptr <ChangeTexAnimation> ptr{ std::make_shared<ChangeTexAnimation>() };
			std::shared_ptr<ChangeTexAnimation> copyptr{ std::dynamic_pointer_cast<ChangeTexAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Sound") {
			std::shared_ptr <SoundAnimation> ptr{ std::make_shared<SoundAnimation>() };
			std::shared_ptr<SoundAnimation> copyptr{ std::dynamic_pointer_cast<SoundAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Swap") {
			std::shared_ptr <SwapAnimation> ptr{ std::make_shared<SwapAnimation>() };
			*ptr = *std::dynamic_pointer_cast<SwapAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "TransformAttach") {
			std::shared_ptr <TransformAttachAnimation> ptr{ std::make_shared<TransformAttachAnimation>() };
			std::shared_ptr<TransformAttachAnimation> copyptr{ std::dynamic_pointer_cast<TransformAttachAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "TransformDirect") {
			std::shared_ptr <TransformDirectAnimation> ptr{ std::make_shared<TransformDirectAnimation>() };
			std::shared_ptr<TransformDirectAnimation> copyptr{ std::dynamic_pointer_cast<TransformDirectAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
				ptr->entityTransform = &ECS::ecs().GetComponent<Transform>(parent);
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Fade") {
			std::shared_ptr <FadeAnimation> ptr{ std::make_shared<FadeAnimation>() };
			std::shared_ptr<FadeAnimation> copyptr{ std::dynamic_pointer_cast<FadeAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Color") {
			std::shared_ptr <ColorAnimation> ptr{ std::make_shared<ColorAnimation>() };
			std::shared_ptr<ColorAnimation> copyptr{ std::dynamic_pointer_cast<ColorAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "SelfDestruct") {
			std::shared_ptr <SelfDestructAnimation> ptr{ std::make_shared<SelfDestructAnimation>() };
			*ptr = *std::dynamic_pointer_cast<SelfDestructAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "DamageImpact") {
			std::shared_ptr <DamageImpactAnimation> ptr{ std::make_shared<DamageImpactAnimation>() };
			std::shared_ptr<DamageImpactAnimation> copyptr{ std::dynamic_pointer_cast<DamageImpactAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "CameraZoom") {
			std::shared_ptr <CameraZoomAnimation> ptr{ std::make_shared<CameraZoomAnimation>() };
			std::shared_ptr<CameraZoomAnimation> copyptr{ std::dynamic_pointer_cast<CameraZoomAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "CameraTarget") {
			std::shared_ptr <CameraTargetAnimation> ptr{ std::make_shared<CameraTargetAnimation>() };
			std::shared_ptr<CameraTargetAnimation> copyptr{ std::dynamic_pointer_cast<CameraTargetAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "CameraReset") {
			std::shared_ptr <CameraResetAnimation> ptr{ std::make_shared<CameraResetAnimation>() };
			std::shared_ptr<CameraResetAnimation> copyptr{ std::dynamic_pointer_cast<CameraResetAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "CreatePrefab") {
			std::shared_ptr <CreatePrefabAnimation> ptr{ std::make_shared<CreatePrefabAnimation>() };
			std::shared_ptr<CreatePrefabAnimation> copyptr{ std::dynamic_pointer_cast<CreatePrefabAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Event") {
			std::shared_ptr <EventAnimation> ptr{ std::make_shared<EventAnimation>() };
			std::shared_ptr<EventAnimation> copyptr{ std::dynamic_pointer_cast<EventAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Child") {
			std::shared_ptr <ChildAnimation> ptr{ std::make_shared<ChildAnimation>() };
			std::shared_ptr<ChildAnimation> copyptr{ std::dynamic_pointer_cast<ChildAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Parent") {
			std::shared_ptr <ParentAnimation> ptr{ std::make_shared<ParentAnimation>() };
			std::shared_ptr<ParentAnimation> copyptr{ std::dynamic_pointer_cast<ParentAnimation>(animation) };
			*ptr = *copyptr;
			if (copyptr->IsActive()) {
				auto keyframe{ ptr->keyframes.begin() };
				while (keyframe->frameNum != copyptr->nextKeyframe->frameNum && keyframe != ptr->keyframes.end()) {
					keyframe++;
				}
				ptr->nextKeyframe = keyframe;
			}
			animations.push_back(ptr);
			}
		else {
			ASSERT(1, "Unable to copy animation group!");
		}
	}
	totalFrames = copy.totalFrames;
	name = copy.name;
	loop = copy.loop;
	frametime = copy.frametime;
	currentFrame = copy.currentFrame;
	active = copy.active;
	return *this;
}

AnimationGroup::AnimationGroup(const AnimationGroup& copy) {
	*this = copy;
}

bool Animation::IsActive() {
	return active;
}

std::string Animation::GetType() const {
	return type;
}

void Animation::SetParent(Entity p) {
	parent = p;
}

void Animation::SetFrameTime(float f) {
	frametime = f;
}

SpriteAnimation::SpriteAnimation() {
	type = "Sprite";
}
void SpriteAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
void SpriteAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		// Advance animation
		Tex& tex{ ECS::ecs().GetComponent<Tex>(parent) };
		if (nextKeyframe->data) {
			tex.frameIndex = tex.frameIndex - 1;
			if (tex.frameIndex < 0) {
				tex.frameIndex = tex.tex->GetSheetSize() - 1;
			}
		}
		else {
			tex.frameIndex = (tex.frameIndex + 1) % tex.tex->GetSheetSize();
		}
		
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void SpriteAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<bool> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<bool*>(frameData));
	}
	else{
		frame.data = false;
	}
	
	keyframes.push_back(frame);
	keyframes.sort();
}
void SpriteAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<bool>{frameNum});
}
bool SpriteAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

ChangeTexAnimation::ChangeTexAnimation() {
	type = "TextureChange";
}
void ChangeTexAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
void ChangeTexAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		//Change texture
		Tex& tex{ ECS::ecs().GetComponent<Tex>(parent) };
		tex.tex = assetmanager.texture.Get(nextKeyframe->data.c_str());
		tex.frameIndex = 0;
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void ChangeTexAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::string> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::string*>(frameData));
	}
	else {
		frame.data = "";
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void ChangeTexAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
}
bool ChangeTexAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

void SoundAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
SoundAnimation::SoundAnimation() {
	type = "Sound";
}
void SoundAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		//Play sound
		std::random_device rd;
		std::mt19937 gen(rd());
		std::string soundGroup{ "SFX" };
		if (nextKeyframe->data.size() > 0) {
			std::uniform_int_distribution<int> dis(0, (int)nextKeyframe->data.size() - 1);
			int num{ dis(gen) };
			FMOD::Channel* channel = assetmanager.audio.PlaySounds(nextKeyframe->data[num].c_str(), soundGroup.c_str());
			float offset{ 0.f };
			if (ECS::ecs().GetComponent<Model>(parent).type == ModelType::GAMEPLAY) {
				offset = camera.GetPos().x;
			}
			float pan{ (ECS::ecs().GetComponent<Transform>(parent).position.x - offset) / GRAPHICS::defaultWidthF };
			channel->setPan(pan);
		}
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void SoundAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::vector<std::string>> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::vector<std::string>*>(frameData));
	}
	else {
		frame.data = std::vector<std::string>{};
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void SoundAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::vector<std::string>>{frameNum});
}
bool SoundAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

SwapAnimation::SwapAnimation() {
	type = "Swap";
	keyframes.frameNum = -1;
}
void SwapAnimation::Update(int frameNum) {
	if (frameNum == keyframes.frameNum) {
		AnimationSet& anim{ ECS::ecs().GetComponent<AnimationSet>(parent) };
		anim.Start(keyframes.data, parent);
		active = false;
	}
}
void SwapAnimation::AddKeyFrame(int frameNum, void* frameData) {
	keyframes.frameNum = frameNum;
	if (frameData != nullptr) {
		keyframes.data = *(static_cast<std::string*>(frameData));
	}
}
void SwapAnimation::RemoveKeyFrame(int frameNum) {
	if (keyframes.frameNum == frameNum) {
		keyframes.frameNum = -1;
		keyframes.data.clear();
	}
}
bool SwapAnimation::HasKeyFrame(int frameNum) {
	return keyframes.frameNum == frameNum;
}

Transform* TransformAttachAnimation::GetEntityTransform(std::string entityName) {
	auto namePairArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Name>().GetPairArray() };
	for (auto& namePair : namePairArray) {
		if (namePair.second->name == entityName) {
			if (ECS::ecs().HasComponent<Clone>(namePair.first)) {
				return &ECS::ecs().GetComponent<Transform>(namePair.first);
			}
		}
	}
	return nullptr;
}
TransformAttachAnimation::TransformAttachAnimation() {
	type = "TransformAttach";
}
void TransformAttachAnimation::Start() {
	static auto& transformArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>() };
	active = true;
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	Transform* nextTransform{ GetEntityTransform(nextKeyframe->data) };
	Transform* entityTransform{ &transformArray.GetData(parent) };
	if (nextTransform != nullptr) {
		velocity = (nextTransform->position - entityTransform->position) / frameCount * FIXED_DT / frametime;
	}
}
void TransformAttachAnimation::Update(int frameNum) {
	static auto& transformArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>() };
	if (keyframes.size() == 0) {
		return;
	}

	Transform* entityTransform{ &transformArray.GetData(parent) };
	entityTransform->position += velocity;


	if (frameNum > nextKeyframe->frameNum) {
		float frameCount{ (float)(nextKeyframe->frameNum) };
		Transform* prevTransform{ GetEntityTransform(nextKeyframe->data) };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			entityTransform->position = prevTransform ? prevTransform->position : entityTransform->position;
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			Transform* nextTransform{ GetEntityTransform(nextKeyframe->data) };
			if (nextTransform != nullptr) {
				velocity = (nextTransform->position - entityTransform->position) / frameCount * FIXED_DT / frametime;
			}
		}
	}
}
void TransformAttachAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::string> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::string*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void TransformAttachAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
}
bool TransformAttachAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

TransformDirectAnimation::TransformDirectAnimation() {
	type = "TransformDirect";
}
void TransformDirectAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	active = true;

	if (ECS::ecs().HasComponent<Child>(parent)) {
		entityTransform = &ECS::ecs().GetComponent<Child>(parent).offset;
	}
	else {
		entityTransform = &ECS::ecs().GetComponent<Transform>(parent);
	}
	
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum) };
	if (frameCount == 0) {
		frameCount = 1.f;
	}
	velocity = (nextKeyframe->data.position) / frameCount * FIXED_DT / frametime;
	rotation = (nextKeyframe->data.rotation) / frameCount * FIXED_DT / frametime;
	scale = (nextKeyframe->data.scale) / frameCount * FIXED_DT / frametime;
}
void TransformDirectAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	entityTransform->position += velocity;
	entityTransform->rotation += rotation * vmath::PI / 180;
	if (entityTransform->rotation > vmath::PI) {
		entityTransform->rotation -= 2 * vmath::PI;
	}
	else if (entityTransform->rotation < vmath::PI) {
		entityTransform->rotation += 2 * vmath::PI;
	}
	entityTransform->scale += scale;

	if (frameNum >= nextKeyframe->frameNum) {
		float frameCount{ (float)nextKeyframe->frameNum };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			velocity = (nextKeyframe->data.position) / frameCount * FIXED_DT / frametime;
			rotation = (nextKeyframe->data.rotation) / frameCount * FIXED_DT / frametime;
			scale = (nextKeyframe->data.scale) / frameCount * FIXED_DT / frametime;
		}
	}
}
void TransformDirectAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<Transform> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<Transform*>(frameData));
	}
	else {
		frame.data.scale = 0;
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void TransformDirectAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<Transform>{frameNum});
}
bool TransformDirectAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

FadeAnimation::FadeAnimation() {
	type = "Fade";
}
void FadeAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	active = true;
	entityModel = &ECS::ecs().GetComponent<Model>(parent);
	if (ECS::ecs().HasComponent<TextLabel>(parent)) {
		entityText = &ECS::ecs().GetComponent<TextLabel>(parent);
	}
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum) };
	if (frameCount == 0) {
		frameCount = 1.f;
	}
	alpha = (nextKeyframe->data - entityModel->GetAlpha()) / frameCount * FIXED_DT / frametime;
	if (entityText != nullptr) {
		alphatext = (nextKeyframe->data - entityText->textColor.a) / frameCount * FIXED_DT / frametime;
	}
}
void FadeAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	entityModel->AddAlpha(alpha);
	if (entityText != nullptr) {
		entityText->textColor.a += alphatext;
	}

	if (frameNum >= nextKeyframe->frameNum) {
		float frameCount{ (float)nextKeyframe->frameNum };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			alpha = (nextKeyframe->data - entityModel->GetAlpha()) / frameCount * FIXED_DT / frametime;
			if (entityText != nullptr) {
				alphatext = (nextKeyframe->data - entityText->textColor.a) / frameCount * FIXED_DT / frametime;
			}
		}
	}
}
void FadeAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<float> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<float*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void FadeAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<float>{frameNum});
}
bool FadeAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

ColorAnimation::ColorAnimation() {
	type = "Color";
}
void ColorAnimation::Start() {
	static auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
	if (keyframes.size() == 0) {
		return;
	}
	active = true;
	entityModel = &modelArray.GetData(parent);
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	color.r = (nextKeyframe->data.r - entityModel->GetColor().r) / frameCount * FIXED_DT / frametime;
	color.g = (nextKeyframe->data.g - entityModel->GetColor().g) / frameCount * FIXED_DT / frametime;
	color.b = (nextKeyframe->data.b - entityModel->GetColor().b) / frameCount * FIXED_DT / frametime;
}
void ColorAnimation::Update(int frameNum) {
	static auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
	if (keyframes.size() == 0) {
		return;
	}

	glm::vec4& entityColor{ modelArray.GetData(parent).GetColorRef()};
	entityColor.r += color.r;
	entityColor.g += color.g;
	entityColor.b += color.b;

	if (frameNum >= nextKeyframe->frameNum) {
		float frameCount{ (float)nextKeyframe->frameNum };
		glm::vec3 prevColor{ nextKeyframe->data };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			modelArray.GetData(parent).SetColor(prevColor.r, prevColor.g, prevColor.b);
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			color.r = (nextKeyframe->data.r - entityModel->GetColor().r) / frameCount * FIXED_DT / frametime;
			color.g = (nextKeyframe->data.g - entityModel->GetColor().g) / frameCount * FIXED_DT / frametime;
			color.b = (nextKeyframe->data.b - entityModel->GetColor().b) / frameCount * FIXED_DT / frametime;
		}
	}
}
void ColorAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<glm::vec3> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<glm::vec3*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void ColorAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<glm::vec3>{frameNum});
}
bool ColorAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

SelfDestructAnimation::SelfDestructAnimation() {
	type = "SelfDestruct";
	keyframes.frameNum = -1;
}
void SelfDestructAnimation::Update(int frameNum) {
	if (frameNum == keyframes.frameNum) {
		EntityFactory::entityFactory().DeleteCloneModel(parent);
	}
}
void SelfDestructAnimation::AddKeyFrame(int frameNum, void* frameData) {
	keyframes.frameNum = frameNum;
	if (frameData != nullptr) {
		keyframes.data = *(static_cast<int*>(frameData));
	}
}
void SelfDestructAnimation::RemoveKeyFrame(int frameNum) {
	if (keyframes.frameNum == frameNum) {
		keyframes.frameNum = -1;
	}
}
bool SelfDestructAnimation::HasKeyFrame(int frameNum) {
	return keyframes.frameNum == frameNum;
}

DamageImpactAnimation::DamageImpactAnimation() {
	type = "DamageImpact";
}
void DamageImpactAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
void DamageImpactAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		BattleSystem* battlesystem{ events.GetBattleSystem() };
		battlesystem->damagePrefab = nextKeyframe->data;
		battlesystem->ProcessDamage();
		active = false;
	}
}
void DamageImpactAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::string> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::string*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void DamageImpactAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
}
bool DamageImpactAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

CameraZoomAnimation::CameraZoomAnimation() {
	type = "CameraZoom";
}
void CameraZoomAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	active = true;
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum) };
	if (frameCount == 0.f) {
		frameCount = 1.f;
	}
	zoom = (nextKeyframe->data - camera.GetZoom()) / frameCount * FIXED_DT / frametime;
}
void CameraZoomAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	camera.AddZoom(zoom);

	if (frameNum >= nextKeyframe->frameNum) {
		float frameCount{ (float)nextKeyframe->frameNum };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			zoom = (nextKeyframe->data - camera.GetZoom()) / frameCount * FIXED_DT / frametime;
		}
	}
}
void CameraZoomAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<float> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<float*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void CameraZoomAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<float>{frameNum});
}
bool CameraZoomAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

CameraTargetAnimation::CameraTargetAnimation() {
	type = "CameraTarget";
}
void CameraTargetAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	active = true;
	entityTransform = &ECS::ecs().GetComponent<Transform>(parent);
	nextKeyframe = keyframes.begin();
	//float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	//velocity = (camera.GetPos() - entityTransform->position) / frameCount * FIXED_DT / frametime;
}
void CameraTargetAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	float frameCount{ (float)(nextKeyframe->frameNum + 1 - frameNum) };
	vmath::Vector2 velocity = (entityTransform->position - camera.GetPos()) / frameCount * FIXED_DT / frametime;
	camera.AddPos(velocity.x,velocity.y);
	camera.Clamp();

	if (frameNum >= nextKeyframe->frameNum) {
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			camera.SetTarget(parent);
			active = false;
		}
	}
}
void CameraTargetAnimation::AddKeyFrame(int frameNum, void* frameData) {
	(void)frameData;
	Keyframe<int> frame{ frameNum };
	keyframes.push_back(frame);
	keyframes.sort();
}
void CameraTargetAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<int>{frameNum});
}
bool CameraTargetAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

CameraResetAnimation::CameraResetAnimation() {
	type = "CameraReset";
}
void CameraResetAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	active = true;
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	velocity = (vmath::Vector2{0,0} - camera.GetPos()) / frameCount * FIXED_DT / frametime;
	zoom = (1.f - camera.GetZoom()) / frameCount * FIXED_DT / frametime;
	camera.DetachTarget();
}
void CameraResetAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	camera.AddPos(velocity.x, velocity.y);
	camera.AddZoom(zoom);

	if (frameNum >= nextKeyframe->frameNum) {
		camera.Reset();
		active = false;
	}
}
void CameraResetAnimation::AddKeyFrame(int frameNum, void* frameData) {
	(void)frameData;
	Keyframe<int> frame{ frameNum };
	keyframes.push_back(frame);
	keyframes.sort();
}
void CameraResetAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<int>{frameNum});
}
bool CameraResetAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

CreatePrefabAnimation::CreatePrefabAnimation() {
	type = "CreatePrefab";
}
void CreatePrefabAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
void CreatePrefabAnimation::Update(int frameNum) {
	static auto& transformArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Transform>() };
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		Entity prefabClone{ EntityFactory::entityFactory().ClonePrefab(nextKeyframe->data) };;
		transformArray.GetData(prefabClone).position = transformArray.GetData(parent).position;
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void CreatePrefabAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::string> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::string*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void CreatePrefabAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
}
bool CreatePrefabAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

EventAnimation::EventAnimation() {
	type = "Event";
}
void EventAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
void EventAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		events.Call(nextKeyframe->data.first, nextKeyframe->data.second);
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void EventAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::pair<std::string,std::string>> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::pair<std::string,std::string>*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void EventAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::pair<std::string, std::string>>{frameNum});
}
bool EventAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

ChildAnimation::ChildAnimation() {
	type = "Child";
}
void ChildAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
void ChildAnimation::Update(int frameNum) {
	static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
	static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		Parent& p{ parentArray.GetData(parent) };
		Entity child{ p.GetChildByName(nextKeyframe->data.first) };
		if (child) {
			animationArray.GetData(child).Start(nextKeyframe->data.second,child);
		}
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void ChildAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::pair<std::string, std::string>> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::pair<std::string, std::string>*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void ChildAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::pair<std::string, std::string>>{frameNum});
}
bool ChildAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}

ParentAnimation::ParentAnimation() {
	type = "Parent";
}
void ParentAnimation::Start() {
	if (keyframes.size() == 0) {
		return;
	}
	if (!ECS::ecs().HasComponent<Child>(parent)) {
		return;
	}
	nextKeyframe = keyframes.begin();
	active = true;
}
void ParentAnimation::Update(int frameNum) {
	static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
	static auto& childArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Child>() };
	static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
	if (keyframes.size() == 0) {
		return;
	}
	if (frameNum >= nextKeyframe->frameNum) {
		Child& c{ childArray.GetData(parent) };
		Entity Parent{ c.parent };
		if (Parent) {
			animationArray.GetData(Parent).Start(nextKeyframe->data, Parent);
		}
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void ParentAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::string> frame{ frameNum };
	if (frameData != nullptr) {
		frame.data = *(static_cast<std::string*>(frameData));
	}
	keyframes.push_back(frame);
	keyframes.sort();
}
void ParentAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
}
bool ParentAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}
