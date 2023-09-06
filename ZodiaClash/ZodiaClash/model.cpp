#include "Model.h"
#include "Graphics.h"
#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp> //for value_ptr
#include <iostream>

Model::Model() {
	color = glm::vec3{ 1,1,1 };
	pos = glm::vec2{ 0,0 };
	tex = nullptr;
	matrix = glm::mat3{ 0.8,0,0,0,0.5,0,0.2,0,1 };
	scale = glm::vec2{ 1,1 };
	width = 0;
	height = 0;
}

Model::Model(Texture& input) {
	color = glm::vec3{ 1,1,1 };
	matrix = glm::mat3{ 0.8,0,0,0,0.5,0,0.2,0,1 };
	pos = glm::vec2{ 0,0 };
	scale = glm::vec2{ 1,1 };
	if (input.IsActive()) {
		tex = &input;
		width = (double)tex->GetWidth();
		height = (double)tex->GetHeight();
	}
	else {
		tex = nullptr;
		width = 0;
		height = 0;
	}
	//tex = new Texture("../Assets/Textures/test.png");
	//Texture test_texture("../Assets/Textures/test.png");
}

void Model::Update() {
	matrix = glm::mat3{ width / GraphicsManager::GetWidth(),0,0,
		0,height / GraphicsManager::GetHeight(),0,
		pos.x / GraphicsManager::GetWidth(),pos.y / GraphicsManager::GetHeight(),1 };
	//matrix = glm::mat3{0.5,0,0,0,0.5,0,0,0,1};
}

void Model::Draw() {
	if (tex != nullptr) {
		glBindTexture(GL_TEXTURE_2D, tex->GetID());
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	
	glBindVertexArray(GraphicsManager::GetVAOInfo().id);

	//Pass matrix to shader
	GLint uniform_var_matrix = glGetUniformLocation(
		(GraphicsManager::GetShader()).GetHandle(), "uModelToNDC");
	if (uniform_var_matrix >= 0) {
		glUniformMatrix3fv(uniform_var_matrix, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	else {
		std::cout << "Uniform variable uModelToNDC doesn't exist!\n";
		std::cout << "Please check vertex shader!\n";
		std::exit(EXIT_FAILURE);
	}

	GLint uniform_var_color = glGetUniformLocation(
		(GraphicsManager::GetShader()).GetHandle(), "uColor");
	if (uniform_var_color >= 0) {
		glUniform3fv(uniform_var_color, 1, glm::value_ptr(color));
	}
	else {
		std::cout << "Uniform variable uColor doesn't exist!\n";
		std::cout << "Please check fragment shader!\n";
		std::exit(EXIT_FAILURE);
	}

	GLint uniform_var_tex = glGetUniformLocation(
		(GraphicsManager::GetShader()).GetHandle(), "uTex2d");
	if (uniform_var_color >= 0) {
		glUniform3fv(uniform_var_color, 1, glm::value_ptr(color));
	}
	else {
		std::cout << "Uniform variable uColor doesn't exist!\n";
		std::cout << "Please check fragment shader!\n";
		std::exit(EXIT_FAILURE);
	}

	glDrawElements(GraphicsManager::GetVAOInfo().primitivetype, GraphicsManager::GetVAOInfo().drawcnt, GL_UNSIGNED_SHORT, NULL);
}

void Model::AttachTexture(Texture& input) {
	tex = &input;
	width = (double)tex->GetWidth();
	height = (double)tex->GetHeight();
}

void Model::AddPos(double x, double y) {
	pos.x += x;
	pos.y += y;
}
