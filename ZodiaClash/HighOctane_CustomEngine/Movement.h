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
*	@file		Movement.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Controls Main Character Movement in Exploration Scene
*
*	This file contains the enums for 8-axes direction calculation.
*
******************************************************************************/

#pragma once
#include "ECS.h"
#include "model.h"

//extern int keyInput;

enum OBJECT_DIRECTION {
	UP_DIRECTION = 21,
	UPRIGHT_DIRECTION = 22,
	RIGHT_DIRECTION = 12,
	DOWNRIGHT_DIRECTION = 2,
	DOWN_DIRECTION = 1,
	DOWNLEFT_DIRECTION = 0,
	LEFT_DIRECTION = 10,
	UPLEFT_DIRECTION = 20,
	NEUTRAL_DIRECTION = 11,
};

void UpdateMovement(Transform & transform, Model & model);
