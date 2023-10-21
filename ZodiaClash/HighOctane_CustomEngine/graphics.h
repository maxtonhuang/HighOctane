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
*	@file		Graphics.h
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
*	@brief		Graphics system of the engine
*
*	This file contains functions used in the main graphic system of the engine
*   This includes the initialisation and destruction of the graphics system as well
*   as the main draw call of the system
*
******************************************************************************/

#pragma once
#include "Shaders.h"
#include "Model.h"
#include "Animator.h"
#include "ECS.h"
#include "VMath.h"
#include "MMath.h"
#include "Renderer.h"
#include "Global.h"
#include "Font.h"
#include "Camera.h"
#include "Viewport.h"
#include "FrameBuffer.h"

extern float g_dt;
 class GraphicsManager {
public:
	Viewport viewport{}; //viewport class
	FrameBuffer framebuffer{};
public:
	GraphicsManager();
	~GraphicsManager();
	void Initialize(int w, int h); //initialise graphics manager at start of program
	void Update();
	void Draw(); //draw game world to the screen
	void EndDraw(); //TO BE CALLED AT END OF EVERY FRAME, swaps buffers
	std::string GetName();
	bool WindowClosed(); //returns true if window is closed, else false
	void Fullscreen(bool); //true to set fullscreen on, false to set fullscreen off
	GLFWwindow* GetWindow(); //returns window of graphics system

	void DrawLabel(std::string labelText, std::string fontFamily, std::string fontVariant, float relFontSize, Vec2 relTextPos, glm::vec4 color);

	//DEBUG DRAW FUNCTIONS
	void DrawPoint(float x, float y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
	void DrawLine(float x1, float y1, float x2, float y2, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
	void DrawCircle(float x, float y, float radius, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
	void DrawRect(float x1, float y1, float x2, float y2, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f); //x1,y1 are bottom left. x2,y2 are top right
	void DrawOutline(float x1, float y1, float x2, float y2, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f); //x1,y1 are bottom left. x2,y2 are top right

	void UpdateWindow();
	float GetWindowWidth();
	float GetWindowHeight();

private:
	GLFWwindow* window;
	int width;
	int height;
};

extern GraphicsManager graphics;

//extern Model test_model;
extern Texture test_tex;

//main renderer of the engine
extern Renderer textureRenderer;

//DEBUG RENDERERS
extern Renderer flatRenderer;
extern Renderer pointRenderer;
extern Renderer lineRenderer;
extern Renderer lineloopRenderer;
extern Renderer rectRenderer;
extern Renderer circleRenderer;
