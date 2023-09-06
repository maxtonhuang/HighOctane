#include "texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

Texture::Texture() {
	active = false;
	data = nullptr;
}

void Texture::Init(char const* filename) {
	int filechannels;
	data = stbi_load(filename, &width, &height, &filechannels, channelnum);
	if (data == nullptr) {
		active = false;
		return;
	}
	else {
		active = true;
	}
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(id, 0, 0, 0, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
}

Texture::~Texture() {
	if (active) {
		stbi_image_free(data);
	}
}

GLuint Texture::GetID() {
	return id;
}

bool Texture::IsActive() {
	return active;
}

int Texture::GetWidth() {
	return width;
}

int Texture::GetHeight() {
	return height;
}
