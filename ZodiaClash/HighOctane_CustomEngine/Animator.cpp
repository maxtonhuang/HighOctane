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
*	@file		Animator.cpp
*
*	@co-author	Chua Zhen Rong
*
*	@email		c.zhenrong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		04 September 2023
*
* *****************************************************************************
*
*	@brief		Animator component used by graphics system
*
*	Animator containing animation functions to update animation frame based on
*	values set in Animation component.
*
******************************************************************************/

#include "Animator.h"
#include "message.h"
#include "Global.h"
//#include "Components.h"
//#include "ECS.h"
//#include "EngineCore.h"

Animator::Animator() {
	animationType = Animator::ANIMATION_NONE;
}

Animator::Animator(ANIMATION_TYPE aniType, float displayDur) {
	animationType = aniType;
	frameDisplayDuration = displayDur;
}

Animator::ANIMATION_TYPE Animator::GetAnimationType() const {
	return animationType;
}

uint32_t Animator::GetFrameIndex() const {
	return frameIndex;
}

float Animator::GetFrameTimeElapsed() const {
	return frameTimeElapsed;
}

float Animator::GetFrameDisplayDuration() const {
	return frameDisplayDuration;
}

// to manually set animation to specified index frame
void Animator::SetAnimation(uint32_t index, Tex& texData) {
	if (index < texData.tex->GetSheetSize()) {
		frameIndex = index;
	}	
}

// core function to cycle through animation frames
void Animator::AdvanceAnimation(Tex& texData) {
	frameIndex = (frameIndex + 1) % texData.tex->GetSheetSize();
}

// changes animation sprite if model has more than 1 texVariant stored
void Animator::ChangeAnimation(Tex& texData) {
	// Update sprite
	if (texData.texVariants.size() == 0) {
		return;
	}
	texData.texVariantIndex = (texData.texVariantIndex + 1) % texData.texVariants.size();
	texData.tex = texData.texVariants.at(texData.texVariantIndex);
}

// called after ChangeAnimation to account for change in texture dimensions
void Animator::ResizeOnChange(Tex& texData, Size& sizeData) {

	sizeData.width = (float)texData.tex->GetWidth();
	sizeData.height = (float)texData.tex->GetHeight();
}

// called for time-based animations; advances to next frame at set intervals (frameDisplayDuration)
void Animator::AnimateOnInterval(Tex& texData) {
	frameTimeElapsed += g_dt;
	if (frameTimeElapsed > frameDisplayDuration) {
		AdvanceAnimation( texData);
		frameTimeElapsed = 0.f;
	}
}

// called for event-based animations; advances on key input
void Animator::AnimateOnKeyPress(Tex& texData) {
	AdvanceAnimation(texData);
}


/********************************************************************************
* 3 kinds of key input handling:
* - KEY_X --> animationType toggling
*	>> Note: ONLY for main character entities!
* - KEY_C --> AnimationChange()
*	>> Note: ONLY for main character entities!
* - KEY_V --> AnimateOnKeyPress()
*	>> Note: (animationType == ANIMATION_EVENT_BASED)
*	>> Note: for ALL entities!
********************************************************************************/

// animation update function for all relevant entities
// called in ModelSystem
void Animator::UpdateAnimation(Tex& texData) {
	if ((animationType != ANIMATION_TIME_BASED) && (animationType != ANIMATION_EVENT_BASED)) { return; }

	if (animationType == ANIMATION_TIME_BASED) {
		AnimateOnInterval(texData);
		//return; 
	}

	// Check mailbox for input triggers
	Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::ANIMATOR, INFO::NONE, 0.f, 0.f);

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::ANIMATOR]) {
		if (msg.type == TYPE::KEY_TRIGGERED) {
			// not in use - sizeData not needed as only MC can change animation; to reinstate when conditions change
			//if (msg.info == INFO::KEY_C) {
			//	ChangeAnimation(aniData, texData);
			//	ResizeOnChange(texData, sizeData);
			//}
			if ((msg.info == INFO::KEY_V) && (animationType == ANIMATION_EVENT_BASED)) {
				AnimateOnKeyPress(texData);
			}
		}
	}
}

// animation update function for all relevant MAIN CHARACTER entities
// called in MovementSystem
void Animator::UpdateAnimationMC(Tex& texData, Size& sizeData) {

	if ((animationType != ANIMATION_TIME_BASED) && (animationType != ANIMATION_EVENT_BASED)) { return; }

	// Check mailbox for input triggers
	Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::ANIMATOR, INFO::NONE, 0.f, 0.f);

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::ANIMATOR]) {
		if (msg.type == TYPE::KEY_TRIGGERED) {
			if (msg.info == INFO::KEY_X) {
				switch (animationType) {
				case(ANIMATION_TIME_BASED):
					animationType = ANIMATION_EVENT_BASED;
					break;
				case(ANIMATION_EVENT_BASED):
					animationType = ANIMATION_TIME_BASED;
					break;
				default:
					break;
				}
			}
			if (msg.info == INFO::KEY_C) {

				ChangeAnimation(texData);
				ResizeOnChange(texData, sizeData);
			}

			//not in use - input will be registered under generic UpdateAnimation
			//if ((msg.info == INFO::KEY_V) && (aniData.animationType == Animation::ANIMATION_EVENT_BASED)) {
			//	AnimateOnKeyPress(aniData, texData);
			//}
		}
	}
}