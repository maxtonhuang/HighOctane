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
	const int defaultWidth = 1920;
	const int defaultHeight = 1080;
	const float defaultWidthF = (float)defaultWidth;
	const float defaultHeightF = (float)defaultHeight;
	const float w = defaultWidthF / 2;
	const float h = defaultHeightF / 2;
	const float ar = defaultWidthF / defaultHeightF;
	const int vertexBufferSize = 20000; //number of vertices to buffer
	const int MAXTEXTURES = 1000;
	const int CIRCLE_SLICES = 100; //number of triangles in a circle
}
