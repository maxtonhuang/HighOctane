#include "message.h"
#include "Model.h"
#include "Graphics.h"
#include "Components.h"
#include "ECS.h"
#include "EngineCore.h"

#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp>
#include <iostream>

const float pi = 3.14159265358979323846;
extern float g_dt;
extern Mail mail;
extern ECS ecs;

std::vector<Model> modelList;
Model test_circle1;
Model test_circle2;
Model test_circle3;
Model test_rect1;
Model test_rect2;
Model test_rect3;

Model::Model() { // Use this
	color = glm::vec4{ 1,1,1,1 }; //done
	pos = glm::vec2{ 0,0 }; //done
	tex = nullptr; //done
	matrix = glm::mat3{ 0.8,0,0,0,0.5,0,0.2,0,1 };
	scale = glm::vec2{ 1,1 };
	width = 0;
	height = 0;
}

Model::Model(Texture& input) {
	color = glm::vec4{ 1,1,1,1 };
	matrix = glm::mat3{ 0.8,0,0,0,0.5,0,0.2,0,1 };
	pos = glm::vec2{ 0,0 };
	scale = glm::vec2{ 1,1 };
	if (input.IsActive()) {
		tex = &input;
		width = (float)tex->GetWidth();
		height = (float)tex->GetHeight();
	}
	else {
		tex = nullptr;
		width = 0;
		height = 0;
	}
}

Model::Model(char const* input) {
	color = glm::vec4{ 1,1,1,1 };
	matrix = glm::mat3{ 0.8,0,0,0,0.5,0,0.2,0,1 };
	pos = glm::vec2{ 0,0 };
	scale = glm::vec2{ 1,1 };
	tex = texList.Add(input);
	if (tex->IsActive()) {
		width = (float)tex->GetWidth();
		height = (float)tex->GetHeight();
	}
	else {
		width = 0;
		height = 0;
	}
}

void Model::Update() {
	float x = scale.x * width;
	float y = scale.y * height;
	matrix = glm::mat3{ cos(rotationRadians) * x / GRAPHICS::defaultWidthF ,-sin(rotationRadians) * x / GRAPHICS::defaultHeightF,0,
		sin(rotationRadians) * y / GRAPHICS::defaultWidthF , cos(rotationRadians) * y / GRAPHICS::defaultHeightF,0,
		pos.x / GRAPHICS::w,pos.y / GRAPHICS::h,1 };
	glm::vec3 bottomleft3 = matrix * glm::vec3{ -1,-1,1 };
	glm::vec3 bottomright3 = matrix * glm::vec3{ 1,-1,1 };
	glm::vec3 topleft3 = matrix * glm::vec3{ -1,1,1 };
	glm::vec3 topright3 = matrix * glm::vec3{ 1,1,1 };
	botleft = glm::vec2{ bottomleft3.x,bottomleft3.y };
	botright = glm::vec2{ bottomright3.x,bottomright3.y };
	topleft = glm::vec2{ topleft3.x,topleft3.y };
	topright = glm::vec2{ topright3.x,topright3.y };
}

void Model::Update(Transform const& entity, Size const& size) {
	float x = entity.scale.x * size.width;
	float y = entity.scale.y * size.height;
	matrix = glm::mat3{ cos(entity.rotation) * x / GRAPHICS::defaultWidthF ,-sin(entity.rotation) * x / GRAPHICS::defaultHeightF,0,
		sin(entity.rotation) * y / GRAPHICS::defaultWidthF , cos(entity.rotation) * y / GRAPHICS::defaultHeightF,0,
		entity.position.x / GRAPHICS::w,entity.position.y / GRAPHICS::h,1 };
	glm::vec3 bottomleft3 = matrix * glm::vec3{ -1,-1,1 };
	glm::vec3 bottomright3 = matrix * glm::vec3{ 1,-1,1 };
	glm::vec3 topleft3 = matrix * glm::vec3{ -1,1,1 };
	glm::vec3 topright3 = matrix * glm::vec3{ 1,1,1 };
	botleft = glm::vec2{ bottomleft3.x,bottomleft3.y };
	botright = glm::vec2{ bottomright3.x,bottomright3.y };
	topleft = glm::vec2{ topleft3.x,topleft3.y };
	topright = glm::vec2{ topright3.x,topright3.y };
}

void Model::Draw() {
	Renderer* renderer;
	if (tex != nullptr) {
		renderer = &textureRenderer;
	}
	else {
		renderer = &flatRenderer;
	}
	if (renderer->GetDrawCount() + 6 >= GRAPHICS::vertexBufferSize) {
		renderer->Draw();
	}
	if (tex != nullptr) {
		renderer->AddVertex(Vertex{ botleft,color, tex->GetTexCoords(animation,0), (float)tex->GetID() - 1 });
		renderer->AddVertex(Vertex{ botright,color, tex->GetTexCoords(animation,1), (float)tex->GetID() - 1 });
		renderer->AddVertex(Vertex{ topleft,color, tex->GetTexCoords(animation,2), (float)tex->GetID() - 1 });
		renderer->AddVertex(Vertex{ topright,color, tex->GetTexCoords(animation,3), (float)tex->GetID() - 1 });
		renderer->AddVertex(Vertex{ botright,color, tex->GetTexCoords(animation,1), (float)tex->GetID() - 1 });
		renderer->AddVertex(Vertex{ topleft,color, tex->GetTexCoords(animation,2), (float)tex->GetID() - 1 });
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

void Model::Draw(Tex const& entity, Animation const& ani) {
	Renderer* renderer;
	if (entity.tex != nullptr) {
		renderer = &textureRenderer;
	}
	else {
		renderer = &flatRenderer;
	}
	if (renderer->GetDrawCount() + 6 >= GRAPHICS::vertexBufferSize) {
		renderer->Draw();
	}
	if (entity.tex != nullptr) {
		renderer->AddVertex(Vertex{ botleft,color, entity.tex->GetTexCoords(ani.frameIndex,0), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ botright,color, entity.tex->GetTexCoords(ani.frameIndex,1), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topleft,color, entity.tex->GetTexCoords(ani.frameIndex,2), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topright,color, entity.tex->GetTexCoords(ani.frameIndex,3), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ botright,color, entity.tex->GetTexCoords(ani.frameIndex,1), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topleft,color, entity.tex->GetTexCoords(ani.frameIndex,2), (float)entity.tex->GetID() - 1.f });
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
	flatRenderer.Draw();
	flatRenderer.AddVertex(Vertex{ botleft, glm::vec3{1,1,1} });
	flatRenderer.AddVertex(Vertex{ botright, glm::vec3{1,1,1} });
	flatRenderer.AddVertex(Vertex{ topright, glm::vec3{1,1,1} });
	flatRenderer.AddVertex(Vertex{ topleft, glm::vec3{1,1,1} });
	flatRenderer.Draw(GL_LINE_LOOP);
	graphics.DrawPoint(topleft.x * GRAPHICS::w,topleft.y * GRAPHICS::h);
	graphics.DrawPoint(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h);
	graphics.DrawPoint(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h);
	graphics.DrawPoint(botright.x * GRAPHICS::w, botright.y * GRAPHICS::h);
}

void Model::SetDim(float w, float h, float r) {
	width = w;
	height = h;
	SetRot(r);
}

void Model::AttachTexture(Texture& input) {
	tex = &input;
	width = (float)tex->GetWidth();
	height = (float)tex->GetHeight();
}

void Model::AttachTexture(char const* input) {
	tex = texList.Add(input);
	if (tex != nullptr) {
		width = (float)tex->GetWidth();
		height = (float)tex->GetHeight();
	}
}

void Model::SetPos(float x, float y) {
	pos.x = x;
	pos.y = y;
}

void Model::AddPos(float x, float y) {
	pos.x += x;
	pos.y += y;
}

void Model::SetRot(float rot) {
	rotation = rot;
	rotationRadians = rotation * pi / 180;
}

void Model::AddRot(float rot) {
	rotation += rot;
	if (rotation > 360) {
		rotation -= 360;
	}
	if (rotation < 0) {
		rotation += 360;
	}
	rotationRadians = rotation * pi / 180;
}

void Model::SetScale(float x, float y) {
	scale.x = x;
	scale.y = y;
}

void Model::SetAnimation(Animation& data, int index) {
	data.frameIndex = index;
}

void Model::AdvanceAnimation(Animation& data, Tex& tex) {
	//data.frameIndex = (data.frameIndex + 1) % (tex->GetSheetSize());
	data.frameIndex = (data.frameIndex + 1) % tex.tex->GetSheetSize();
}

//TODO: determine texture set based on direction?

void Model::AnimateOnInterval(Animation& data, Tex& tex) {
	data.frameTimeElapsed += g_dt;
	//std::cout << data.frameTimeElapsed << "\n";
	if (data.frameTimeElapsed > data.frameDisplayDuration) {
		AdvanceAnimation(data, tex);
		data.frameTimeElapsed = 0.f;
	}
}

void Model::AnimateOnKeyPress(Animation& data, Tex& tex) {
	mail.CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MODEL, INFO::NONE, 0.f, 0.f);

	for (Postcard msg : mail.mailbox[ADDRESS::MODEL]) {
		if (msg.type == TYPE::KEY_DOWN) {
			if (msg.info == INFO::KEY_P) { AdvanceAnimation(data, tex); }
		}
	}
	mail.mailbox[ADDRESS::MODEL].clear();
}
