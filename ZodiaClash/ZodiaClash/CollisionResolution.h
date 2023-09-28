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
*	@file		CollsionResolution.h
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

#pragma once
#include "body.h"
#include "collision.h"

namespace physics {
<<<<<<< Updated upstream
    void DynamicStaticResponse(Body& dynamicBody, const Body& staticBody);
    void DynamicDynamicResponse(Body& bodyA, Body& bodyB);
    void HandleCollisionResponse(Body& alpha, Body& beta);
=======
    void DynamicStaticResponse(Body& dynamicBody);
    void DynamicDynamicResponse(Body& bodyA, Body& bodyB, vmath::Vector2 const& contactNormal);
    void HandleCollisionResponse(Body& alpha, Body& beta, vmath::Vector2 const& contactNormal);
>>>>>>> Stashed changes
}
