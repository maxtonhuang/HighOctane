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
*	@file		Editing.h
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
*	@brief		Function declarations to functions in Editing.cpp
*
*	-
*
******************************************************************************/

#pragma once

#include "Components.h"
#include "model.h"

void UpdateProperties (Entity & entity, Name & name, Transform & transform, Size & size, Model * model, size_t layer_it);

enum AXIS {
	VERTICAL,
	HORIZONTAL
};

enum SNAP_ALIGN {
	
	SA_TOP_LEFT,
	SA_TOP,
	SA_TOP_RIGHT,
	SA_CENTER_LEFT,
	SA_CENTER,
	SA_CENTER_RIGHT,
	SA_BOTTOM_LEFT,
	SA_BOTTOM,
	SA_BOTTOM_RIGHT
};