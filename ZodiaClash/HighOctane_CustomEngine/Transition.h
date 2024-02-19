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
*	@file		Transition.h
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
*	@date		15 January 2024
*
* *****************************************************************************
*
*	@brief		Transition system for the engine
*
*	Transition system used by the engine to play a transition while
*	changing scenes
*
******************************************************************************/

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
