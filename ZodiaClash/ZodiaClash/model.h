#pragma once

#include "GraphLib.h"
#include "Texture.h"

class Texture;

class Model {
public:
	Model();
	Model(Texture&);
	Model(char const*);
	void Update();
	void Draw();
	void AttachTexture(Texture&);
	void AttachTexture(char const*);
	void AddPos(double x, double y);
private:
	glm::vec2 pos{}; //Position in screen coordinates
	glm::vec2 scale{}; //Scale
	double width{};
	double height{};
	double rotation{};

	Texture* tex{};
	glm::mat3 matrix{};
	glm::vec4 color{};
};
