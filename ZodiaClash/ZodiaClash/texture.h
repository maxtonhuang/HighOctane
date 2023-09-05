#pragma once
#include "graphics.h"

const int channelnum = 4;

class Texture {
public:
	Texture();
	~Texture();
	void Init(char const*);
	GLuint GetID();
private:
	GLuint id{};
	int width{};
	int height{};
	bool active{false};

	unsigned char* data;

};
