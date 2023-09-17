#pragma once

#include "ECS.h"
#include "VMath.h"

using Vec2 = vmath::Vector2;

struct Transform {
    Vec2 position;
    Vec2 rotation;
    Vec2 scale;
};

struct Vel {
    Vec2 velocity;
};
