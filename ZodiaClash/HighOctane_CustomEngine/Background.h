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
*	@file		Background.h
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
*	@date		19 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains class for background system used for parallax
*	background rendering
*
******************************************************************************/

#pragma once
#include "graphlib.h"
#include "GraphicConstants.h"
#include "ECS.h"

class BackgroundSystem {
public:
	BackgroundSystem();
	~BackgroundSystem();
	void Update();
	void SetFocusEntity(Entity entity);
	void AddBackground(float scroll);
private:
	float* scrolldata{};
	int count{};
	Entity focus{};
};
