#pragma once

#include <cstddef>
#include "Components.h"
#include "model.h"

void Selection(Entity& entity, Name& name, Transform& transform, Model& model, size_t layer_it);

void UnselectAll();

void ProcessSelection(Name& name, size_t layer_it, CLICKED corner);