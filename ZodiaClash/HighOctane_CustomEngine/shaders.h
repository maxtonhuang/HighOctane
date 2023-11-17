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
*	@file		Shaders.h
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
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
*	@brief		Shader class
*
*	Shader class to compile and store shader information
*
******************************************************************************/

#pragma once
#include <glew-2.2.0/include/GL/glew.h>
#include <string>
#include <vector>

class Shader {
public:
	bool Compile(std::vector<std::pair<GLenum, std::string>> vec); //Compiles shader using input shader files, returns false if compile failure
	void DeleteShader(); //deletes shader from OpenGL system
	void Use(); //Use the shader for drawing
	GLuint GetHandle() const; //returns handle of the shader
private:
	GLuint handle{}; //handle of shader from OpenGL
};
