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
#include "Font.h"
#include "AssetManager.h"

#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

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

void Texture::Init(char const* filepath, char const* filename) {
	int filechannels;

	name = filename;

	unsigned char* data;
	data = stbi_load(filepath, &width, &height, &filechannels, channelnum);
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

void Texture::Init(Font& font, const char* texname) {
	std::vector<Texcoords> newtexcoords;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	//for all printable characters [32, 127], 169 (copyright) and 174 (registered copyright)
	unsigned int fontWidth = 0;
	unsigned int fontHeight = 0;

	//first pass, get font sprite sheet size
	for (unsigned char c = 32; c < 175; c++)
	{
		//skip over characters not in intended range
		if (((c > 127) && (c < 169)) || ((c > 169) && (c < 174))) {
			continue;
		}

		// load character glyph
		if (FT_Load_Char(font.fontFace, c, FT_LOAD_RENDER))
		{
			DEBUG_PRINT("ERROR::FONT: Failed to load Glyph");
			continue;
		}
		// generate texture
		if (fontWidth < font.fontFace->glyph->bitmap.width) {
			fontWidth = font.fontFace->glyph->bitmap.width;
		}
		fontHeight += font.fontFace->glyph->bitmap.rows;
	}

	unsigned char* fontData = new unsigned char[fontWidth * fontHeight] {};

	// generate texture
	unsigned int texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_R8, fontWidth,
		fontHeight);
	unsigned int currHeight = 0;

	for (unsigned char c = 32; c < 175; c++)
	{
		//skip over characters not in intended range
		if (((c > 127) && (c < 169)) || ((c > 169) && (c < 174))) {
			continue;
		}

		// load character glyph
		if (FT_Load_Char(font.fontFace, c, FT_LOAD_RENDER))
		{
			DEBUG_PRINT("ERROR::FONT: Failed to load Glyph");
			continue;
		}
		
		//concatenate glyph data into spritesheet data
		for (unsigned int h = 0; h < font.fontFace->glyph->bitmap.rows; ++h) {
			for (unsigned int w = 0; w < font.fontFace->glyph->bitmap.width; ++w) {
				fontData[(currHeight + h) * fontWidth + w] = font.fontFace->glyph->bitmap.buffer[h * font.fontFace->glyph->bitmap.width + w];
			}
		}

		float t = (float)currHeight / fontHeight;
		float b = (float)(currHeight + font.fontFace->glyph->bitmap.rows) / fontHeight;
		float r = (float)font.fontFace->glyph->bitmap.width / fontWidth;
		Texcoords spriteCoords;
		spriteCoords.bl = glm::vec2{ 0.f, b };
		spriteCoords.br = glm::vec2{ r, b };
		spriteCoords.tl = glm::vec2{ 0.f, t };
		spriteCoords.tr = glm::vec2{ r, t };
		newtexcoords.emplace_back(spriteCoords);

		// store texture in Character map
		Character character = {
			nullptr,
			newtexcoords.size() - 1,
			glm::ivec2(font.fontFace->glyph->bitmap.width, font.fontFace->glyph->bitmap.rows),
			glm::ivec2(font.fontFace->glyph->bitmap_left, font.fontFace->glyph->bitmap_top),
			static_cast<size_t>(font.fontFace->glyph->advance.x)
		};
		font.characters.insert(std::pair<char, Character>(c, character));
		currHeight += font.fontFace->glyph->bitmap.rows;
	}
	glTextureSubImage2D(texture, 0, 0, 0, fontWidth,
		fontHeight,
		GL_RED,
		GL_UNSIGNED_BYTE,
		fontData);
	delete[] fontData;
	id = texture;
	active = true;
	name = texname;
	texcoords.swap(newtexcoords);
}

void Texture::CreateSpriteSheet(int row, int column, int spritenum) {
	int count = 0;
	std::vector<Texcoords> newtexcoords;
	rowCount = row;
	colCount = column;
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

int Texture::GetRowCount() {
	return rowCount;
}

int Texture::GetColCount() {
	return colCount;
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

Texture* TextureManager::Get(char const* texname) {
	if (data.count(texname)) {
		return &data[texname];
	}
	else {
		std::string texpath{ assetmanager.GetDefaultPath() + "Textures/" + texname };
		return Add(texpath.c_str(), texname);
	}
	ASSERT(1, "Unable to find texture!");
	return nullptr;
}

Texture* TextureManager::Add(const char* texpath, const char* texname) {
	if (data.count(texname)) {
		return &data[texname];
	}
	Texture temp;
	temp.Init(texpath, texname);
	if (temp.IsActive() == false) {
		return nullptr;
	}
	data.emplace(texname, temp);
	return &data[texname];
}

Texture* TextureManager::AddSpriteSheet(const char* texname, int row, int col, int spritenum, const char* texpath) {
	if (data.count(texname)) {
		data[texname].CreateSpriteSheet(row, col, spritenum);
		return &data[texname];
	}
	if (texpath == nullptr) {
		ASSERT(1, "Sprite sheet texture path not provided!");
		return nullptr;
	}
	Texture temp;
	temp.Init(texpath, texname);
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

Texture* TextureManager::Add(Font& font) {
	std::string texname = { "font0" };
	int count = 0;
	while (data.count(texname)) {
		++count;
		std::stringstream namestream{};
		namestream << "font" << count;
		texname = namestream.str();
	}
	Texture temp;
	temp.Init(font, texname.c_str());
	data.emplace(texname, temp);

	for (auto& c : font.characters) {
		c.second.textureID = &data[texname];
	}

	return &data[texname];
}

std::vector<std::string> TextureManager::GetTextureNames() {
	std::vector<std::string> output;
	for (auto& texture : data) {
		if (texture.first.find("font") != std::string::npos) {
			continue;
		}
		if (texture.second.GetSheetSize() == 1) {
			output.push_back(texture.first);
		}
		else {
			size_t pos = texture.first.find_last_of(".");
			std::string tmp{ texture.first.substr(0,pos)};
			tmp += ".spritesheet";
			output.push_back(tmp);
		}
	}
	return output;
}

void TextureManager::SetWindowIcon(GLFWwindow* window, std::string iconpath) {
	GLFWimage images[1]; images[0].pixels = stbi_load(iconpath.c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels
	glfwSetWindowIcon(window, 1, images); 
	stbi_image_free(images[0].pixels);
}
