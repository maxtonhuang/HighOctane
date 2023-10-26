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
*	@file		Texture.h
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

#pragma once
#include "GraphLib.h"
#include "FontLib.h"
#include <string>
#include <unordered_map>
#include <vector>

class Font; //forward declaration

const int channelnum = 4;

struct Texcoords {
	glm::vec2 bl{};
	glm::vec2 br{};
	glm::vec2 tl{};
	glm::vec2 tr{};
};

class Texture {
public:
	Texture();
	~Texture();
	void Init(char const* filepath, char const* filename); //Initialise a texture using the texture file path as input
	void Init(Font& font, const char* texname);
	void FreeTexture();		//Free the texture from OpenGL memory

	GLuint GetID();			//Get texture ID of texture
	bool IsActive();		//Does the texture object have a texture saved in OpenGL
	void CreateSpriteSheet(int row, int column, int spritenum); //Create a sprite sheet using the number of rows, columns and the total number of sprites in the sprite sheet

	int GetWidth(); //returns width of texture
	int GetHeight(); //returns height of texture
	int GetRowCount(); //returns number of rows of texture
	int GetColCount(); //returns number of columns of texture
	std::string GetName(); //returns name of texture as saved in texture managers data

	/*
	Get texture coordinates. Index is the index in the sprite sheet array while position corresponds as follows:
	pos 0 = bottom left coordinate
	pos 1 = bottom right coordinate
	pos 2 = top left coordinate
	pos 3 = top right coordinate
	*/
	glm::vec2 GetTexCoords(int index, int pos); //get texture coordinates. Index is the index in the sprite sheet array while position 
	int GetSheetSize(); //returns amount of sprites in sprite sheet
private:
	std::string name{}; //name of texture as stored in texture manager
	GLuint id{}; //texture id as stored in opengl
	int width{}; //width of individual sprite
	int height{}; //height of individual sprite
	int rowCount{}; //number of rows of texture
	int colCount{}; //number of columns of texture
	bool active{false}; //true if texture has been saved to OpenGL
	std::vector<Texcoords> texcoords; //array containing sprite coordinates for sprite sheet
};

class TextureManager {
public:
	~TextureManager(); //Calls Clear, in case of deletion without calling Clear
	Texture* Get(char const* texname);
	Texture* Add(const char* texpath, char const* texname); //Create a texture using the texture file path as input. If texture already exists, return the texture instead.
	Texture* Add(Font& font);
	Texture* AddSpriteSheet(const char* texname, int row, int col, int spritenum, const char* texpath = nullptr); //Create a sprite sheet using the number of rows, columns and the total number of sprites in the sprite sheet
	std::vector<std::string> GetTextureNames();
	void Clear(); //Removes all textures from OpenGL memory and empties the map
	std::unordered_map<std::string, Texture> data; //storage of textures
};
