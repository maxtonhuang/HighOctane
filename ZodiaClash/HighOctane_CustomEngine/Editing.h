#pragma once

#include "Components.h"
#include "model.h"

void UpdateProperties (Name & name, Transform & transform, Model & model);


enum class CLICKED {
	NE,
	SE,
	SW,
	NW,
	INSIDE,
	NOT
};