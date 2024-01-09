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
*	@file		Collision.cpp
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
*   This file contains all the definition of the functions for collision detection
*
******************************************************************************/

#include "Collision.h"
#include "Physics.h"
#include "Global.h"

namespace physics {

	/*!
	 * \brief AABB-AABB collision detection
	 *
	 * This function checks for collision between 2 AABB entities. First does STATIC collision detection, then does DYNAMIC collision detection.
	 *
	 * \param alpha : The first AABB entity to check collision for.
	 * \param beta : The second AABB entity to check collision for.
	 *
	 */
	bool CheckCollisionBoxBox(const Collider& alpha, const Collider& beta, vmath::Vector2 v1, vmath::Vector2 v2) {

		//extract min-max
		Vec2 min1 = { alpha.position.x - alpha.halfDimensions.x, alpha.position.y - alpha.halfDimensions.y };
		Vec2 max1 = { alpha.position.x + alpha.halfDimensions.x, alpha.position.y + alpha.halfDimensions.y };

		Vec2 min2 = { beta.position.x - beta.halfDimensions.x, beta.position.y - beta.halfDimensions.y };
		Vec2 max2 = { beta.position.x + beta.halfDimensions.x, beta.position.y + beta.halfDimensions.y };

		//extract velocities
		vmath::Vector2 vel1 = v1;
		vmath::Vector2 vel2 = v2;


		/*
		Step 1: Check for static collision detection between rectangles (before moving).
					If the check returns no overlap you continue with the following next steps (dynamics).
					Otherwise you return collision true (if all 4 tests fail)
		*/
		if ((max1.x < min2.x) || (max1.y < min2.y)
			|| (min1.x > max2.x) || (min1.y > max2.y))
		{
			return 0;
		}
		else
		{
			/*
			Step 2: Initialize and calculate the new velocity of Vb
			*/
			float tFirst = 0.f;
			float tLast = FIXED_DT;

			vmath::Vector2 vB;
			vB.x = vel2.x - vel1.x;
			vB.y = vel2.y - vel1.y;

			if (vB.x < 0.f)
			{
				//case 1
				if (min1.x > max2.x)
					return 0;
				//case 4
				if (max1.x < min2.x)
				{
					float dFirst = max1.x - min2.x;
					tFirst = ((dFirst / vB.x) > tFirst) ? (dFirst / vB.x) : tFirst;
				}
				if (min1.x < max2.x)
				{
					float dLast = min1.x - max2.x;
					tLast = ((dLast / vB.x) < tLast) ? (dLast / vB.x) : tLast;
				}
			}
			if (vB.x > 0.f)
			{
				//case 2
				if (min1.x > max2.x)
				{
					float dFirst = min1.x - max2.x;
					tFirst = ((dFirst / vB.x) > tFirst) ? (dFirst / vB.x) : tFirst;
				}
				if (max1.x > min2.x)
				{
					float dLast = max1.x - min2.x;
					tLast = ((dLast / vB.x) < tLast) ? (dLast / vB.x) : tLast;
				}
				//case 3
				if (max1.x < min2.x)
					return 0;
			}
			if (tFirst > tLast)
				return 0;

			/* Step 4: Repeat step 3 on the y - axis */
			if (vB.y < 0.f)
			{
				//case 1
				if (min1.y > max2.y)
					return 0;
				//case 4
				if (max1.y < min2.y)
				{
					float dFirst = max1.y - min2.y;
					tFirst = ((dFirst / vB.y) > tFirst) ? (dFirst / vB.y) : tFirst;
				}
				if (min1.y < max2.y)
				{
					float dLast = min1.y - max2.y;
					tFirst = ((dLast / vB.y) < tLast) ? (dLast / vB.y) : tLast;
				}
			}
			if (vB.y > 0.f)
			{
				//case 2
				if (min1.y > max2.y)
				{
					float dFirst = min1.y - max2.y;
					tFirst = ((dFirst / vB.y) > tFirst) ? (dFirst / vB.y) : tFirst;
				}
				if (max1.y > min2.y)
				{
					float dLast = max1.y - min2.y;
					tFirst = ((dLast / vB.y) < tLast) ? (dLast / vB.y) : tLast;
				}
				//case 3
				if (max1.y < min2.y)
					return 0;
			}
			if (tFirst > tLast)
				return 0;

			/* Step 5: Otherwise the rectangles intersect */
			return 1;
		}
	}

	/*!
	 * \brief Circle-Circle collision detection
	 *
	 * This function checks for STATIC collision between 2 Circle entities.
	 *
	 * \param alpha : The first AABB entity to check collision for.
	 * \param beta : The second AABB entity to check collision for.
	 *
	 */
	bool CheckCollisionCircleCircle(const Collider& alpha, const Collider& beta) {

		vmath::Vector2 centerA = alpha.position;
		vmath::Vector2 centerB = beta.position;
		float radiusA = alpha.radius;
		float radiusB = beta.radius;

		float radiusSum = radiusA + radiusB;
		float centerDistance = centerA.distance(centerB);

		return (centerDistance <= radiusSum);
	}

	/*!
	 * \brief Circle-AABB collision detection
	 *
	 * This function checks for STATIC collision between 1 Circle entity and 1 AABB entity.
	 *
	 * \param alpha : The first AABB entity to check collision for.
	 * \param beta : The second AABB entity to check collision for.
	 *
	 */
	bool CheckCollisionCircleBox(const Collider& alpha, const Collider& beta) {

		vmath::Vector2 circleCenter = alpha.position;

		//extract min-max
		Vec2 min = { beta.position.x - beta.halfDimensions.x, beta.position.y - beta.halfDimensions.y };
		Vec2 max = { beta.position.x + beta.halfDimensions.x, beta.position.y + beta.halfDimensions.y };

		// 1. identify closest sides of AABB to circle
		float closestX = std::max(min.x, std::min(circleCenter.x, max.x));
		float closestY = std::max(min.y, std::min(circleCenter.y, max.y));
		vmath::Vector2 closestPoint{ closestX, closestY };

		// 2. compare distance between circle and closest point
		float distance = circleCenter.distance(closestPoint);

		return (distance < alpha.radius);
	}

	/*!
	 * \brief AABB-Circle collision detection
	 *
	 * This function checks for STATIC collision between 1 AABB entity and 1 Circle entity.
	 * Functionality works the same as Circle-AABB collision detection, refer to CheckCollisionCircleBox().
	 *
	 * \param alpha : The first AABB entity to check collision for.
	 * \param beta : The second AABB entity to check collision for.
	 *
	 */
	bool CheckCollisionBoxCircle(const Collider& alpha, const Collider& beta) {
		return CheckCollisionCircleBox(beta, alpha);
	}

	/*!
	 * \brief AABB-Border collision detection
	 *
	 * This function checks for STATIC collision between 1 AABB entity and screen border.
	 *
	 * \param alpha : The AABB entity to check collision for.
	 *
	 */
	bool CheckCollisionBoxBorder(const Collider& alpha) {

		//extract min-max
		Vec2 min = { alpha.position.x - alpha.halfDimensions.x, alpha.position.y - alpha.halfDimensions.y };
		Vec2 max = { alpha.position.x + alpha.halfDimensions.x, alpha.position.y + alpha.halfDimensions.y };

		//assuming camera is not involved!!
		if ((max.x < GRAPHICS::defaultWidthF) || (max.y < GRAPHICS::defaultHeightF)
			|| (min.x > 0.f) || (min.y > 0.f))
		{
			return 0;
		}
		else
			return 1;
	}

	/*!
	 * \brief Circle-Border collision detection
	 *
	 * This function checks for STATIC collision between 1 Circle entity and screen border.
	 *
	 * \param alpha : The Circle entity to check collision for.
	 *
	 */
	bool CheckCollisionCircleBorder(const Collider& alpha) {
		vmath::Vector2 circleCenter = alpha.position;

		//circle->radius = alpha.radius;

		float windowWidth = GRAPHICS::defaultWidthF;
		float windowHeight = GRAPHICS::defaultHeightF;

		//assuming camera is not involved!!
		if (((circleCenter.x + alpha.radius) < windowWidth)
			|| ((circleCenter.y + alpha.radius) < windowHeight)
			|| ((circleCenter.x - alpha.radius) > 0.f)
			|| ((circleCenter.y - alpha.radius) > 0.f))
		{
			return 0;
		}
		else
			return 1;
	}
}