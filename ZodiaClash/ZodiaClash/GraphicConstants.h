/*
This file contains constants used by the graphics system and related functions
*/
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
