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
#include "Reflections.h"
#include <unordered_set>

using Vec2 = vmath::Vector2;
using Entity = std::uint32_t;
using Vec3 = vmath::Vector3;

enum class CLICKED {
    N,
    E,
    S,
    W,
    NE,
    SE,
    SW,
    NW,
    INSIDE,
    DOT,
    NONE
};

struct Transform {
    Vec2                    position{};
    float                   rotation{};
    float                   scale{1.f};
    Vec2                    velocity{}; 
    float                   radius{};
    Vec2                    halfDimensions{ scale / 2.f,  scale / 2.f };
    bool                    isStatic{}; 
    //add forces
    float                   mass{1};
    float                   inverseMass{1/mass};
    Vec2                    acceleration{};
    Vec2                    force{acceleration * mass};
};

//Operator overload for child offset calculations
Transform operator+ (const Transform& lhs, const Transform& rhs);

struct Collider {
    enum SHAPE_ID {
        SHAPE_BOX,
        SHAPE_CIRCLE,
        NUM_OF_SHAPES
    };
    enum COLLISION_TYPE {
        MAIN,
        WALL,
        EVENT
    };
    SHAPE_ID                bodyShape{};
    COLLISION_TYPE          type{WALL};
    Vec2                    dimension{};
    Vec2                    position{};
    float                   rotation{};
    float                   scale{ 1.f };
    float                   radius{};
    Vec2                    halfDimensions{ scale / 2.f,  scale / 2.f };

    //For event type collision
    std::string             eventName{};
    std::string             eventInput{};
    bool                    collided{ false };
};

struct Size {
    float                   width{};
    float                   height{};
};

struct Color {
    glm::vec4               color{};
};

struct Tex {
    Texture*                tex{};
    std::vector<Texture*>   texVariants{};
    int                     frameIndex{};
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

struct Name {
    std::string             name{};
    bool                    selected{ false };
    CLICKED                 clicked = CLICKED::NONE;
    size_t                  group{};
    size_t                  serializationLayer{};
    size_t                  serializationOrderInLayer{};
    bool                    skip{ true };
    bool                    lock{ true };
    vmath::Vector2          draggingOffset{};
};

struct Parent {
    Entity GetChildByName(std::string name);
    std::vector<Entity>     children{};
};

struct Clone {
    std::string             prefab{};
    std::unordered_set<std::string> unique_components{ typeid(Transform).name(), typeid(Name).name(), typeid(Parent).name() };
};

struct Movable {
    // empty by design
};

struct Script {
    std::string             className{};  // ID of owner
    std::vector<std::string> scriptNameVec{};

};

struct Tag {
    std::string             tag;
};

struct Child {
    Entity                  parent{};
    Transform               offset{};
};


