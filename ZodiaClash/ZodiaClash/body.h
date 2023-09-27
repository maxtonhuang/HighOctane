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
*	@file		Body.h
*
*	@author		Liu Wan Ting
*
*	@email		wanting.liu@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		24 September 2023
*
* *****************************************************************************
*
*	@brief		Define Body GameComponent
*
*
******************************************************************************/

#pragma once
#include "vmath.h"
#include "Collision.h"
#include "Physics.h"
#include <memory>

using namespace vmath;

//class Shape; //forward declaration

namespace physics {

	struct Body {
		Body* next{};
		Body* prev{};
		Vector2 position{};
		Vector2 prevPosition{};
		Vector2 velocity{};
		Vector2 acceleration{};
		Vector2 accumulatedForce{};
		float width{50};
		float height{50};
		Vector2 halfDimensions; // Half-width and half-height of the AABB
		float mass{};
		float restitution{};
		float friction{ 0.2f };
		bool isStatic{};
		std::shared_ptr<Shape> bodyShape;

		// Constructors, if needed, can remain
		Body();

		// These setters might stay if you find them useful, but in pure ECS they might not be here.
		void SetPosition(Vector2 pos);
		void SetVelocity(Vector2 vel);

		// Getters can also be useful
		Vector2 GetPosition() const { return position; }
		Vector2 GetVelocity() const { return velocity; }
	};
}
