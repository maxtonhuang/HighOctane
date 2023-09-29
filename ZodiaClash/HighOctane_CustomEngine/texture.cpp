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
*	@file		Texture.cpp
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
*	@brief		Texture class and texture manager
*
*	Texture class to store texture information such as texture id in OpenGL storage
*	as well as texture coordinates for sprite sheets
* 
*	Texture manager loads and unloads all textures as well as stores them in a central map
*
******************************************************************************/

#include "Texture.h"
#include "debugdiagnostic.h"
#include "GraphicConstants.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

TextureManager texList;

Texture::Texture() {
	active = false;
	Texcoords spriteCoords;
	spriteCoords.bl = glm::vec2{ 0, 1 };
	spriteCoords.br = glm::vec2{ 1, 1 };
	spriteCoords.tl = glm::vec2{ 0, 0 };
	spriteCoords.tr = glm::vec2{ 1, 0 };
	texcoords.emplace_back(spriteCoords);
}

Texture::~Texture() {
	//if (active) {
		//stbi_image_free(data);
	//}
	//glDeleteTextures(1, &id);
}

void Texture::Init(char const* filename) {
	int filechannels;
	std::string filepath{ "../Assets/Textures/" };
	filepath += filename;

	name = filename;

	unsigned char* data;
	data = stbi_load(filepath.c_str(), &width, &height, &filechannels, channelnum);
	if (data == nullptr) {
		active = false;
		ASSERT("Unable to find texture %s\n", filename);
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

void Texture::CreateSpriteSheet(int row, int column, int spritenum) {
	int count = 0;
	std::vector<Texcoords> newtexcoords;
	float colDist = 1.f / column;
	float rowDist = 1.f / row;
	width = (int)((float) width * colDist);
	height = (int)((float)height * rowDist);
	for (int i = 0; i < row; ++i) {
		for (int t = 0; t < column; ++t) {
			Texcoords spriteCoords;
			spriteCoords.bl = glm::vec2{ colDist * t, rowDist * (i + 1) };
			spriteCoords.br = glm::vec2{ colDist * (t + 1), rowDist * (i + 1) };
			spriteCoords.tl = glm::vec2{ colDist * t, rowDist * i };
			spriteCoords.tr = glm::vec2{ colDist * (t + 1), rowDist * i };
			newtexcoords.emplace_back(spriteCoords);
			++count;
			if (count >= spritenum) {
				texcoords.swap(newtexcoords);
				return;
			}
		}
	}
	texcoords.swap(newtexcoords);
	return;
}

void Texture::FreeTexture() {
	if (active) {
		glDeleteTextures(1, &id);
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

std::string Texture::GetName() {
	return name;
}

int Texture::GetSheetSize() {
	return (int)texcoords.size();
}

glm::vec2 Texture::GetTexCoords(int index, int pos) {
	switch (pos) {
	case 0:
		return texcoords[index].bl;
		break;
	case 1:
		return texcoords[index].br;
		break;
	case 2:
		return texcoords[index].tl;
		break;
	case 3:
		return texcoords[index].tr;
		break;
	}
	return glm::vec2{};
}

TextureManager::~TextureManager() {
	this->Clear();
}

Texture* TextureManager::Add(const char* texname) {
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

Texture* TextureManager::AddSpriteSheet(const char* texname, int row, int col, int spritenum) {
	if (data.count(texname)) {
		return &data[texname];
	}
	Texture temp;
	temp.Init(texname);
	if (temp.IsActive() == false) {
		return nullptr;
	}
	temp.CreateSpriteSheet(row, col, spritenum);
	data.emplace(texname, temp);
	return &data[texname];
}

void TextureManager::Clear() {
	for (auto& t : data) {
		t.second.FreeTexture();
	}
	data.clear();
}