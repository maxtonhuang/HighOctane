#include "Animation.h"
#include "AssetManager.h"
#include "Events.h"

void AnimationSet::Start(std::string animationName, Entity entity) {
	//Set active animation
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

void AnimationSet::Update() {
	if (activeAnimation != nullptr && !paused) {
		activeAnimation->Update();
	}
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

void AnimationGroup::Update() {
	if (active == false) {
		return;
	}
	for (auto& a : animations) {
		if (a->IsActive()) {
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

//TransformAttachAnimation::TransformAttachAnimation() {
//	type = "TransformAttach";
//}
//void TransformAttachAnimation::Start() {
//	entityTransform = &ECS::ecs().GetComponent<Transform>(parent);
//}
//void TransformAttachAnimation::Update(int frameNum) {
//	if (keyframes.size() == 0) {
//		return;
//	}
//
//	entityTransform->position += velocity;
//
//	if (frameNum >= nextKeyframe->frameNum) {
//		nextKeyframe++;
//		if (nextKeyframe == keyframes.end()) {
//			active = false;
//		}
//		else {
//
//		}
//	}
//}

TransformDirectAnimation::TransformDirectAnimation() {
	type = "TransformDirect";
}
void TransformDirectAnimation::Start() {
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
	active = true;
	entityModel = &ECS::ecs().GetComponent<Model>(parent);
	nextKeyframe = keyframes.begin();
	float frameCount{ (float)(nextKeyframe->frameNum + 1) };
	alpha = (nextKeyframe->data - entityModel->GetAlpha()) / frameCount;
}
void FadeAnimation::Update(int frameNum) {
	if (keyframes.size() == 0) {
		return;
	}

	entityModel->AddAlpha(alpha);

	if (frameNum >= nextKeyframe->frameNum) {
		float frameCount{ (float)nextKeyframe->frameNum };
		nextKeyframe++;
		if (nextKeyframe == keyframes.end()) {
			active = false;
		}
		else {
			frameCount = nextKeyframe->frameNum - frameCount;
			alpha = (nextKeyframe->data - entityModel->GetAlpha()) / frameCount;
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