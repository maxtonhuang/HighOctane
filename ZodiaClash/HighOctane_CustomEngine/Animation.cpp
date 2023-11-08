#include "Animation.h"
#include "AssetManager.h"
#include "Events.h"

void AnimationGroup::Start() {
	active = true;
	currentFrame = 0;
	for (auto& a : animations) {
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
		active = false;
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
