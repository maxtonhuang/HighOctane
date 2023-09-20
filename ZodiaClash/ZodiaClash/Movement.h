#pragma once
#include "ECS.h"

//extern int keyInput;

enum OBJECT_DIRECTION {
	UP_DIRECTION = 21,
	UPRIGHT_DIRECTION = 22,
	RIGHT_DIRECTION = 12,
	DOWNRIGHT_DIRECTION = 2,
	DOWN_DIRECTION = 1,
	DOWNLEFT_DIRECTION = 0,
	LEFT_DIRECTION = 10,
	UPLEFT_DIRECTION = 20,
	NEUTRAL_DIRECTION = 11,
};

void UpdateMovement(Transform & transform);

void UpdateModel(Transform & transform);