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
	Animator();
	void SetAnimation(Animation& aniData, int index);		 //Set animation to frame of input index
	void AdvanceAnimation(Animation& aniData, Tex& texData); //Advance animation to next frame
	void ChangeAnimation(Tex& texData);	//Changes animation if >1 texVariant stored
	void ResizeOnChange(Tex& texData, Size& sizeData); //Resizes if ChangeAnimation() is called

	void AnimateOnInterval(Animation& aniData, Tex& texData); //time-based animation
	void AnimateOnKeyPress(Animation& aniData, Tex& texData); //event-based animation

	void UpdateAnimation(Animation& aniData, Tex& texData); // for all entities
	void UpdateAnimationMC(Animation& aniData, Tex& texData, Size& sizeData); // for MC entities
};