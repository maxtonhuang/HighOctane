#include "Animation.h"
#include "AssetManager.h"
#include "Events.h"

void Animation::Start() {
	active = true;
	currentFrame = 0;
}

bool Animation::IsActive() {
	return active;
}

std::string Animation::GetType() {
	return type;
}

void Animation::SetParent(Entity p) {
	parent = p;
}

SpriteAnimation::SpriteAnimation() {
	type = "Sprite";
}

void SpriteAnimation::Start() {
	active = true;
	currentFrame = 0;
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
}

void SpriteAnimation::Update() {
	if (keyframes.size() == 0) {
		return;
	}
	if (currentFrame > totalFrames) {
		return;
	}
	currentFrame++;
	if (currentFrame >= nextKeyframe->frameNum) {
		//Advance animation
		Tex& tex{ ECS::ecs().GetComponent<Tex>(parent) };
		tex.frameIndex = (tex.frameIndex + 1) % tex.tex->GetSheetSize();
		if (nextKeyframe != keyframes.end()) {
			nextKeyframe++;
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

ChangeTexAnimation::ChangeTexAnimation() {
	type = "TextureChange";
}

void ChangeTexAnimation::Start() {
	active = true;
	currentFrame = 0;
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
}

void ChangeTexAnimation::Update() {
	if (keyframes.size() == 0) {
		return;
	}
	if (currentFrame > totalFrames) {
		return;
	}
	currentFrame++;
	if (currentFrame >= nextKeyframe->frameNum) {
		//Change texture
		Tex& tex{ ECS::ecs().GetComponent<Tex>(parent) };
		tex.tex = assetmanager.texture.Get(nextKeyframe->data.c_str());
		tex.frameIndex = 0;
		if (nextKeyframe != keyframes.end()) {
			nextKeyframe++;
		}
	}
}

void ChangeTexAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::string> frame{ frameNum, *(static_cast<std::string*>(frameData)) };
	keyframes.push_back(frame);
	keyframes.sort();
}

void ChangeTexAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
}

void ChangeTexAnimation::Start() {
	active = true;
	currentFrame = 0;
	if (keyframes.size() == 0) {
		return;
	}
	nextKeyframe = keyframes.begin();
}

SoundAnimation::SoundAnimation() {
	type = "Sound";
}

void SoundAnimation::Update() {
	if (keyframes.size() == 0) {
		return;
	}
	if (currentFrame > totalFrames) {
		return;
	}
	currentFrame++;
	if (currentFrame >= nextKeyframe->frameNum) {
		//Play sound
		events.Call("Play Sound", nextKeyframe->data);
		if (nextKeyframe != keyframes.end()) {
			nextKeyframe++;
		}
	}
}

void SoundAnimation::AddKeyFrame(int frameNum, void* frameData) {
	Keyframe<std::string> frame{ frameNum, *(static_cast<std::string*>(frameData)) };
	keyframes.push_back(frame);
	keyframes.sort();
}

void SoundAnimation::RemoveKeyFrame(int frameNum) {
	keyframes.remove(Keyframe<std::string>{frameNum});
}
