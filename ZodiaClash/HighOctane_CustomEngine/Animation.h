#pragma once
#include <string>
#include <list>
#include "ECS.h"

class Animation {
public:
	virtual void Start() { active = true; };
	virtual void Update(int frameNum) { (void)frameNum; };
	virtual void AddKeyFrame(int frameNum, void* frameData) { (void)frameNum; (void)frameData; };
	virtual void RemoveKeyFrame(int frameNum) { (void)frameNum; };
	virtual bool HasKeyFrame(int frameNum) { (void)frameNum; return false; };
	bool IsActive();
	std::string GetType() const;
	void SetParent(Entity p);
protected:
	bool active{};
	std::string type{};
	Entity parent{};
};

//These are the multiple animation functions that will play in a single animations
class AnimationGroup {
public:
	void Start();
	void Update();

	std::vector<std::shared_ptr<Animation>> animations;
	int totalFrames{};
	int currentFrame{};
	std::string name{};
	bool active{};
};

//These are a set of animation groups per entity
class AnimationSet {
public:
	std::vector<AnimationGroup> animationSet;
};

template <typename T>
class Keyframe {
public:
	int frameNum{};
	T data{};
	bool operator==(const Keyframe& input) const { return frameNum == input.frameNum; }
	bool operator<(const Keyframe& input) const { return frameNum < input.frameNum; }
};

class SpriteAnimation : public Animation {
public:
	SpriteAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<int>> keyframes;
private:
	std::list<Keyframe<int>>::iterator nextKeyframe{};
};

class ChangeTexAnimation : public Animation {
public:
	ChangeTexAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::string>> keyframes;
private:
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};

class SoundAnimation : public Animation {
public:
	SoundAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::string>> keyframes;
private:
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};
