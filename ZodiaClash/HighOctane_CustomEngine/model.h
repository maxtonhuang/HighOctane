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
*	@file		Model.h
*
*	@author		Foong Pun Yuen Nigel (Initial creation and draw functions)
*
*	@email		p.foong\@digipen.edu
* 
*	@co-author	Chua Zhen Rong (animation functions)
* 
*	@email		c.zhenrong\@digipen.edu 
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		04 September 2023
*
* *****************************************************************************
*
*	@brief		Model component used by graphics system
*
*	Model containing draw functions to add vertices to the graphics system to
*	draw at the end of the frame
* 
*	Model containing animation functions to update animation frame based on
*	values set in Animation component. Animation functions will be separated
*	into another class in M2.
*
******************************************************************************/

#pragma once

#include "Components.h"
#include "GraphLib.h"
#include "Animator.h"
#include "Texture.h"
#include "Renderer.h"
#include "ECS.h"
#include "vmath.h"
#include <vector>

class Texture; //forward declaration

enum class ModelType {
	GAMEPLAY, BACKGROUND, UI
};

class Model {
public:
	Model(ModelType = ModelType::GAMEPLAY, float bgScrollSpeed = 0.f); //default constructor of model, used to initialise matrix and color
	void Update(Transform const& entity, Size const& size); //Update transforms for the model
	void Draw(Tex const& entity, Animator const& ani); //Add vertices to renderer
	void DrawOutline(); //Draw an outline around the model, for debugging purposes

	void SetColor(float r, float g, float b); //Set color of model (colour bounds are between 0 and 1)
	void SetAlpha(float a); //Set alpha of model (alpha bounds are between 0 and 1)
	void AddAlpha(float a); //Add alpha of model (alpha bounds are between 0 and 1)

	bool CheckTransformUpdated(Transform& transform, Size& size); //Check if transform was updated since last frame, returns true if transform was updated

	vmath::Vector2 GetMin(); //returns minimum point in screen coordinates
	vmath::Vector2 GetMax(); //returns maximum point in screen coordinates

	glm::vec4 GetColor(); // retrieve color stored

private:
	glm::mat3 matrix{}; //matrix used for computing model verrtices
	glm::vec4 color{}; //color of the model
	ModelType type{};

	float backgroundScrollSpeed{}; //for parallax background

	//PRECOMPUTED VALUES FOR 4 VERTICES OF RECTANGULAR MODEL
	glm::vec2 topleft{};
	glm::vec2 topright{};
	glm::vec2 botleft{};
	glm::vec2 botright{};

	Transform previous{}; //used for check if previous is same as current
	Size previous_size{}; //used for check if previous is same as current
};
