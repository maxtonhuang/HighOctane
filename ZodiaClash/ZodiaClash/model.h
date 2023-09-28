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
#include "Texture.h"
#include "Renderer.h"
#include "ECS.h"
#include <vector>

class Texture;

class Model {
public:
	Model(); //default constructor of model, used to initialise matrix and color
	void Update(Transform const& entity, Size const& size); //Update transforms for the model
	void Draw(Tex const& entity, Animation const& ani); //Add vertices to renderer
	void DrawOutline(); //Draw an outline around the model, for debugging purposes

	void SetAnimation(Animation& aniData, int index);		 //Set animation to frame of input index
	void AdvanceAnimation(Animation& aniData, Tex& texData); //Advance animation to next frame
	void ChangeAnimation(Animation& aniData, Tex& texData);
	void ResizeOnChange(Tex& texData, Size& sizeData);

	void AnimateOnInterval(Animation& aniData, Tex& texData);
	void AnimateOnKeyPress(Animation& aniData, Tex& texData);

	void UpdateAnimation(Animation& aniData, Tex& texData);
	void UpdateAnimationMC(Animation& aniData, Tex& texData, Size& sizeData);

	bool CheckTransformUpdated(Transform& transform, Size& size); //Check if transform was updated since last frame, returns true if transform was updated

private:
	glm::mat3 matrix{}; //matrix used for computing model verrtices
	glm::vec4 color{}; //color of the model

	//PRECOMPUTED VALUES FOR 4 VERTICES OF RECTANGULAR MODEL
	glm::vec2 topleft{};
	glm::vec2 topright{};
	glm::vec2 botleft{};
	glm::vec2 botright{};

	Transform previous{}; //used for check if previous is same as current
	Size previous_size{}; //used for check if previous is same as current
};
