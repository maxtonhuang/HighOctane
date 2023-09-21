#pragma once

#include "ECS.h"
#include "VMath.h"
#include "GraphLib.h"
#include "Texture.h"

using Vec2 = vmath::Vector2;

struct Transform {
    Vec2 position;
    float rotation;
    Vec2 scale;
    Vec2 velocity;
};

struct Size {
    float width;
    float height;
};

struct Color {
    glm::vec4 color;
};

struct Tex {
    Texture* tex;
    uint32_t rows;
    uint32_t cols;
    uint32_t spritenum;
};

struct Visible {
    bool isVisible;
};

struct MainCharacter {
    
};

struct Circle {
    float radius;
};

struct AABB {
    Vec2 min;
    Vec2 max;
    Vec2 extents;
};

struct Animation {
    enum ANIMATION_TYPE {
        ANIMATION_NONE,
        ANIMATION_TIME_BASED,
        ANIMATION_EVENT_BASED,
        // PARTICLE
    };
    ANIMATION_TYPE animationType;
    uint32_t frameIndex{}; //animation in Model class
    float frameTimeElapsed{};
    float frameDisplayDuration{};
};

struct Clone {

};

class Model;