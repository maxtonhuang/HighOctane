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

struct Matrix {
    glm::mat3 matrix;
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
};

struct Visible {
    bool isVisible;
};

struct BoundingBox {
    //AABB

};
