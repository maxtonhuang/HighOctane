#include "Texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

TextureManager texList;

Texture::Texture() {
	active = false;
	sheetmatrix.emplace_back(glm::mat3{ 1,0,0,0,1,0,0,0,1 });
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

void Texture::CreateSpriteSheet(int row, int column, int spritenum) {
	int count = 0;
	std::vector<glm::mat3> newsheetmatrix;
	float colDist = 1.f / column;
	float rowDist = 1.f / row;
	width *= colDist;
	height *= rowDist;
	for (int i = 0; i < row; ++i) {
		for (int t = 0; t < column; ++t) {
			newsheetmatrix.emplace_back(glm::mat3{colDist, 0, 0, 0, rowDist, 0, colDist * t, rowDist * i, 1});
			++count;
			if (count >= spritenum) {
				sheetmatrix.swap(newsheetmatrix);
				return;
			}
		}
	}
	sheetmatrix.swap(newsheetmatrix);
	return;
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

glm::mat3& Texture::GetSheetMatrix(int index) {
	if (index >= sheetmatrix.size() || index < 0) {
		return sheetmatrix[0];
	}
	return sheetmatrix[index];
}

int Texture::GetSheetSize() {
	return (int)sheetmatrix.size();
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
