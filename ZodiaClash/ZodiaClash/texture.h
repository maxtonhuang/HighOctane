#pragma once
#include "GraphLib.h"
#include <string>
#include <map>

const int channelnum = 4;

class Texture {
public:
	Texture();
	~Texture();
	void Init(char const*);
	GLuint GetID();
	bool IsActive();

	int GetWidth();
	int GetHeight();
private:
	std::string name{};
	GLuint id{};
	int width{};
	int height{};
	bool active{false};
};

class TextureManager {
public:
	Texture* Add(char const* texname);
	std::map<std::string, Texture> data;
};

extern TextureManager texList;
