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
*	@file		Body.cpp
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

#include "Body.h"
#include "Physics.h"

namespace physics {

	Body::Body(): 
				prevPosition(0, 0),
				acceleration(0, 0),
				accumulatedForce(0, 0),
				mass(0.0f),
				restitution(0.0f),
				halfDimensions{width/2.f, height/2.f},
				friction(0.2f),
				isStatic(false),
				bodyShape(nullptr)
	{}

	void Body::SetPosition(Vector2 pos) 
	{
		position = pos;
	}

	void Body::SetVelocity(Vector2 vel) 
	{
		velocity = vel;
	}
}
