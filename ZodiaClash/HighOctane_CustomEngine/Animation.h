#pragma once
#include <string>
#include <list>
#include "ECS.h"
#include "model.h"

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
	void Start(Entity entity);
	void Update();

	std::vector<std::shared_ptr<Animation>> animations;
	int totalFrames{};
	int currentFrame{};
	std::string name{};
	bool active{};
	bool loop{};

private:
	Entity parent{};
};

//These are a set of animation groups per entity
class AnimationSet {
public:
	void Start(std::string animationName, Entity entity);
	void Initialise(Entity entity);
	void Update();
	std::vector<AnimationGroup> animationSet;
	bool paused;
	std::string defaultAnimation{};
private:
	AnimationGroup* activeAnimation;
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

class SwapAnimation : public Animation {
public:
	SwapAnimation();
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	Keyframe<std::string> keyframes;
};

class TransformAttachAnimation : public Animation {
public:
	TransformAttachAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::string>> keyframes;
private:
	Transform* GetEntityTransform(std::string);
	Transform* entityTransform;
	vmath::Vector2 velocity; //per frame velocity
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};

class TransformDirectAnimation : public Animation {
public:
	TransformDirectAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<Transform>> keyframes;
private:
	Transform* entityTransform;
	vmath::Vector2 velocity; //per frame velocity
	float rotation; //per frame rotation
	float scale; //per frame scale
	std::list<Keyframe<Transform>>::iterator nextKeyframe{};
};

class FadeAnimation : public Animation {
public:
	FadeAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<float>> keyframes;
private:
	Model* entityModel;
	float alpha;
	std::list<Keyframe<float>>::iterator nextKeyframe{};
};

class ColorAnimation : public Animation {
public:
	ColorAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<glm::vec3>> keyframes;
private:
	Model* entityModel;
	glm::vec3 color;
	std::list<Keyframe<glm::vec3>>::iterator nextKeyframe{};
};
