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
*	@file		Animation.h
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

#pragma once
#include <string>
#include <list>
#include "ECS.h"
#include "model.h"
#include "UIComponents.h"

class Animation {
public:
	//Start the animation
	virtual void Start() { active = true; };

	//Update the animation by input frameNum
	virtual void Update(int frameNum) { (void)frameNum; };

	//Add a keyframe by input frameNum, with keyframe data of input frameData
	virtual void AddKeyFrame(int frameNum, void* frameData) { (void)frameNum; (void)frameData; };

	//Remove keyframe at input frameNum
	virtual void RemoveKeyFrame(int frameNum) { (void)frameNum; };

	//Returns true if animation has keyframe at frameNum
	virtual bool HasKeyFrame(int frameNum) { (void)frameNum; return false; };

	//Returns true if animation is currently active
	bool IsActive();

	//Returns name of type of animation
	std::string GetType() const;

	//Sets the entity ID of animation
	void SetParent(Entity p);

	//Sets frame time of animation
	void SetFrameTime(float f);

protected:
	bool active{};
	float frametime{};
	std::string type{};
	Entity parent{};
};

//These are the multiple animation functions that will play in a single animations
class AnimationGroup {
public:
	//Starts animation group, with input of the entities ID
	void Start(Entity entity);

	//Updates animation group by 1 frame, with input of the entities ID
	void Update(Entity entity);

	//Default constructor
	AnimationGroup() = default;

	//Copy constructor to ensure that deep copy of animations are made
	AnimationGroup(const AnimationGroup&);

	//Assignment operator overload to ensure that deep copy of animations are made
	AnimationGroup& operator= (const AnimationGroup&);

	//container for animations
	std::vector<std::shared_ptr<Animation>> animations{};

	//Total amount of frames in the animation
	int totalFrames{};

	//Current frame of the animation
	int currentFrame{};

	//Total time per animation frame
	float frametime{};

	//Time since last update
	float updatetime{};

	//Name of the animation
	std::string name{};

	//If animation is active
	bool active{};

	//If animation should loop
	bool loop{};
private:
	//Entity ID of entity this animation belongs to
	Entity parent{};
};

//These are a set of animation groups per entity
class AnimationSet {
public:
	/*
	Starts animation with name animationName and input entity ID
	If animationName is not found, this function does nothing
	*/
	void Start(std::string animationName, Entity entity);

	//Queue up an animation to start if no animations are playing
	void Queue(std::string animationName, Entity entity);

	//Stops the animation and removes all from queue
	void Stop();

	//Initialises the animation set and runs its default animation
	void Initialise(Entity entity);

	//Updates the animation set by 1 frame
	void Update(Entity entity);

	//Instantly finishes the animation
	void InstantFinish(Entity entity);

	//Default constructor of animation set
	AnimationSet() = default;

	//Copy constructor of animation set to ensure activeAnimation pointer is not copied
	AnimationSet(const AnimationSet&);
	
	//Assignment operator of animation set to ensure activeAnimation pointer is not copied
	AnimationSet& operator= (const AnimationSet&);

	//Container of animations
	std::vector<AnimationGroup> animationSet{};

	//If the animation set is paused
	bool paused{};

	//Default animation to play at entity start
	std::string defaultAnimation{};

	//Current active animation
	AnimationGroup* activeAnimation{};
private:
	//If animation set has been initialised
	bool initialised{ false };

	//To queue up animations
	std::queue<AnimationGroup*> animationQueue{};
};

template <typename T>
class Keyframe {
public:
	//frame number of this key frame
	int frameNum{};

	//Data used by each animation class
	T data{};

	//== operator used for sorting list of keyframes
	bool operator==(const Keyframe& input) const { return frameNum == input.frameNum; }

	//< operator used for sorting list of keyframes
	bool operator<(const Keyframe& input) const { return frameNum < input.frameNum; }
};

//Advances the current texture to the next texture in the sprite sheet, if keyframe data is true, then reverse to previous texture
class SpriteAnimation : public Animation {
public:
	SpriteAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<bool>> keyframes;
	std::list<Keyframe<bool>>::iterator nextKeyframe{};
};

//Changes the current texture to the texture stored in the keyframe 
class ChangeTexAnimation : public Animation {
public:
	ChangeTexAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::string>> keyframes;
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};

//Plays the sound stored in the keyframe 
class SoundAnimation : public Animation {
public:
	SoundAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::vector<std::string>>> keyframes;
	std::list<Keyframe<std::vector<std::string>>>::iterator nextKeyframe{};
};

//Swaps the animation to another animation stored, ending the current animation 
class SwapAnimation : public Animation {
public:
	SwapAnimation();
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	Keyframe<std::string> keyframes;
};

//Interpolates the distance to the entity stored in the next keyframe and travels that distance 
class TransformAttachAnimation : public Animation {
public:
	TransformAttachAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::string>> keyframes;
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
private:
	Transform* GetEntityTransform(std::string);
	vmath::Vector2 velocity; //per frame velocity
};

//Interpolates the transform stored in the next keyframe and transforms the entity by that amount. Transform includes the distance, rotation and scale. 
class TransformDirectAnimation : public Animation {
public:
	TransformDirectAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<Transform>> keyframes;
	std::list<Keyframe<Transform>>::iterator nextKeyframe{};
	Transform* entityTransform;
private:
	vmath::Vector2 velocity; //per frame velocity
	float rotation; //per frame rotation
	float scale; //per frame scale
};

//Interpolates the alpha stored in the next keyframe with the current alpha and changes the alpha each frame until it reaches the targeted alpha by next keyframe.
class FadeAnimation : public Animation {
public:
	FadeAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<float>> keyframes;
	std::list<Keyframe<float>>::iterator nextKeyframe{};
private:
	Model* entityModel{};
	TextLabel* entityText{};
	float alpha;
	float alphatext;
};

//Interpolates the color stored in the next keyframe with the current color and changes the color each frame until it reaches the targeted color by next keyframe. 
class ColorAnimation : public Animation {
public:
	ColorAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<glm::vec3>> keyframes;
	std::list<Keyframe<glm::vec3>>::iterator nextKeyframe{};
private:
	Model* entityModel;
	glm::vec3 color;
};

/*
Not really an animation
Destroys the entity after animation 
(for easier destruction of particles/entities created purely for animation)
*/
class SelfDestructAnimation : public Animation {
public:
	SelfDestructAnimation();
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	Keyframe<int> keyframes;
};

//Signals to the system to play damage animations, string in keyframe to create damage effect
class DamageImpactAnimation : public Animation {
public:
	DamageImpactAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::string>> keyframes;
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};

//Sets the zoom of camera to keyframe zoom
class CameraZoomAnimation : public Animation {
public:
	CameraZoomAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<float>> keyframes;
	std::list<Keyframe<float>>::iterator nextKeyframe{};
private:
	float zoom;
};

//Sets the target of camera to entity
class CameraTargetAnimation : public Animation {
public:
	CameraTargetAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<int>> keyframes;
	std::list<Keyframe<int>>::iterator nextKeyframe{};
private:
	Transform* entityTransform;
};

//Resets the camera to 0,0 and 1.0 zoom
class CameraResetAnimation : public Animation {
public:
	CameraResetAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<int>> keyframes;
	std::list<Keyframe<int>>::iterator nextKeyframe{};
private:
	float zoom;
	vmath::Vector2 velocity;
};

//Creates a copy of a prefab on the entity
class CreatePrefabAnimation : public Animation {
public:
	CreatePrefabAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::string>> keyframes;
	std::list<Keyframe<std::string>>::iterator nextKeyframe{};
};

//Calls an event, general use animation
class EventAnimation : public Animation {
public:
	EventAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::pair<std::string,std::string>>> keyframes;
	std::list<Keyframe<std::pair<std::string, std::string>>>::iterator nextKeyframe{};
};

//Calls a childs animation, does not work if entity is not a parent
class ChildAnimation : public Animation {
public:
	ChildAnimation();
	void Start() override;
	void Update(int frameNum) override;
	void AddKeyFrame(int frameNum, void* frameData) override;
	void RemoveKeyFrame(int frameNum) override;
	bool HasKeyFrame(int frameNum) override;

	std::list<Keyframe<std::pair<std::string, std::string>>> keyframes;
	std::list<Keyframe<std::pair<std::string, std::string>>>::iterator nextKeyframe{};
};
