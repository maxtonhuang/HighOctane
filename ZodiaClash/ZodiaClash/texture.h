#pragma once
#include "graphlib.h"

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
	GLuint id{};
	int width{};
	int height{};
	bool active{false};

	unsigned char* data;

};
