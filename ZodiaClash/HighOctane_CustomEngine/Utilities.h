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
*	@file		Utilities.h
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
*	@date		3 November 2023
*
* *****************************************************************************
*
*	@brief		Function declarations to functions in Utilities.cpp
*
*	-
*
******************************************************************************/

#pragma once

#include "Components.h"
#include "vmath.h"
#include "model.h"

bool IsWithinObject(Model& model, vmath::Vector2 cursorPos);

vmath::Vector2 GetOffset(vmath::Vector2 point1, vmath::Vector2 point2);

bool IsNearby(vmath::Vector2 point1, vmath::Vector2 point2, float distance);
