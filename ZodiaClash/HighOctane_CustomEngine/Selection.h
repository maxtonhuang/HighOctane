#pragma once

#include <cstddef>
#include "Components.h"
#include "model.h"

void Selection(Entity& entity, Name& name, Transform& transform, Model& model, size_t layer_it);

void UnselectAll();

void ProcessSelection(Name& name, size_t layer_it);

void GroupSelection();

void UngroupSelection();

void SelectWholeGroup(size_t groupNumber);

void UnselectWholeGroup(size_t groupNumber);