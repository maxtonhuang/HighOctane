#pragma once

#include "Components.h"
#include "vmath.h"
#include "model.h"

bool IsWithinObject(Model& model, vmath::Vector2 cursorPos);

vmath::Vector2 GetOffset(vmath::Vector2 point1, vmath::Vector2 point2);

bool IsNearby(vmath::Vector2 point1, vmath::Vector2 point2, float distance);
