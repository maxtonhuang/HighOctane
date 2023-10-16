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

}

// to manually set animation to specified index frame
void Animator::SetAnimation(Animation& aniData, int index) {
	aniData.frameIndex = index;
}

// core function to cycle through animation frames
void Animator::AdvanceAnimation(Animation& aniData, Tex& texData) {
	aniData.frameIndex = (aniData.frameIndex + 1) % texData.tex->GetSheetSize();
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
void Animator::AnimateOnInterval(Animation& aniData, Tex& texData) {
	aniData.frameTimeElapsed += g_dt;
	if (aniData.frameTimeElapsed > aniData.frameDisplayDuration) {
		AdvanceAnimation(aniData, texData);
		aniData.frameTimeElapsed = 0.f;
	}
}

// called for event-based animations; advances on key input
void Animator::AnimateOnKeyPress(Animation& aniData, Tex& texData) {
	AdvanceAnimation(aniData, texData);
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
void Animator::UpdateAnimation(Animation& aniData, Tex& texData) {
	if ((aniData.animationType != Animation::ANIMATION_TIME_BASED) && (aniData.animationType != Animation::ANIMATION_EVENT_BASED)) { return; }

	if (aniData.animationType == Animation::ANIMATION_TIME_BASED) {
		AnimateOnInterval(aniData, texData);
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
			if ((msg.info == INFO::KEY_V) && (aniData.animationType == Animation::ANIMATION_EVENT_BASED)) {
				AnimateOnKeyPress(aniData, texData);
			}
		}
	}
}

// animation update function for all relevant MAIN CHARACTER entities
// called in MovementSystem
void Animator::UpdateAnimationMC(Animation& aniData, Tex& texData, Size& sizeData) {

	if ((aniData.animationType != Animation::ANIMATION_TIME_BASED) && (aniData.animationType != Animation::ANIMATION_EVENT_BASED)) { return; }

	// Check mailbox for input triggers
	Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::ANIMATOR, INFO::NONE, 0.f, 0.f);

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::ANIMATOR]) {
		if (msg.type == TYPE::KEY_TRIGGERED) {
			if (msg.info == INFO::KEY_X) {
				switch (aniData.animationType) {
				case(Animation::ANIMATION_TIME_BASED):
					aniData.animationType = Animation::ANIMATION_EVENT_BASED;
					break;
				case(Animation::ANIMATION_EVENT_BASED):
					aniData.animationType = Animation::ANIMATION_TIME_BASED;
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