#include "Texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

TextureManager texList;

Texture::Texture() {
	active = false;
}

void Texture::Init(char const* filename) {
	int filechannels;
	std::string filepath{ "../Assets/Textures/"};
	filepath += filename;

	name = filename;

	unsigned char* data;
	data = stbi_load(filepath.c_str(), &width, &height, &filechannels, channelnum);
	if (data == nullptr) {
		active = false;
		std::cout << "Unable to find texture " << filename << "!\n";
		return;
	}
	else {
		active = true;
	}
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(id, 0, 0, 0, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

Texture::~Texture() {
	//if (active) {
		//stbi_image_free(data);
	//}
	//glDeleteTextures(1, &id);
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

Texture* TextureManager::Add(const char* texname) {
	//std::string name = texname;
	if (data.count(texname)) {
		return &data[texname];
	}
	Texture temp;
	temp.Init(texname);
	if (temp.IsActive() == false) {
		return nullptr;
	}
	data.emplace(texname, temp);
	return &data[texname];
}
