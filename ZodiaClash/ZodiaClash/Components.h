#pragma once

#include "ECS.h"
#include "VMath.h"
#include "GraphLib.h"
#include "Texture.h"

using Vec2 = vmath::Vector2;

struct Transform {
    glm::vec2 position;
    float rotation;
    glm::vec2 scale;
};

struct Vel {
    Vec2 velocity;
};

struct Color {
    glm::vec4 color;
};

struct Texture {
    Texture* tex{};
};


