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
*	@file		Model.cpp
*
*	@author		Foong Pun Yuen Nigel (Initial creation and draw functions)
*
*	@email		p.foong\@digipen.edu
* 
*  	@co-author	Chua Zhen Rong (animation functions)
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

#include "message.h"
#include "Model.h"
#include "Graphics.h"
#include "Components.h"
#include "ECS.h"
#include "EngineCore.h"
#include "Global.h"

#include <iostream>

const float pi = 3.14159265358979323846f;

Model::Model() { 
	color = glm::vec4{ 1,1,1,1 };
	matrix = glm::mat3{ 1,0,0,0,1,0,0,0,1 };
}

void Model::Update(Transform const& entity, Size const& size) {
	float x = entity.scale.x * size.width;
	float y = entity.scale.y * size.height;
	matrix = glm::mat3{ cos(entity.rotation) * x / GRAPHICS::defaultWidthF ,-sin(entity.rotation) * x / GRAPHICS::defaultHeightF,0,
		sin(entity.rotation) * y / GRAPHICS::defaultWidthF , cos(entity.rotation) * y / GRAPHICS::defaultHeightF,0,
		entity.position.x / GRAPHICS::w,entity.position.y / GRAPHICS::h,1 };

	//vec3s are standard values for each corner of a 2x2 square
	glm::vec3 bottomleft3 = matrix * glm::vec3{ -1,-1,1 };
	glm::vec3 bottomright3 = matrix * glm::vec3{ 1,-1,1 };
	glm::vec3 topleft3 = matrix * glm::vec3{ -1,1,1 };
	glm::vec3 topright3 = matrix * glm::vec3{ 1,1,1 };
	botleft = glm::vec2{ bottomleft3.x,bottomleft3.y };
	botright = glm::vec2{ bottomright3.x,bottomright3.y };
	topleft = glm::vec2{ topleft3.x,topleft3.y };
	topright = glm::vec2{ topright3.x,topright3.y };
}

void Model::Draw(Tex const& entity, Animation const& ani) {
	Renderer* renderer;
	if (entity.tex != nullptr) {
		renderer = &textureRenderer;
	}
	else {
		renderer = &flatRenderer;
	}
	if (entity.tex != nullptr) {
		renderer->AddVertex(Vertex{ botleft,color, entity.tex->GetTexCoords(ani.frameIndex,0), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ botright,color, entity.tex->GetTexCoords(ani.frameIndex,1), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topleft,color, entity.tex->GetTexCoords(ani.frameIndex,2), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topright,color, entity.tex->GetTexCoords(ani.frameIndex,3), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ botright,color, entity.tex->GetTexCoords(ani.frameIndex,1), (float)entity.tex->GetID() - 1.f });
		renderer->AddVertex(Vertex{ topleft,color, entity.tex->GetTexCoords(ani.frameIndex,2), (float)entity.tex->GetID() - 1.f });
	}
	else {
		renderer->AddVertex(Vertex{ botleft,color });
		renderer->AddVertex(Vertex{ botright,color });
		renderer->AddVertex(Vertex{ topleft,color });
		renderer->AddVertex(Vertex{ topright,color });
		renderer->AddVertex(Vertex{ botright,color });
		renderer->AddVertex(Vertex{ topleft,color });
	}
}

void Model::DrawOutline() {
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawLine(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, topleft.x * GRAPHICS::w, topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawLine(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(topleft.x * GRAPHICS::w,topleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(topright.x * GRAPHICS::w, topright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(botleft.x * GRAPHICS::w, botleft.y * GRAPHICS::h, 0.f, 1.f, 0.f);
	graphics.DrawPoint(botright.x * GRAPHICS::w, botright.y * GRAPHICS::h, 0.f, 1.f, 0.f);
}

bool Model::CheckTransformUpdated(Transform& transform, Size& size) {
	if (transform.position == previous.position && transform.rotation == previous.rotation && transform.scale == previous.scale && size.height == previous_size.height && size.width == previous_size.width) {
		return false;
	}
	previous = transform;
	previous_size = size;
	return true;
}

/* ----------------------------------------------------------------------------
THE FOLLOWING FUNCTIONS ARE TO BE MOVED INTO A DIFFERENT ANIMATOR CLASS
---------------------------------------------------------------------------- */
// to manually set animation to specified index frame
void Model::SetAnimation(Animation& aniData, int index) {
	aniData.frameIndex = index;
}

// core function to cycle through animation frames
void Model::AdvanceAnimation(Animation& aniData, Tex& texData) {
	aniData.frameIndex = (aniData.frameIndex + 1) % texData.tex->GetSheetSize();
}

// changes animation sprite if model has more than 1 texVariant stored
void Model::ChangeAnimation(Tex& texData) {
	// Update sprite
	if (texData.texVariants.size() == 0) {
		return;
	}
	texData.texVariantIndex = (texData.texVariantIndex + 1) % texData.texVariants.size();
	texData.tex = texData.texVariants.at(texData.texVariantIndex);
}

// called after ChangeAnimation to account for change in texture dimensions
void Model::ResizeOnChange(Tex& texData, Size& sizeData) {

	sizeData.width = (float)texData.tex->GetWidth();
	sizeData.height = (float)texData.tex->GetHeight();
}

// called for time-based animations; advances to next frame at set intervals (frameDisplayDuration)
void Model::AnimateOnInterval(Animation& aniData, Tex& texData) {
	aniData.frameTimeElapsed += g_dt;
	if (aniData.frameTimeElapsed > aniData.frameDisplayDuration) {
		AdvanceAnimation(aniData, texData);
		aniData.frameTimeElapsed = 0.f;
	}
}

// called for event-based animations; advances on key input
void Model::AnimateOnKeyPress(Animation& aniData, Tex& texData) {
	AdvanceAnimation(aniData, texData);
}


/********************************************************************************
* 3 kinds of key input handling:
* - KEY_X --> animationType toggling
*	>> Note: ONLY for main character entities!
* - KEY_C --> AnimationChange()
*	>> Note: ONLY for main character entities!
* - KEY_V --> AnimateOnKeyPress()
*	>> Note: (animationType == ANIMATION_EVENT_BASED)
*	>> Note: for ALL entities!
********************************************************************************/

// animation update function for all relevant entities
// called in ModelSystem
void Model::UpdateAnimation(Animation& aniData, Tex& texData) {
	if ((aniData.animationType != Animation::ANIMATION_TIME_BASED) && (aniData.animationType != Animation::ANIMATION_EVENT_BASED)) { return; }

	if (aniData.animationType == Animation::ANIMATION_TIME_BASED) { 
		AnimateOnInterval(aniData, texData); 
		//return; 
	}

	// Check mailbox for input triggers
	Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MODEL, INFO::NONE, 0.f, 0.f);

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::MODEL]) {
		if (msg.type == TYPE::KEY_TRIGGERED) {
			// not in use - sizeData not needed as only MC can change animation; to reinstate when conditions change
			//if (msg.info == INFO::KEY_C) {
			//	ChangeAnimation(aniData, texData);
			//	ResizeOnChange(texData, sizeData);
			//}
			if ((msg.info == INFO::KEY_V) && (aniData.animationType == Animation::ANIMATION_EVENT_BASED)) {
				AnimateOnKeyPress(aniData, texData);
			}
		}
	}
}

// animation update function for all relevant MAIN CHARACTER entities
// called in MovementSystem
void Model::UpdateAnimationMC(Animation& aniData, Tex& texData, Size& sizeData) {

	if ((aniData.animationType != Animation::ANIMATION_TIME_BASED) && (aniData.animationType != Animation::ANIMATION_EVENT_BASED)) { return; }

	// Check mailbox for input triggers
	Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MODEL, INFO::NONE, 0.f, 0.f);

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::MODEL]) {
		if (msg.type == TYPE::KEY_TRIGGERED) {
			if (msg.info == INFO::KEY_X) {
				switch (aniData.animationType) {
				case(Animation::ANIMATION_TIME_BASED):
					aniData.animationType = Animation::ANIMATION_EVENT_BASED;
					break;
				case(Animation::ANIMATION_EVENT_BASED):
					aniData.animationType = Animation::ANIMATION_TIME_BASED;
					break;
				}
			}
			if (msg.info == INFO::KEY_C) {

				ChangeAnimation(texData);
				ResizeOnChange(texData, sizeData);
			}

			//not in use - input will be registered under generic UpdateAnimation
			//if ((msg.info == INFO::KEY_V) && (aniData.animationType == Animation::ANIMATION_EVENT_BASED)) {
			//	AnimateOnKeyPress(aniData, texData);
			//}
		}
	}
}