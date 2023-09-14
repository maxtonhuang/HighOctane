#include "Model.h"
#include "Graphics.h"
#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp>
#include <iostream>

const float pi = 3.14159265358979323846;

std::vector<Model> modelList;

Model::Model() {
	color = glm::vec4{ 1,1,1,1 };
	pos = glm::vec2{ 0,0 };
	tex = nullptr;
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
	matrix = glm::mat3{ cos(rotationRadians) * x / graphics.GetWidth() ,-sin(rotationRadians) * x / graphics.GetHeight(),0,
		sin(rotationRadians) * y / graphics.GetWidth() , cos(rotationRadians) * y / graphics.GetHeight(),0,
		pos.x / graphics.GetWidth(),pos.y / graphics.GetHeight(),1 };
	//matrix = glm::mat3{0.5,0,0,0,0.5,0,0,0,1};
}

void Model::Draw() {
	if (tex != nullptr) {
		glBindTextureUnit(1, tex->GetID());
		//glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}
	else {
		glBindTextureUnit(1, 0);
	}

	glBindVertexArray(graphics.GetVAOInfo().id);

	//Pass matrix to shader
	GLint uniform_var_matrix = glGetUniformLocation(
		graphics.GetShader().GetHandle(), "uModelToNDC");
	if (uniform_var_matrix >= 0) {
		glUniformMatrix3fv(uniform_var_matrix, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	else {
		std::cout << "Uniform variable uModelToNDC doesn't exist!\n";
		std::cout << "Please check vertex shader!\n";
		//std::exit(EXIT_FAILURE);
	}

	GLint uniform_var_texmatrix = glGetUniformLocation(
		graphics.GetShader().GetHandle(), "uTexCoord");
	if (uniform_var_matrix >= 0) {
		glUniformMatrix3fv(uniform_var_texmatrix, 1, GL_FALSE, glm::value_ptr(tex->GetSheetMatrix(animation)));
	}
	else {
		std::cout << "Uniform variable uTexCoord doesn't exist!\n";
		std::cout << "Please check vertex shader!\n";
		//std::exit(EXIT_FAILURE);
	}

	GLint uniform_var_color = glGetUniformLocation(
		graphics.GetShader().GetHandle(), "uColor");
	if (uniform_var_color >= 0) {
		glUniform4fv(uniform_var_color, 1, glm::value_ptr(color));
	}
	else {
		std::cout << "Uniform variable uColor doesn't exist!\n";
		std::cout << "Please check fragment shader!\n";
		//std::exit(EXIT_FAILURE);
	}

	GLint uniform_var_tex = glGetUniformLocation(
		graphics.GetShader().GetHandle(), "uTex2d");
	if (uniform_var_tex >= 0) {
		glUniform1i(uniform_var_tex, 1);
	}
	else {
		std::cout << "Uniform variable uTex2d doesn't exist!\n";
		std::cout << "Please check fragment shader!\n";
		//std::exit(EXIT_FAILURE);
	}

	glDrawElements(graphics.GetVAOInfo().primitivetype, graphics.GetVAOInfo().drawcnt, GL_UNSIGNED_SHORT, NULL);
	//glDrawElements(GL_LINE_LOOP, graphics.GetVAOInfo().drawcnt, GL_UNSIGNED_SHORT, NULL);
}

void Model::DrawOutline() {
	graphics.DrawLineLoop(matrix);
	float tmp_width = cos(rotationRadians) * -width + sin(rotationRadians) * -height;
	float tmp_height = -sin(rotationRadians) * -width + cos(rotationRadians) * -height;
	graphics.DrawPoint(pos.x + tmp_width, pos.y + tmp_height);
	graphics.DrawPoint(pos.x - tmp_width, pos.y + tmp_height);
	graphics.DrawPoint(pos.x + tmp_width, pos.y - tmp_height);
	graphics.DrawPoint(pos.x - tmp_width, pos.y - tmp_height);
}

void Model::AttachTexture(Texture& input) {
	tex = &input;
	width = (float)tex->GetWidth();
	height = (float)tex->GetHeight();
}

void Model::AttachTexture(char const* input) {
	tex = texList.Add(input);
	if (tex->IsActive()) {
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

void Model::SetAnimation(int index) {
	animation = index;
}

void Model::AdvanceAnimation() {
	++animation;
	if (animation >= tex->GetSheetSize()) {
		animation = 0;
	}
}
