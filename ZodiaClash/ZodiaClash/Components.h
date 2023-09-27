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
*	@file		Components.h
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		All the Structs of individual components available for the ECS
*
*	-
*
******************************************************************************/

#pragma once

#include "ECS.h"
#include "VMath.h"
#include "GraphLib.h"
#include "Texture.h"
//#include "physics.h"

using Vec2 = vmath::Vector2;

struct Transform {
    Vec2                    position;
    float                   rotation;
    Vec2                    scale;
    Vec2                    velocity;
};

struct Size {
    float                   width;
    float                   height;
};

//NOT NEEDED, TO STORE IN MODEL COMPONENT
struct Color {
    glm::vec4               color;
};

struct Tex {
    Texture*                tex;
    std::vector<Texture*>   texVariants;
    uint32_t                texVariantIndex;
    uint32_t                rows;
    uint32_t                cols;
    uint32_t                spritenum;
};

struct Visible {
    bool                    isVisible;
};

struct MainCharacter {
    // empty by design
};

struct Circle {
    float                   radius;
};

struct AABB {
    Vec2                    min;
    Vec2                    max;
    Vec2                    extents;
};

struct Animation {
    enum ANIMATION_TYPE {
        ANIMATION_NONE,
        ANIMATION_TIME_BASED,
        ANIMATION_EVENT_BASED,
        // PARTICLE
    };
    ANIMATION_TYPE          animationType;
    uint32_t                frameIndex{};
    float                   frameTimeElapsed{};
    float                   frameDisplayDuration{};
};

struct Clone {
    // empty by design
};
