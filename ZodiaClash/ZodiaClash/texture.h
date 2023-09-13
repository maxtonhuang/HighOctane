#pragma once
#include "GraphLib.h"
#include <string>
#include <map>
#include <vector>

const int channelnum = 4;

class Texture {
public:
	Texture();
	~Texture();
	void Init(char const*);
	GLuint GetID();
	bool IsActive();
	void CreateSpriteSheet(int row, int column, int spritenum);

	int GetWidth();
	int GetHeight();

	glm::mat3& GetSheetMatrix(int index);
	int GetSheetSize();
private:
	std::string name{};
	GLuint id{};
	int width{};
	int height{};
	bool active{false};
	std::vector<glm::mat3> sheetmatrix;
};

class TextureManager {
public:
	Texture* Add(char const* texname);
	Texture* AddSpriteSheet(const char* texname, int row, int col, int spritenum);
	std::map<std::string, Texture> data;
};

//I THINK THIS CURRENTLY CAUSES MEMORY LEAKS AS MAP MEMORY ISNT CLEARED
extern TextureManager texList;
