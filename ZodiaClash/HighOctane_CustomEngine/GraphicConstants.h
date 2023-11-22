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
#pragma once

namespace GRAPHICS {
	void UpdateConstants(int input_w, int input_h);

	extern int defaultWidth;
	extern int defaultHeight;
	extern float defaultWidthF;
	extern float defaultHeightF;
	extern float w;
	extern float h;
	extern float ar; //aspect ratio
	const int vertexBufferSize = 20000; //number of vertices to buffer
	const int MAXTEXTURES = 150;
	const int CIRCLE_SLICES = 100; //number of triangles in a circle
	const float DEBUG_CIRCLE_RADIUS = 50.f;
}
