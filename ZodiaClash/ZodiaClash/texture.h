#pragma once
#include "GraphLib.h"
#include <string>
#include <unordered_map>
#include <vector>

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
	void Init(char const*);
	void FreeTexture();

	GLuint GetID();
	bool IsActive();
	void CreateSpriteSheet(int row, int column, int spritenum);

	int GetWidth();
	int GetHeight();

	glm::vec2 GetTexCoords(int index, int pos);
	int GetSheetSize();
private:
	std::string name{};
	GLuint id{};
	int width{};
	int height{};
	bool active{false};
	std::vector<Texcoords> texcoords;
};

class TextureManager {
public:
	~TextureManager();
	void Initialize();
	Texture* Add(char const* texname);
	Texture* AddSpriteSheet(const char* texname, int row, int col, int spritenum);
	void Clear();
	std::unordered_map<std::string, Texture> data;
	GLuint arrayid;
};

extern TextureManager texList;
