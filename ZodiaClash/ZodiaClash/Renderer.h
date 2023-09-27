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
*	@file		Renderer.h
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
*	@brief		Renderer class to render for different shaders and draw types
*
*	Renderer to store vertices for each shader and draw type
*	Draw function to be called at the end of each frame to draw all vertices
*	to the screen
*
******************************************************************************/

#pragma once
#include "GraphLib.h"
#include "GraphicConstants.h"
#include "shaders.h"
#include "texture.h"

struct Vertex {
	glm::vec2 pos; //Vertex coordinates
	glm::vec3 col; //RGB colour
	glm::vec2 tex; //Texture coordinates
	GLfloat index;
};

class Renderer {
public:
	Renderer();
	~Renderer();

	void Initialize(char const* vertexshader, char const* fragmentshader, GLenum type);
	void Initialize(Shader shader, GLenum typ);

	Shader& ShaderProgram();

	void AddVertex(Vertex);
	void Draw();
	void FontDraw(GLuint texID);

	GLuint GetDrawCount();
	void CreateVAO();
	GLuint vao{};
	GLuint vbo{};
private:
	
	GLuint drawcount{};
	Vertex* data{};
	GLenum drawtype{};
	Shader shaderprogram{};
};
