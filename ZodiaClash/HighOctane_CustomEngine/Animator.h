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
*	@file		Animator.h
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

#pragma once

#include "Components.h"

class Animator {
public:
	enum ANIMATION_TYPE {
		ANIMATION_NONE,
		ANIMATION_TIME_BASED,
		ANIMATION_EVENT_BASED,
		// PARTICLE
	};

	Animator();
	Animator(ANIMATION_TYPE aniType, float displayDur);

	ANIMATION_TYPE GetAnimationType() const;
	uint32_t GetFrameIndex() const;
	float GetFrameTimeElapsed() const;
	float GetFrameDisplayDuration() const;

	void SetAnimation(uint32_t index, Tex& texData);		 //Set animation to frame of input index
	void AdvanceAnimation(Tex& texData); //Advance animation to next frame
	void ChangeAnimation(Tex& texData);	//Changes animation if >1 texVariant stored
	void ResizeOnChange(Tex& texData, Size& sizeData); //Resizes if ChangeAnimation() is called

	void AnimateOnInterval(Tex& texData); //time-based animation
	void AnimateOnKeyPress(Tex& texData); //event-based animation

	void UpdateAnimation(Tex& texData); // for all entities
	void UpdateAnimationMC(Tex& texData, Size& sizeData); // for MC entities
private:
	ANIMATION_TYPE          animationType{};
	uint32_t                frameIndex{};
	float                   frameTimeElapsed{};
	float                   frameDisplayDuration{};
};