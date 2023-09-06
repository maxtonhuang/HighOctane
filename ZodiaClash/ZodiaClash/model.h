#pragma once

#include "graphlib.h"
#include "texture.h"

class Texture;

class Model {
public:
	Model();
	Model(Texture&);
	void Update();
	void Draw();
	void AttachTexture(Texture&);
	void AddPos(double x, double y);
private:
	glm::vec2 pos{}; //Position in screen coordinates
	glm::vec2 scale{}; //Scale
	double width{};
	double height{};

	Texture* tex{};
	glm::mat3 matrix{};
	glm::vec3 color{};
};
