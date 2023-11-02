#pragma once
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
*	@file		CollsionResolution.cpp
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
*	@brief		Iterative impulse collision resolution system.
*
*
******************************************************************************/
#include "CollisionResolution.h" 
using namespace vmath;

#define FIXED_DT 1.0f/60.f

namespace physics {

    /**************************************************************************/
    /*!
        @brief Handles the collision response between a dynamic body and a static body.
        @param dynamicBody Reference to the dynamic (moving) body involved in the collision.
        @param staticBody Reference to the static (stationary) body involved in the collision.

        Static vs Dynamic Collison response
     */
     /**************************************************************************/
    void DynamicStaticResponse(Transform& dynamicBody) 
    {
        dynamicBody.position -= dynamicBody.velocity * FIXED_DT * 2;
        dynamicBody.velocity = Vec2{ 0,0 };
    }
}



