#pragma once
#include "graphlib.h"
#include "GraphicConstants.h"

class FrameBuffer {
public:
	void Initialize();
	void Bind();
	void Unbind();

	unsigned int GetTextureID();
private:
	GLuint id; //framebuffer id
	GLuint textureid; //resultant texture id
};
