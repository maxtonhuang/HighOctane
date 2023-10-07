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
*	@file		GraphicConstants.h
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
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains constants used by the graphics system and related functions
*
******************************************************************************/

#include "GraphicConstants.h"

namespace GRAPHICS
{
	int defaultWidth;
	int defaultHeight;
	float defaultWidthF;
	float defaultHeightF;
	float w;
	float h;
	float ar; //aspect ratio

	void UpdateConstants(int input_w, int input_h) {
		defaultWidth = input_w;
		defaultHeight = input_h;
		defaultWidthF = (float)defaultWidth;
		defaultHeightF = (float)defaultHeight;
		w = defaultWidthF / 2;
		h = defaultHeightF / 2;
		ar = defaultWidthF / defaultHeightF;
	}
}

