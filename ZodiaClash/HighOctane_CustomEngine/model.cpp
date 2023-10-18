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
*  	@co-author	Chua Zhen Rong (animation functions)
* 
*	@email		c.zhenrong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
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

#include "message.h"
#include "Model.h"
#include "Graphics.h"
//#include "Components.h"
#include "ECS.h"
#include "EngineCore.h"
#include "Global.h"

#include <iostream>

const float pi = 3.14159265358979323846f;

Model::Model(ModelType inputType, float bgScrollSpeed) { 
	color = glm::vec4{ 1,1,1,1 };
	matrix = glm::mat3{ 1,0,0,0,1,0,0,0,1 };
	type = inputType;
	backgroundScrollSpeed = bgScrollSpeed;
}

void Model::Update(Transform const& entity, Size const& size) {
	float x = entity.scale.x * size.width;
	float y = entity.scale.y * size.height;
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
}

void Model::Draw(Tex const& entity, Animator const& ani) {
	Renderer* renderer;
	if (entity.tex != nullptr) {
		switch (type) {
		case ModelType::BACKGROUND:
			renderer = &parallaxRenderer;
			graphics.backgroundsystem.AddBackground(backgroundScrollSpeed);
			break;
		default:
			renderer = &textureRenderer;
		}
	}
	else {
		
		renderer = &flatRenderer;
	}
	if (entity.tex != nullptr) {
		renderer->AddVertex(Vertex{ botleft,color,	entity.tex->GetTexCoords((int)ani.GetFrameIndex(),0), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ botright,color, entity.tex->GetTexCoords((int)ani.GetFrameIndex(),1), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topleft,color,	entity.tex->GetTexCoords((int)ani.GetFrameIndex(),2), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topright,color, entity.tex->GetTexCoords((int)ani.GetFrameIndex(),3), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ botright,color, entity.tex->GetTexCoords((int)ani.GetFrameIndex(),1), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topleft,color,	entity.tex->GetTexCoords((int)ani.GetFrameIndex(),2), (float)entity.tex->GetID() - 1.f });
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
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(topleft.x * GRAPHICS::w,topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
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

vmath::Vector2 Model::GetMin() {
	return vmath::Vector2{ botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h };
}

vmath::Vector2 Model::GetMax() {
	return vmath::Vector2{ topright.x * GRAPHICS::w, topright.y * GRAPHICS::h };
}
