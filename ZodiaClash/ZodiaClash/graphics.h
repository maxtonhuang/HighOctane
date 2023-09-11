#pragma once
#include "Shaders.h"
#include "Model.h"
#include "System.h"

class GraphicsManager : virtual public Architecture::ISystem {
public:
	GraphicsManager();
	~GraphicsManager();
	void Initialize(int w, int h); //initialise graphics manager at start of program
	void Update(float dt);
	void Draw(); //draw the screen, to be called every frame
	std::string GetName();
	bool WindowClosed(); //returns true if window is closed, else false

	//VAO info for draw functions in other classes
	struct VAOInfo {
		GLenum primitivetype;
		GLuint id;
		GLuint drawcnt;
	};
	const VAOInfo& GetVAOInfo(); //get vao info (for other class graphics use)
	const Shader& GetShader();
	double GetWidth();
	double GetHeight();

private:
	//Vertex struct for creating VAO with
	struct Vertex {
		glm::vec2 pos; //Vertex coordinates
		glm::vec3 col; //RGB colour
		glm::vec2 tex; //Texture coordinates
	};
	void CreateVAO();

	GLFWwindow* window;
	int width;
	int height;
	Shader shaderprogram;
	VAOInfo vao;
};

extern GraphicsManager graphics;

extern Model test_model;
extern Texture test_tex;

