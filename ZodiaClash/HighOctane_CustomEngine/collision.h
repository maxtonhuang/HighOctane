/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		Collision.h
*
*	@author		Chua Zhen Rong
*
*	@email		c.zhenrong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		05 September 2023
*
* *****************************************************************************
*
*	@brief		Collision detection between 2 entities.
*
*   This file contains all the declaration of parent functions for collision detection
*
******************************************************************************/

#pragma once
#include <vector>
#include "VMath.h"
#include "ECS.h"

namespace physics {

	class CollisionManager
	{
	public:
		//static bool CheckBorderCollision(const Transform& alpha);
	};

	bool CheckCollisionBoxBox(const Collider& alpha, const Collider& beta, vmath::Vector2 v1, vmath::Vector2 v2);
	bool CheckCollisionCircleCircle(const Collider& alpha, const Collider& beta);
	bool CheckCollisionCircleBox(const Collider& alpha, const Collider& beta);
	bool CheckCollisionBoxCircle(const Collider& alpha, const Collider& beta);
	bool CheckCollisionBoxBorder(const Collider& alpha);
	bool CheckCollisionCircleBorder(const Collider& alpha);

	extern CollisionManager* COLLISION;
}