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

namespace physics {

    /**************************************************************************/
    /*!
        @brief Handles the collision response between a dynamic body and a static body.
        @param dynamicBody Reference to the dynamic (moving) body involved in the collision.
        @param staticBody Reference to the static (stationary) body involved in the collision.

        Static vs Dynamic Collison response
     */
     /**************************************************************************/
    void DynamicStaticResponse(Body& dynamicBody, const Body& staticBody) 
    {
        //reverse the velocity to cause it to "bounce back" , cancel the velocity
        dynamicBody.velocity = (dynamicBody.velocity)*(-1);
    }

    /**************************************************************************/
    /*!
        @brief Handles the collision response between two dynamic bodies.
        @param bodyA Reference to the first dynamic body involved in the collision.
        @param bodyB Reference to the second dynamic body involved in the collision.

        Dynamic vs Dynamic Collision response
     */
     /**************************************************************************/
    void DynamicDynamicResponse(Body& bodyA, Body& bodyB) 
    {
        Vector2 tempVelocity = bodyA.velocity;
        //swap the velocities, causing them to exchange their velocities upon collision
        //bounce off each other
        bodyA.velocity = bodyB.velocity;
        bodyB.velocity = tempVelocity;
    }

    /**************************************************************************/
    /*!
        @brief Handles the collision response between two bodies.
        @param alpha Reference to the first body involved in the collision.
        @param beta Reference to the second body involved in the collision.

        Depending on the nature of the bodies involved in the collision, this function
        will delegate the response to either DynamicStaticResponse or DynamicDynamicResponse.
     */
     /**************************************************************************/
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



