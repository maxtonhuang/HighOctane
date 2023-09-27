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
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong@digipen.edu
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
*	@brief		Model component used by graphics system
*
*	Model containing draw functions to add vertices to the graphics system to
*	draw at the end of the frame
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
	Model();
	void Update(Transform const& entity, Size const& size); //Update transforms for the model
	void Draw(Tex const& entity, Animation const& ani); //Add vertices to renderer
	void DrawOutline(); //Draw an outline around the model, for debugging purposes

	void SetAnimation(Animation& aniData, int index);		 //Set animation to frame of input index
	void AdvanceAnimation(Animation& aniData, Tex& texData); //Advance animation to next frame
	void ChangeAnimation(Animation& aniData, Tex& texData);
	void ResizeOnChange(Tex& texData, Size& sizeData);

	void AnimateOnInterval(Animation& aniData, Tex& texData);
	void AnimateOnKeyPress(Animation& aniData, Tex& texData);

	void UpdateAnimation(Animation& aniData, Tex& texData, Size& sizeData);
	void UpdateAnimationMC(Animation& aniData, Tex& texData, Size& sizeData);

private:
	glm::mat3 matrix{};
	glm::vec4 color{};

	//PRECOMPUTED VALUES
	glm::vec2 topleft{};
	glm::vec2 topright{};
	glm::vec2 botleft{};
	glm::vec2 botright{};
};
