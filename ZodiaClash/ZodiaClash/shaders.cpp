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
*	@file		Shaders.cpp
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief		Functions for shader class
*
*	Shader class to compile and store shader information
*
******************************************************************************/

#include "Shaders.h"
#include "debugdiagnostic.h"
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
			DEBUG_PRINT("Unable to find shader file %s!\n", p.second.c_str());
			//std::cout << "Unable to find shader file " << p.second << "!\n";
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
			DEBUG_PRINT("Not valid shader type!\n");
			//std::cout << "Not valid shader type!\n";
			return false;
		}

		glShaderSource(shaderhandle, 1, shadercodearray, NULL);
		glCompileShader(shaderhandle);

		int compileresult;
		glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compileresult);
		if (compileresult == GL_FALSE) {
			std::cout << "Shader compilation failed!\n";
			GLchar* errorLog = new GLchar[1024];
			int length;
			glGetShaderInfoLog(shaderhandle, 1024, &length, &errorLog[0]);
			DEBUG_PRINT("%s\n",errorLog);
			//std::cout << errorLog << "\n";
			delete[] errorLog;
			return false;
		}

		glAttachShader(handle, shaderhandle);
	}
	
	//Link the program
	glLinkProgram(handle);
	int linkresult;
	glGetShaderiv(handle, GL_LINK_STATUS, &linkresult);
	if (linkresult == GL_FALSE) {
		DEBUG_PRINT("Unable to link shader!\n");
		//std::cout << "Unable to link shader!\n";
		return false;
	}

	//Validate the program
	glValidateProgram(handle);
	int validateresult;
	glGetShaderiv(handle, GL_VALIDATE_STATUS, &validateresult);
	if (validateresult == GL_FALSE) {
		DEBUG_PRINT("Unable to validate shader!\n");
		//std::cout << "Unable to validate shader!\n";
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
