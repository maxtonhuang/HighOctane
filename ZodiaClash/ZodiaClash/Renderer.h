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

	void Initialize(char const* vertexshader, char const* fragmentshader);

	void AddVertex(Vertex);
	void Draw(GLenum = GL_TRIANGLES);

	GLuint GetDrawCount();
	void CreateVAO();
private:
	GLuint vao{};
	GLuint vbo{};
	GLuint drawcount{};
	Vertex* data{};
	Shader shaderprogram{};
};
