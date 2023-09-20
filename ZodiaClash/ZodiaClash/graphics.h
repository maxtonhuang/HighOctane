#pragma once
#include "Shaders.h"
#include "Model.h"
#include "System.h"
#include "MMath.h"
#include "Renderer.h"

class GraphicsManager /*: virtual public Architecture::ISystem*/ { // need to change
public:
	GraphicsManager();
	~GraphicsManager();
	void Initialize(int w, int h); //initialise graphics manager at start of program
	void Update(float g_dt);
	void Draw(); //draw the screen, to be called every frame
	std::string GetName();
	bool WindowClosed(); //returns true if window is closed, else false

	void DrawPoint(float x, float y, float pointsize = 10.f);
	void DrawLine(float x1, float y1, float x2, float y2);

	//VAO info for draw functions in other classes
	struct VAOInfo {
		GLenum primitivetype;
		GLuint id;
		GLuint drawcnt;
	};
	float GetWidth();
	float GetHeight();
private:
	GLFWwindow* window;
	int width;
	int height;
};

extern GraphicsManager graphics;

extern Model test_model;
extern Texture test_tex;

extern Renderer flatRenderer;
extern Renderer textureRenderer;
