#pragma once
#include "Shaders.h"
#include "Model.h"
#include "System.h"
#include "MMath.h"

class GraphicsManager : virtual public Architecture::ISystem {
public:
	GraphicsManager();
	~GraphicsManager();
	void Initialize(int w, int h); //initialise graphics manager at start of program
	void Update(float dt);
	void Draw(); //draw the screen, to be called every frame
	std::string GetName();
	bool WindowClosed(); //returns true if window is closed, else false

	void DrawPoint(float x, float y);
	void DrawLineLoop(const glm::mat3& input);

	//VAO info for draw functions in other classes
	struct VAOInfo {
		GLenum primitivetype;
		GLuint id;
		GLuint drawcnt;
	};
	const VAOInfo& GetVAOInfo(); //get vao info (for other class graphics use)
	const Shader& GetShader();
	float GetWidth();
	float GetHeight();
private:
	//Vertex struct for creating VAO with
	struct Vertex {
		glm::vec2 pos; //Vertex coordinates
		glm::vec3 col; //RGB colour
		glm::vec2 tex; //Texture coordinates
	};
	void CreateVAO(VAOInfo& vao, std::vector<Vertex>& vtx_array, std::vector<GLushort>& idx_vtx);

	GLFWwindow* window;
	int width;
	int height;
	Shader textureshaderprogram;
	Shader flatshaderprogram;
	VAOInfo vao;
	VAOInfo lineloopvao;
};

extern GraphicsManager graphics;

extern Model test_model;
extern Texture test_tex;

