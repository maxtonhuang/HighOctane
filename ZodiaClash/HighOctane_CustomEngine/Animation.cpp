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

void AnimationSet::Initialise(Entity entity) {
	Start(defaultAnimation, entity);
	initialised = true;
}

void AnimationSet::Start(std::string animationName, Entity entity) {
	//Set active animation
	initialised = true;
	for (auto& a : animationSet) {
		if (a.name == animationName) {
			activeAnimation = &a;
			break;
		}
	}
	if (activeAnimation != nullptr) {
		activeAnimation->Start(entity);
	}
}

void AnimationSet::Update(Entity entity) {
	if (!initialised) {
		Initialise(entity);
	}
	if (activeAnimation != nullptr && !paused) {
		activeAnimation->Update(entity);
	}
}

AnimationSet::AnimationSet(const AnimationSet& copy) {
	*this = copy;
}

AnimationSet& AnimationSet::operator= (const AnimationSet& copy) {
	animationSet = copy.animationSet;
	defaultAnimation = copy.defaultAnimation;
	initialised = false;
	return *this;
}

void AnimationGroup::Start(Entity entity) {
	active = true;
	currentFrame = 0;
	parent = entity;
	for (auto& a : animations) {
		a->SetParent(parent);
		a->Start();
	}
}

void AnimationGroup::Update(Entity entity) {
	if (active == false) {
		return;
	}
	for (auto& a : animations) {
		if (a->IsActive()) {
			a->SetParent(entity);
			a->Update(currentFrame);
		}
	}
	currentFrame++;
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
	for (auto& animation : copy.animations) {
		if (animation->GetType() == "Sprite") {
			std::shared_ptr <SpriteAnimation> ptr{ std::make_shared<SpriteAnimation>() };
			*ptr = *std::dynamic_pointer_cast<SpriteAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "TextureChange") {
			std::shared_ptr <ChangeTexAnimation> ptr{ std::make_shared<ChangeTexAnimation>() };
			*ptr = *std::dynamic_pointer_cast<ChangeTexAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Sound") {
			std::shared_ptr <SoundAnimation> ptr{ std::make_shared<SoundAnimation>() };
			*ptr = *std::dynamic_pointer_cast<SoundAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Swap") {
			std::shared_ptr <SwapAnimation> ptr{ std::make_shared<SwapAnimation>() };
			*ptr = *std::dynamic_pointer_cast<SwapAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "TransformAttach") {
			std::shared_ptr <TransformAttachAnimation> ptr{ std::make_shared<TransformAttachAnimation>() };
			*ptr = *std::dynamic_pointer_cast<TransformAttachAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "TransformDirect") {
			std::shared_ptr <TransformDirectAnimation> ptr{ std::make_shared<TransformDirectAnimation>() };
			*ptr = *std::dynamic_pointer_cast<TransformDirectAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Fade") {
			std::shared_ptr <FadeAnimation> ptr{ std::make_shared<FadeAnimation>() };
			*ptr = *std::dynamic_pointer_cast<FadeAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "Color") {
			std::shared_ptr <ColorAnimation> ptr{ std::make_shared<ColorAnimation>() };
			*ptr = *std::dynamic_pointer_cast<ColorAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "SelfDestruct") {
			std::shared_ptr <SelfDestructAnimation> ptr{ std::make_shared<SelfDestructAnimation>() };
			*ptr = *std::dynamic_pointer_cast<SelfDestructAnimation>(animation);
			animations.push_back(ptr);
		}
		else if (animation->GetType() == "DamageImpact") {
			std::shared_ptr <DamageImpactAnimation> ptr{ std::make_shared<DamageImpactAnimation>() };
			*ptr = *std::dynamic_pointer_cast<DamageImpactAnimation>(animation);
			animations.push_back(ptr);
		}
		else {
			ASSERT(1, "Unable to copy animation group!");
		}
	}
	totalFrames = copy.totalFrames;
	name = copy.name;
	loop = copy.loop;
	currentFrame = -1;
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
		//Advance animation
		Tex& tex{ ECS::ecs().GetComponent<Tex>(parent) };
		tex.frameIndex = (tex.frameIndex + 1) % tex.tex->GetSheetSize();
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void SpriteAnimation::AddKeyFrame(int frameNum, void* frameData) {
	(void)frameData;
	Keyframe<int> frame{ frameNum };
	keyframes.push_back(frame);
	keyframes.sort();
}
void SpriteAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<int>{frameNum});
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
		events.Call("Play Sound", nextKeyframe->data);
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
	}
}
void SoundAnimation::AddKeyFrame(int frameNum, void* frameData) {
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
void SoundAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
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
	ASSERT(1,"Unable to find entity for attach animation");
	return nullptr;
}
TransformAttachAnimation::TransformAttachAnimation() {
	type = "TransformAttach";
}
void TransformAttachAnimation::Start() {
	active = true;
	entityTransform = &ECS::ecs().GetComponent<Transform>(parent);
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum) + 1 };
	Transform* nextTransform{ GetEntityTransform(nextKeyframe->data) };
	velocity = (nextTransform->position - entityTransform->position) / frameCount;
}
void TransformAttachAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	entityTransform->position += velocity;

	if (frameNum >= nextKeyframe->frameNum) {
		float frameCount{ (float)(nextKeyframe->frameNum) };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			Transform* nextTransform{ GetEntityTransform(nextKeyframe->data) };
			velocity = (nextTransform->position - entityTransform->position) / frameCount;
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
	entityTransform = &ECS::ecs().GetComponent<Transform>(parent);
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	velocity = (nextKeyframe->data.position) / frameCount;
	rotation = (nextKeyframe->data.rotation) / frameCount;
	scale = (nextKeyframe->data.scale) / frameCount;
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
			velocity = (nextKeyframe->data.position) / frameCount;
			rotation = (nextKeyframe->data.rotation) / frameCount;
			scale = (nextKeyframe->data.scale) / frameCount;
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
	float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	alpha = (nextKeyframe->data - entityModel->GetAlpha()) / frameCount;
	if (entityText != nullptr) {
		alphatext = (nextKeyframe->data - entityText->textColor.a) / frameCount;
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
			alpha = (nextKeyframe->data - entityModel->GetAlpha()) / frameCount;
			if (entityText != nullptr) {
				alphatext = (nextKeyframe->data - entityText->textColor.a) / frameCount;
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
	if (keyframes.size() == 0) {
		return;
	}
	active = true;
	entityModel = &ECS::ecs().GetComponent<Model>(parent);
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	color.r = (nextKeyframe->data.r - entityModel->GetColor().r) / frameCount;
	color.g = (nextKeyframe->data.g - entityModel->GetColor().g) / frameCount;
	color.b = (nextKeyframe->data.b - entityModel->GetColor().b) / frameCount;
}
void ColorAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	glm::vec4& entityColor{ entityModel->GetColorRef() };
	entityColor.r += color.r;
	entityColor.g += color.g;
	entityColor.b += color.b;

	if (frameNum >= nextKeyframe->frameNum) {
		float frameCount{ (float)nextKeyframe->frameNum };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			color.r = (nextKeyframe->data.r - entityModel->GetColor().r) / frameCount;
			color.g = (nextKeyframe->data.g - entityModel->GetColor().g) / frameCount;
			color.b = (nextKeyframe->data.b - entityModel->GetColor().b) / frameCount;
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
		battlesystem->ProcessDamage();
	}
}
void DamageImpactAnimation::AddKeyFrame(int frameNum, void* frameData) {
	(void)frameData;
	Keyframe<int> frame{ frameNum };
	keyframes.push_back(frame);
	keyframes.sort();
}
void DamageImpactAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<int>{frameNum});
}
bool DamageImpactAnimation::HasKeyFrame(int frameNum) {
	for (auto& k : keyframes) {
		if (k.frameNum == frameNum) {
			return true;
		}
	}
	return false;
}
