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
#include "FontLib.h"
#include "Texture.h"

using Vec2 = vmath::Vector2;

struct Transform {
    Vec2                    position{};
    float                   rotation{};
    float                   scale{};
    Vec2                    velocity{};       
    float                   radius{};
    Vec2                    halfDimensions{ scale / 2.f,  scale / 2.f };
    bool                    isStatic{}; 
};

struct Collider {
    enum SHAPE_ID {
        SHAPE_BOX,
        SHAPE_CIRCLE,
        NUM_OF_SHAPES
    };
    SHAPE_ID                bodyShape{};
    //Collider(SHAPE_ID shape = SHAPE_BOX) { (void)shape; };
};

struct Size {
    float                   width{};
    float                   height{};
};

//NOT NEEDED, TO STORE IN MODEL COMPONENT
struct Color {
    glm::vec4               color{};
};

struct Tex {
    Texture*                tex{};
    std::vector<Texture*>   texVariants{};
    uint32_t                texVariantIndex{};
    uint32_t                rows{};
    uint32_t                cols{};
    uint32_t                spritenum{};
};

struct Visible {
    bool                    isVisible{};
};

struct MainCharacter {
    // empty by design
};

struct Circle {
    float                   radius{};
};

struct AABB {
    Vec2                    min{};
    Vec2                    max{};
    Vec2                    extents{};
};

struct Master {
    // empty by design
};

struct Clone {
    // empty by design
};

struct Name {
    std::string             name{};
    bool                    selected{ false };
};

struct Movable {
    // empty by design
};

struct Screen {
    bool                    isActive{false};
};

struct Script {
    std::string             scriptName{};     // ID of owner
    // ID of owner
    //Entity entityID;
    ////vector of the names on scripts
    //std::vector<std::string> monoScriptName;
    ////vector of script instances
    //std::vector<std::shared_ptr<ScriptInstance>> monoScriptObj; //This is every C# component


    //// string to be added (display for ImGui side)
    //std::string currentAddingScript = "";
    //// string currently selected (display for ImGui side)
    //std::string currentSelectedScript = "";
};

struct Tag {
    std::string             tag;
};
