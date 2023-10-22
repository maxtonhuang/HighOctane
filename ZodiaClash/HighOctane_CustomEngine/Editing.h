#pragma once

#include "Components.h"
#include "model.h"

void UpdateProperties (Name & name, Transform & transform, Model & model, Size & size);


enum class CLICKED {
	NE,
	SE,
	SW,
	NW,
	INSIDE,
	NOT
};