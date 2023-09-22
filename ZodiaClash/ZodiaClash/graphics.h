#pragma once
#include "Shaders.h"
#include "Model.h"
#include "ECS.h"
#include "MMath.h"
#include "Renderer.h"

extern float g_dt;

class GraphicsManager : public System /*: virtual public ISystem*/ { // need to change
public:
	GraphicsManager();
	~GraphicsManager();
	void Initialize(int w, int h); //initialise graphics manager at start of program
	void Update();
	void Draw(); //draw the screen, to be called every frame
	std::string GetName();
	bool WindowClosed(); //returns true if window is closed, else false

	//DEBUG DRAW FUNCTIONS
	void DrawPoint(float x, float y, float pointsize = 10.f);
	void DrawLine(float x1, float y1, float x2, float y2);
	void DrawCircle(float x, float y, float radius);
	void DrawRect(float x1, float y1, float x2, float y2); //x1,y1 are bottom left. x2,y2 are top right
	void DrawOutline(float x1, float y1, float x2, float y2); //x1,y1 are bottom left. x2,y2 are top right

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
