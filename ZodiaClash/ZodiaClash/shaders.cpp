#include "Shaders.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool Shader::Compile(std::vector<std::pair<GLenum, std::string>> vec) {
	if (handle <= 0) {
		handle = glCreateProgram();
	}
	
	for (auto& p : vec) {
		std::ifstream shaderfile(p.second, std::ifstream::in);
		if (!shaderfile) {
			std::cout << "Unable to find shader file " << p.second << "!\n";
			return false;
		}

		std::stringstream buffer;
		buffer << shaderfile.rdbuf();
		std::string shadercode{ buffer.str() };
		GLchar const* shadercodearray[] = { shadercode.c_str() };
		shaderfile.close();

		int shaderhandle;
		switch (p.first) {
		case GL_VERTEX_SHADER:
			shaderhandle = glCreateShader(GL_VERTEX_SHADER);
			break;
		case GL_FRAGMENT_SHADER:
			shaderhandle = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		default:
			return false;
		}

		glShaderSource(shaderhandle, 1, shadercodearray, NULL);
		glCompileShader(shaderhandle);

		int compileresult;
		glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compileresult);
		if (compileresult == GL_FALSE) {
			std::cout << "Shader compilation failed!\n";
			return false;
		}

		glAttachShader(handle, shaderhandle);
	}
	
	//Link the program
	glLinkProgram(handle);
	int linkresult;
	glGetShaderiv(handle, GL_LINK_STATUS, &linkresult);
	if (linkresult == GL_FALSE) {
		std::cout << "Unable to link shader!\n";
		return false;
	}

	//Validate the program
	//glLinkProgram(handle);
	glValidateProgram(handle);
	int validateresult;
	glGetShaderiv(handle, GL_VALIDATE_STATUS, &validateresult);
	if (validateresult == GL_FALSE) {
		std::cout << "Unable to validate shader!\n";
		return false;
	}

	return true;
}

void Shader::DeleteShader() {
	if (handle > 0) {
		glDeleteProgram(handle);
	}
}

void Shader::Use() {
	if (handle > 0) {
		glUseProgram(handle);
	}
}

void Shader::UnUse() {
	glUseProgram(0);
}

GLuint Shader::GetHandle() const {
	return handle;
}
