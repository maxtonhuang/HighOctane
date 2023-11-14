#pragma once
#include <string>
#include <list>
#include "ECS.h"

class Animation {
public:
	virtual void Start();
	virtual void Update() = 0;
	virtual void AddKeyFrame(int frameNum, void* frameData) = 0;
	virtual void RemoveKeyFrame(int frameNum) = 0;
	bool IsActive();
	std::string GetType();
	void SetParent(Entity p);
	int totalFrames{};
	int currentFrame{};
	std::string name{};
protected:
	std::string type{};
	bool active{};
	Entity parent{};
};

struct AnimationSet {
	std::vector<Animation> animations;
};

template <typename T>
class Keyframe {
public:
	int frameNum{};
	T data{};
	bool operator==(const Keyframe& input) { return frameNum == input.frameNum; }
	bool operator<(const Keyframe& input) { return frameNum < input.frameNum; }
};

class SpriteAnimation : public Animation {
public:
	SpriteAnimation();
	void Start() override;
	void Update() override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
private:
	std::list<Keyframe<int>> keyframes;
	std::list<Keyframe<int>>::iterator nextKeyframe{};
};

class ChangeTexAnimation : public Animation {
public:
	ChangeTexAnimation();
	void Start() override;
	void Update() override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
private:
	std::list<Keyframe<std::string>> keyframes;
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};

class SoundAnimation : public Animation {
public:
	SoundAnimation();
	void Start() override;
	void Update() override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
private:
	std::list<Keyframe<std::string>> keyframes;
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};
