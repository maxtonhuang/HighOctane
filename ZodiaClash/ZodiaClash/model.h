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
	void SetPos(float x, float y);
	void AddPos(float x, float y);
	void SetRot(float rot); //INPUT IN DEGREES
	void AddRot(float rot); //INPUT IN DEGREES
	void SetScale(float x, float y);
private:
	glm::vec2 pos{}; //Position in screen coordinates
	glm::vec2 scale{}; //Scale
	float width{};
	float height{};
	float rotation{}; //rotation in degrees

	Texture* tex{};
	glm::mat3 matrix{};
	glm::vec4 color{};

	//PRECOMPUTED VALUES
	float rotationRadians{};
};

//THIS CURRENTLY CAUSES MEMORY LEAKS AS VECTOR MEMORY ISNT CLEARED
//ALSO ITS TEMPORARY, COMPONENT TO BE ADDED TO ENTITY LIST
extern std::vector<Model> modelList;
