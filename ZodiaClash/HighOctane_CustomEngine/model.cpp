/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		Model.cpp
*
*	@author		Foong Pun Yuen Nigel (Initial creation and draw functions)
*
*	@email		p.foong\@digipen.edu
*
*	@section	Section A
*
*	@date		04 September 2023
*
* *****************************************************************************
*
*	@brief		Model component used by graphics system
*
*	Model containing draw functions to add vertices to the graphics system to
*	draw at the end of the frame
* 
*	Model containing animation functions to update animation frame based on
*	values set in Animation component. Animation functions will be separated
*	into another class in M2.
*
******************************************************************************/

#include "Camera.h"
#include "message.h"
#include "Model.h"
#include "Graphics.h"
//#include "Components.h"
#include "ECS.h"
#include "EngineCore.h"
#include "Global.h"

#include <algorithm>
#include <iostream>

const float pi = 3.14159265358979323846f;
Renderer* previousRenderer{};

Model::Model(ModelType inputType, float bgScrollSpeed) { 
	color = glm::vec4{ 1,1,1,1 };
	matrix = glm::mat3{ 1,0,0,0,1,0,0,0,1 };
	type = inputType;
	backgroundScrollSpeed = bgScrollSpeed;
}

Model::Model(int modelType, float bgScrollSpeed) {
	color = glm::vec4{ 1,1,1,1 };
	matrix = glm::mat3{ 1,0,0,0,1,0,0,0,1 };
	type = static_cast<ModelType>(modelType);
	backgroundScrollSpeed = bgScrollSpeed;
}

Model& Model::operator= (const Model& rhs) {
	type = rhs.type;
	backgroundScrollSpeed = rhs.backgroundScrollSpeed;
	color = rhs.color;
	mirror = rhs.mirror;
	return *this;
}

Model::Model(const Model& rhs) {
	*this = rhs;
}

void Model::Update(Transform const& entity, Size const& size) {
	const float rotationLineLength{ 100 };

	if (type == ModelType::BACKGROUNDLOOP) {
		float x = camera.GetPos().x / GRAPHICS::w;
		float y = camera.GetPos().y / GRAPHICS::h;
		botleft = glm::vec2{ -1-x,-1-y };
		botright = glm::vec2{ 1-x,-1-y };
		topleft = glm::vec2{ -1-x,1-y };
		topright = glm::vec2{ 1-x,1-y };
		return;
	}
	float x = entity.scale * size.width;
	float y = entity.scale * size.height;
	matrix = glm::mat3{ cos(entity.rotation) * x / GRAPHICS::defaultWidthF ,-sin(entity.rotation) * x / GRAPHICS::defaultHeightF,0,
		sin(entity.rotation) * y / GRAPHICS::defaultWidthF , cos(entity.rotation) * y / GRAPHICS::defaultHeightF,0,
		entity.position.x / GRAPHICS::w,entity.position.y / GRAPHICS::h,1 };
	//vec3s are standard values for each corner of a 2x2 square
	glm::vec3 bottomleft3 = matrix * glm::vec3{ -1,-1,1 };
	glm::vec3 bottomright3 = matrix * glm::vec3{ 1,-1,1 };
	glm::vec3 topleft3 = matrix * glm::vec3{ -1,1,1 };
	glm::vec3 topright3 = matrix * glm::vec3{ 1,1,1 };
	botleft = glm::vec2{ bottomleft3.x,bottomleft3.y };
	botright = glm::vec2{ bottomright3.x,bottomright3.y };
	topleft = glm::vec2{ topleft3.x,topleft3.y };
	topright = glm::vec2{ topright3.x,topright3.y };
	top = glm::vec2{ (topright.x + topleft.x) / 2, (topright.y + topleft.y) / 2 };
	bot = glm::vec2{ (botright.x + botleft.x) / 2, (botright.y + botleft.y) / 2 };
	left = glm::vec2{ (botleft.x + topleft.x) / 2, (botleft.y + topleft.y) / 2 };
	right = glm::vec2{ (botright.x + topright.x) / 2, (botright.y + topright.y) / 2 };

	glm::vec2 rotationvector{ sin(entity.rotation),cos(entity.rotation) };
	glm::vec2 topmidpoint{ (topright.x + topleft.x) / 2, (topright.y + topleft.y) / 2 };
	rotationpoint.x = (topmidpoint.x * GRAPHICS::w) + (rotationvector.x * rotationLineLength);
	rotationpoint.y = (topmidpoint.y * GRAPHICS::h) + (rotationvector.y * rotationLineLength);

	minimum.x = fmin(botleft.x, botright.x);
	minimum.x = fmin(minimum.x, topleft.x);
	minimum.x = fmin(minimum.x, topright.x);
	minimum.y = fmin(botleft.y, botright.y);
	minimum.y = fmin(minimum.y, topleft.y);
	minimum.y = fmin(minimum.y, topright.y);

	maximum.x = fmax(botleft.x, botright.x);
	maximum.x = fmax(maximum.x, topleft.x);
	maximum.x = fmax(maximum.x, topright.x);
	maximum.y = fmax(botleft.y, botright.y);
	maximum.y = fmax(maximum.y, topleft.y);
	maximum.y = fmax(maximum.y, topright.y);

	minimum.x *= GRAPHICS::w;
	minimum.y *= GRAPHICS::h;
	maximum.x *= GRAPHICS::w;
	maximum.y *= GRAPHICS::h;
	if (type != UI) {
		minimum += camera.GetPos();
		maximum += camera.GetPos();
	}
}

void Model::Draw(Tex* const entity) {
	static Renderer* parallaxRenderer = &graphics.renderer["parallax"];
	static Renderer* textureRenderer = &graphics.renderer["texture"];
	static Renderer* staticRenderer = &graphics.renderer["static"];
	static Renderer* flatRenderer = &graphics.renderer["flat"];
	static Renderer* staticflatRenderer = &graphics.renderer["staticflat"];

	Renderer* renderer;
	if (entity != nullptr) {
		switch (type) {
		case ModelType::BACKGROUND:
		case ModelType::BACKGROUNDLOOP:
			renderer = parallaxRenderer;
			graphics.backgroundsystem.AddBackground(backgroundScrollSpeed);
			graphics.backgroundsystem.Update();
			break;
		case ModelType::UI:
			renderer = staticRenderer;
			break;
		default:
			renderer = textureRenderer;
		}
	}
	else {
		switch (type) {
		case ModelType::UI:
			renderer = staticflatRenderer;
			break;
		default:
			renderer = flatRenderer;
		}
	}

	if (renderer != previousRenderer) {
		if (previousRenderer != nullptr) {
			previousRenderer->Draw();
		}
		previousRenderer = renderer;
	}

	if (entity != nullptr && entity->tex != nullptr) {
		float texID{ (float)entity->tex->GetID() - 1.f };
		int frameIndex{entity->frameIndex};
		if (mirror) {
			renderer->AddVertex(Vertex{ botleft,color,	entity->tex->GetTexCoords(frameIndex,1), texID });
			renderer->AddVertex(Vertex{ botright,color, entity->tex->GetTexCoords(frameIndex,0), texID });
			renderer->AddVertex(Vertex{ topleft,color,	entity->tex->GetTexCoords(frameIndex,3), texID });
			renderer->AddVertex(Vertex{ topright,color, entity->tex->GetTexCoords(frameIndex,2), texID });
			renderer->AddVertex(Vertex{ botright,color, entity->tex->GetTexCoords(frameIndex,0), texID });
			renderer->AddVertex(Vertex{ topleft,color,	entity->tex->GetTexCoords(frameIndex,3), texID });
		}
		else {
			renderer->AddVertex(Vertex{ botleft,color,	entity->tex->GetTexCoords(frameIndex,0), texID });
			renderer->AddVertex(Vertex{ botright,color, entity->tex->GetTexCoords(frameIndex,1), texID });
			renderer->AddVertex(Vertex{ topleft,color,	entity->tex->GetTexCoords(frameIndex,2), texID });
			renderer->AddVertex(Vertex{ topright,color, entity->tex->GetTexCoords(frameIndex,3), texID });
			renderer->AddVertex(Vertex{ botright,color, entity->tex->GetTexCoords(frameIndex,1), texID });
			renderer->AddVertex(Vertex{ topleft,color,	entity->tex->GetTexCoords(frameIndex,2), texID });
		}
	}
	else {
		renderer->AddVertex(Vertex{ botleft,color });
		renderer->AddVertex(Vertex{ botright,color });
		renderer->AddVertex(Vertex{ topleft,color });
		renderer->AddVertex(Vertex{ topright,color });
		renderer->AddVertex(Vertex{ botright,color });
		renderer->AddVertex(Vertex{ topleft,color });
	}
}

void Model::DrawOutline() {
	Renderer* render{ nullptr };

	glm::vec2 topmidpoint{ (topright.x + topleft.x) / 2, (topright.y + topleft.y) / 2 };

	if (type == UI) {
		render = &graphics.renderer["staticline"];
	}
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawLine(topmidpoint.x * GRAPHICS::w, topmidpoint.y * GRAPHICS::h, rotationpoint.x, rotationpoint.y, 0.f, 1.f, 0.f, 1.f, render);

	if (type == UI) {
		render = &graphics.renderer["staticpoint"];
	}
	graphics.DrawPoint(topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(top.x * GRAPHICS::w, top.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(bot.x * GRAPHICS::w, bot.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(right.x * GRAPHICS::w, right.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(left.x * GRAPHICS::w, left.y * GRAPHICS::h, 0.f, 1.f, 0.f, 1.f, render);
	graphics.DrawPoint(rotationpoint.x, rotationpoint.y, 0.f, 1.f, 0.f, 1.f, render);
}

void Model::DrawOutlineRed() {
	Renderer* render{ nullptr };

	//glm::vec2 topmidpoint{ (topright.x + topleft.x) / 2, (topright.y + topleft.y) / 2 };

	if (type == UI) {
		render = &graphics.renderer["staticline"];
	}
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 1.f, 0.f, 0.f, 1.f, render);
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 1.f, 0.f, 0.f, 1.f, render);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 1.f, 0.f, 0.f, 1.f, render);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 1.f, 0.f, 0.f, 1.f, render);
}

bool Model::CheckTransformUpdated(Transform& transform, Size& size) {
	if (transform.position == previous.position && transform.rotation == previous.rotation && transform.scale == previous.scale && size.height == previous_size.height && size.width == previous_size.width) {
		return false;
	}
	previous = transform;
	previous_size = size;
	return true;
}

void Model::SetColor(float r, float g, float b) {
	color.r = r;
	color.g = g;
	color.b = b;
}

void Model::SetAlpha(float a) {
	color.a = a;
}

void Model::AddAlpha(float a) {
	color.a += a;
	if (color.a > 1.f) {
		color.a = 1.f;
	}
	if (color.a < 0.f) {
		color.a = 0.f;
	}
}

float Model::GetAlpha() {
	return color.a;
}

vmath::Vector2 Model::GetMin() const {
	return minimum;
}

vmath::Vector2 Model::GetMax() const {
	return maximum;
}

glm::vec4 Model::GetColor() const {
	return color;
}

glm::vec4& Model::GetColorRef() {
	return color;
}

/*!***********************************************************************
 \brief
  Retrieves the mirror state of the model. The mirror state indicates whether the model is mirrored along its vertical axis.
 \param
  This method does not take any parameters.
 \return
  Returns a boolean value indicating the mirror state of the model. True means the model is mirrored; false means it is not.
 *************************************************************************/
bool Model::GetMirror() const {
	return mirror;
}

/*!***********************************************************************
 \brief
  Provides a reference to the mirror state of the model. This allows for direct modification of the model's mirror state.
 \param
  This method does not take any parameters.
 \return
  Returns a reference to a boolean value representing the model's mirror state. This reference can be used to modify the mirror state directly.
 *************************************************************************/
bool& Model::GetMirrorRef() {
	return mirror;
}

/*!***********************************************************************
 \brief
  Sets the mirror state of the model. This method allows the model to be mirrored or unmirrored along its vertical axis.
 \param m
  A boolean value indicating the desired mirror state. True sets the model to be mirrored; false sets it to be unmirrored.
 \return
  This method does not return a value. It updates the mirror state of the model based on the provided boolean value.
 *************************************************************************/
void Model::SetMirror(bool m) {
	mirror = m;
}

vmath::Vector2 Model::GetRotPoint() const {
	return vmath::Vector2{ rotationpoint.x,rotationpoint.y };
}

vmath::Vector2 Model::GetTopLeft() const {
	if (type == UI) {
		return vmath::Vector2{ topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ topleft.x * GRAPHICS::w - camera.GetPos().x, topleft.y * GRAPHICS::h - camera.GetPos().y };
	}
}

vmath::Vector2 Model::GetTopRight() const {
	if (type == UI) {
		return vmath::Vector2{ topright.x * GRAPHICS::w, topright.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ topright.x * GRAPHICS::w - camera.GetPos().x, topright.y * GRAPHICS::h - camera.GetPos().y };
	}
}
vmath::Vector2 Model::GetBotLeft() const {
	if (type == UI) {
		return vmath::Vector2{ botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ botleft.x * GRAPHICS::w - camera.GetPos().x, botleft.y * GRAPHICS::h - camera.GetPos().y };
	}
}
vmath::Vector2 Model::GetBotRight() const {
	if (type == UI) {
		return vmath::Vector2{ botright.x * GRAPHICS::w, botright.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ botright.x * GRAPHICS::w - camera.GetPos().x, botright.y * GRAPHICS::h - camera.GetPos().y };
	}
}

/*!***********************************************************************
 \brief
  Retrieves the top position of the model in screen space. The position is adjusted based on whether the model is considered part of the UI or part of the game world, with UI elements being unaffected by the camera's position.
 \param
  This method does not take any parameters.
 \return
  Returns a vmath::Vector2 representing the top position of the model in screen space. If the model is of type UI, the position is directly scaled by the screen dimensions. Otherwise, it is adjusted based on the camera's current position.
 *************************************************************************/

vmath::Vector2 Model::GetTop() const {
	if (type == UI) {
		return vmath::Vector2{ top.x * GRAPHICS::w, top.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ top.x * GRAPHICS::w - camera.GetPos().x, top.y * GRAPHICS::h - camera.GetPos().y };
	}
}

/*!***********************************************************************
 \brief
  Retrieves the bottom position of the model in screen space. Similar to GetTop, this method adjusts the position based on the model's type (UI or game world) and the camera's position for non-UI elements.
 \param
  This method does not take any parameters.
 \return
  Returns a vmath::Vector2 representing the bottom position of the model in screen space. The calculation accounts for whether the model is part of the UI or the game world, adjusting for the camera's position as necessary.
 *************************************************************************/
vmath::Vector2 Model::GetBot() const {
	if (type == UI) {
		return vmath::Vector2{ bot.x * GRAPHICS::w, bot.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ bot.x * GRAPHICS::w - camera.GetPos().x, bot.y * GRAPHICS::h - camera.GetPos().y };
	}
}

/*!***********************************************************************
 \brief
  Retrieves the leftmost position of the model in screen space. The position is adjusted based on the model's type, with UI elements positioned based on screen dimensions and game world elements adjusted for the camera's position.
 \param
  This method does not take any parameters.
 \return
  Returns a vmath::Vector2 representing the leftmost position of the model in screen space, adjusted for the camera's position for non-UI elements.
 *************************************************************************/
vmath::Vector2 Model::GetLeft() const {
	if (type == UI) {
		return vmath::Vector2{ left.x * GRAPHICS::w, left.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ left.x * GRAPHICS::w - camera.GetPos().x, left.y * GRAPHICS::h - camera.GetPos().y };
	}
}

/*!***********************************************************************
 \brief
  Retrieves the rightmost position of the model in screen space. This method differentiates between UI and game world elements in its calculation, adjusting the position based on the camera's position for the latter.
 \param
  This method does not take any parameters.
 \return
  Returns a vmath::Vector2 representing the rightmost position of the model in screen space. For UI elements, the position is based on screen dimensions; for game world elements, it is adjusted for the camera's position.
 *************************************************************************/
vmath::Vector2 Model::GetRight() const {
	if (type == UI) {
		return vmath::Vector2{ right.x * GRAPHICS::w, right.y * GRAPHICS::h };
	}
	else {
		return vmath::Vector2{ right.x * GRAPHICS::w - camera.GetPos().x, right.y * GRAPHICS::h - camera.GetPos().y };
	}
}

