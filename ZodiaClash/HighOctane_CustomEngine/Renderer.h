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
	glm::vec4 col; //RGB colour
	glm::vec2 tex; //Texture coordinates
	GLfloat index; //Texture index
};

class Renderer {
public:
	Renderer(); //default constructor of Renderer, creates vertex buffer
	~Renderer();//destroys vertex buffer

	void Initialize(char const* vertexshader, char const* fragmentshader, GLenum type); //create the renderer using 2 input shader files and an OpenGL draw type
	void Initialize(Shader shader, GLenum type); //creates the renderer using a compiled shader and an OpenGL draw type

	Shader& ShaderProgram(); //returns shader program

	void AddVertex(Vertex); //Add vertex to the buffer
	void Draw(); //To be called at end of every frame or if buffer is filled
	void DrawFrameBuffer();
	void FontDraw(GLuint texID);
	
	void UpdateUniformMatrix3fv(char const* uniform_name, glm::mat3* matrix); //update uniform matrix for a shader

	GLuint GetDrawCount(); //Gets current amount of vertices in the buffer
	void CreateVAO(); //Creates VAO and buffers for storage
private:
	GLuint vao{}; //VAO handle
	GLuint vbo{}; //VBO handle
	GLuint drawcount{}; //amount of vertices currently drawn
	Vertex* data{}; //storage of vertices
	GLenum drawtype{}; //draw type of the renderer
	Shader shaderprogram{}; //shader used by renderer
	GLuint objvertsize{}; //amount of vertices per screen object
};
