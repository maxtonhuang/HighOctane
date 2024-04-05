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
	int viewportWidth;
	int viewportHeight;
	float defaultWidthF;
	float defaultHeightF;
	float w;
	float h;
	float ar; //aspect ratio

/*!***********************************************************************
\brief
Updates global constants related to the viewport size and calculates aspect ratios and other dimensions based on default and input values. This function is designed to be called whenever the viewport size changes, such as on window resize events, to maintain correct aspect ratios and dimensions for rendering.
\param input_w
The new width of the viewport.
\param input_h
The new height of the viewport.
\return
This function does not return a value. It updates global variables related to the viewport dimensions, default dimensions, half-dimensions, and the aspect ratio based on the provided input dimensions.
*************************************************************************/

	void UpdateConstants(int input_w, int input_h) {
		(void)input_w;
		(void) input_h;
		viewportWidth = input_w;
		viewportHeight = input_h;
		defaultWidthF = (float)defaultWidth;
		defaultHeightF = (float)defaultHeight;
		w = defaultWidthF / 2;
		h = defaultHeightF / 2;
		ar = defaultWidthF / defaultHeightF;
	}
}

