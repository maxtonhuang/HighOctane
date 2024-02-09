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
*	@file		Transition.cpp
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

#include "Transition.h"
#include "Global.h"
#include "Events.h"
#include "EntityFactory.h"

bool transitionActive{ false };
bool transitionType{};
const float TRANSITION_TIME{ 1.2f };
std::string transitionNextScene{};

const std::string TRANSITION_FADEIN_PREFAB{ "transition_fadein.prefab" };
const std::string TRANSITION_FADEOUT_PREFAB{ "transition_fadeout.prefab" };

void TransitionSystem::Update() {
	if (transitionActive) {
		if (timer > 0.f) {
			timer -= FIXED_DT;
			if (timer < 0.f) {
				if (transitionType) {
					events.Call("Change Scene", transitionNextScene);
					transitionType = false;
				}
				else {
					transitionActive = false;
				}
			}
		}
		else {
			timer = TRANSITION_TIME;
			if (transitionType) {
				EntityFactory::entityFactory().ClonePrefab(TRANSITION_FADEOUT_PREFAB);
			}
			else {
				EntityFactory::entityFactory().ClonePrefab(TRANSITION_FADEIN_PREFAB);
			}
		}
	}
}
