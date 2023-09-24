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
*	@brief		Body system of the engine
*
*	This file contains functions used for collision response
*
******************************************************************************/

#include "CollisionResolution.h" 
using namespace vmath;

namespace physics {

    //handle the collision response between a dynamic body and a static body
    //dynamicBody is the moving object, while staticBody remains stationary
    void DynamicStaticResponse(Body& dynamicBody, const Body& staticBody) 
    {
        //reverse the velocity to cause it to "bounce back" , cancel the velocity
        dynamicBody.velocity = (dynamicBody.velocity)*(-1);
    }

    //to handle the collision response between two dynamic bodies
    void DynamicDynamicResponse(Body& bodyA, Body& bodyB) 
    {
        Vector2 tempVelocity = bodyA.velocity;
        //swap the velocities, causing them to exchange their velocities upon collision
        //bounce off each other
        bodyA.velocity = bodyB.velocity;
        bodyB.velocity = tempVelocity;
    }

    void HandleCollisionResponse(Body& alpha, Body& beta) 
    {
        if (beta.isStatic) 
        {
            DynamicStaticResponse(alpha, beta);
        }
        else 
        {
            DynamicDynamicResponse(alpha, beta);
        }
    }
}



