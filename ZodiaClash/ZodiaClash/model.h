#pragma once

#include "GraphLib.h"
#include "Texture.h"
#include "Renderer.h"
#include "ECS.h"
#include <vector>

class Texture;

class Model {
public:
	Model();
	Model(Texture&);
	Model(char const*);

	void Update();
	void Update(Entity const& entity);
	void Draw(); //Draw the texture of the model
	void Draw(Entity const& entity);
	void DrawOutline(); //Draw an outline around the model, for debugging purposes

	void AttachTexture(Texture&);
	void AttachTexture(char const*);

	void SetDim(float w, float h, float r = 0.f);

	void SetPos(float x, float y);
	void AddPos(float x, float y);

	void SetRot(float rot); //INPUT IN DEGREES
	void AddRot(float rot); //INPUT IN DEGREES

	void SetScale(float x, float y);

	void SetAnimation(int index);
	void AdvanceAnimation();

private:
	glm::vec2 pos{}; //Position in screen coordinates
	glm::vec2 scale{}; //Scale
	float width{};
	float height{};
	float rotation{}; //rotation in degrees
	int animation{}; //animation index

	Texture* tex{};
	glm::mat3 matrix{};
	glm::vec4 color{};

	//PRECOMPUTED VALUES
	float rotationRadians{};
	glm::vec2 topleft{};
	glm::vec2 topright{};
	glm::vec2 botleft{};
	glm::vec2 botright{};
};

extern std::vector<Model> modelList;

extern Model test_circle1;
extern Model test_circle2;
extern Model test_circle3;
extern Model test_rect1;
extern Model test_rect2;
extern Model test_rect3;
