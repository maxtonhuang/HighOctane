#pragma once
#include "shaders.h"
#include "model.h"

class GraphicsManager {
public:
	static void Init(); //initialise graphics manager at start of program
	static void Cleanup(); //delete memory used by graphics manager at end of program
	static void Draw(); //draw the screen, to be called every frame
	
	static bool WindowClosed(); //returns true if window is closed, else false

	//VAO info for draw functions in other classes
	struct VAOInfo {
		GLenum primitivetype;
		GLuint id;
		GLuint drawcnt;
	};
	static const VAOInfo& GetVAOInfo(); //get vao info (for other class graphics use)
	static const Shader& GetShader();
	static double GetWidth();
	static double GetHeight();

private:
	//Vertex struct for creating VAO with
	struct Vertex {
		glm::vec2 pos; //Vertex coordinates
		glm::vec3 col; //RGB colour
		glm::vec2 tex; //Texture coordinates
	};
	static void CreateVAO();

	static GLFWwindow* window;
	static int width;
	static int height;
	static Shader shaderprogram;
	static VAOInfo vao;
};

extern Model test_model;
extern Texture test_tex;

