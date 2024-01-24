#pragma once
#include "ECS.h"

enum TransitionType {
	FADE_IN,FADE_OUT
};

extern bool transitionActive;

//True for fade out to the next scene, false for fade in to new scene
extern bool transitionType;

extern std::string transitionNextScene;

class TransitionSystem : public System {
public:
	void Update() override;
private:
	float timer{};
};
