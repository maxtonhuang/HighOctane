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

//forward declaration
class Texture; 

enum ModelType {
	GAMEPLAY, BACKGROUND, BACKGROUNDLOOP, UI
};

//Map to get enum names
const std::unordered_map<ModelType, std::string> modelTypeMap { {GAMEPLAY,"GAMEPLAY"},{BACKGROUND,"BACKGROUND"},{BACKGROUNDLOOP,"BACKGROUNDLOOP"},{UI,"UI"} };

class Model {
public:
	Model(ModelType = ModelType::GAMEPLAY, float bgScrollSpeed = 0.f); //default constructor of model, used to initialise matrix and color
	Model& operator= (const Model&); //assignment operator overload in order for pre-computed values to not be overwritten
	Model (const Model&); //copy constructor in order for pre-computed values to not be overwritten
	Model(int modelType, float bgScrollSpeed = 0.f); //default constructor of model, used to initialise matrix and color
	void Update(Transform const& entity, Size const& size); //Update transforms for the model
	void Draw(Tex* const entity); //Add vertices to renderer
	void DrawOutline(); //Draw an outline around the model, for debugging purposes
	void DrawOutlineRed(); //Draw an outline around the model, for debugging purposes
	void SetColor(float r, float g, float b); //Set color of model (colour bounds are between 0 and 1)
	void SetAlpha(float a); //Set alpha of model (alpha bounds are between 0 and 1)
	void AddAlpha(float a); //Add alpha of model (alpha bounds are between 0 and 1)
	float GetAlpha(); //Get alpha of model (alpha bounds are between 0 and 1)

	bool CheckTransformUpdated(Transform& transform, Size& size); //Check if transform was updated since last frame, returns true if transform was updated

	vmath::Vector2 GetMin() const; //returns minimum point in screen coordinates
	vmath::Vector2 GetMax() const; //returns maximum point in screen coordinates
	vmath::Vector2 GetRotPoint() const; //returns rotation point in screen coordinates
	vmath::Vector2 GetTopLeft() const; //returns top left point in screen coordinates
	vmath::Vector2 GetTopRight() const; //returns top right point in screen coordinates
	vmath::Vector2 GetBotLeft() const; //returns bottom left point in screen coordinates
	vmath::Vector2 GetBotRight() const; //returns bottom right point in screen coordinates
	vmath::Vector2 GetTop() const;
	vmath::Vector2 GetBot() const;
	vmath::Vector2 GetLeft() const;
	vmath::Vector2 GetRight() const;

	glm::vec4 GetColor() const; // retrieve color stored
	glm::vec4& GetColorRef(); //retrieve color reference

	ModelType type{};
	float backgroundScrollSpeed{}; //for parallax background

	bool GetMirror() const;
	bool& GetMirrorRef();
	void SetMirror(bool m);
private:
	glm::mat3 matrix{}; //matrix used for computing model verrtices
	glm::vec4 color{}; //color of the model
	bool mirror{false};

	//PRECOMPUTED VALUES FOR 4 VERTICES OF RECTANGULAR MODEL
	glm::vec2 topleft{};
	glm::vec2 topright{};
	glm::vec2 botleft{};
	glm::vec2 botright{};
	glm::vec2 rotationpoint{};
	glm::vec2 top{};
	glm::vec2 bot{};
	glm::vec2 left{};
	glm::vec2 right{};

	vmath::Vector2 minimum{};
	vmath::Vector2 maximum{};

	Transform previous; //used for check if previous is same as current
	Size previous_size; //used for check if previous is same as current
};

extern Renderer* previousRenderer; //FOR LAYERING
