#pragma once

#include "GraphLib.h"
#include "Texture.h"
#include <vector>

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
	void SetPos(double x, double y);
	void AddPos(double x, double y);
	void SetRot(double rot); //INPUT IN DEGREES
	void AddRot(double rot); //INPUT IN DEGREES
	void SetScale(double x, double y);
private:
	glm::vec2 pos{}; //Position in screen coordinates
	glm::vec2 scale{}; //Scale
	double width{};
	double height{};
	double rotation{}; //rotation in degrees

	Texture* tex{};
	glm::mat3 matrix{};
	glm::vec4 color{};

	//PRECOMPUTED VALUES
	double rotationRadians{};
};

extern std::vector<Model> modelList;
