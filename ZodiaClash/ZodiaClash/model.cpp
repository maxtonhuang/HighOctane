#include "model.h"
#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp> //for value_ptr
#include <iostream>

Model::Model() {
	matrix = glm::mat3{ 0.8,0,0,0,0.5,0,0.2,0,1 };
	color = glm::vec3{ 1,0,0 };
}

void Model::Update() {

}

void Model::Draw() {
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
		std::cout << "Uniform variable uModelToNDC doesn't exist!\n";
		std::cout << "Please check vertex shader!\n";
		std::exit(EXIT_FAILURE);
	}

	glDrawElements(GraphicsManager::GetVAOInfo().primitivetype, GraphicsManager::GetVAOInfo().drawcnt, GL_UNSIGNED_SHORT, NULL);
}
