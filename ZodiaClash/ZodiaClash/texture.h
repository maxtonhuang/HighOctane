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

//I THINK THIS CURRENTLY CAUSES MEMORY LEAKS AS MAP MEMORY ISNT CLEARED
extern TextureManager texList;
