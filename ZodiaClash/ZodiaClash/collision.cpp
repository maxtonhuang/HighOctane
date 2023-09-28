#include "Collision.h"
#include "Physics.h"
#include "Global.h"

namespace physics {

	// SECTION: COLLISION DETECTION
    bool CheckCollisionBoxBox(const Body& alpha, const Body& beta) {
		/*AABB* aabb1 = static_cast<AABB*>(alpha.bodyShape);
		AABB* aabb2 = static_cast<AABB*>(beta.bodyShape);*/

		/*aabb1->extents = alpha.halfDimensions;
		aabb2->extents = beta.halfDimensions;*/

		//extract min-max
		Vec2 min1 = { alpha.position.x - alpha.halfDimensions.x, alpha.position.y - alpha.halfDimensions.y };
		Vec2 max1 = { alpha.position.x + alpha.halfDimensions.x, alpha.position.y + alpha.halfDimensions.y };

		Vec2 min2 = { beta.position.x - beta.halfDimensions.x, beta.position.y - beta.halfDimensions.y };
		Vec2 max2 = { beta.position.x + beta.halfDimensions.x, beta.position.y + beta.halfDimensions.y };

		//extract velocities
		vmath::Vector2 vel1 = alpha.velocity;
		vmath::Vector2 vel2 = beta.velocity;


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
			float tLast = g_dt;

			vmath::Vector2 vB;
			vB.x = vel2.x - vel1.x;
			vB.y = vel2.y - vel1.y;

			/*
			Step 3: Working with one dimension (x-axis).
					if(Vb < 0)
						case 1
						case 4
					if(Vb > 0)
						case 2
						case 3

					case 5
			*/
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

    bool CheckCollisionCircleCircle(const Body& alpha, const Body& beta) {
		/*const Circle& circle1 = static_cast<const Circle&>(alpha.bodyShape);
		const Circle& circle2 = static_cast<const Circle&>(beta.bodyShape);*/

		vmath::Vector2 centerA = alpha.position;
		vmath::Vector2 centerB = beta.position;
		float radiusA = alpha.radius;
		float radiusB = beta.radius;

		float radiusSum = radiusA + radiusB;
		float centerDistance = centerA.distance(centerB);

		return (centerDistance <= radiusSum);
    }

    bool CheckCollisionCircleBox(const Body& alpha, const Body& beta) {
		/*const Circle& circle = static_cast<const Circle&>(alpha.bodyShape);
		AABB* aabb = static_cast<AABB*>(beta.bodyShape);*/

		vmath::Vector2 circleCenter = alpha.position;
		/*circle.radius = alpha.radius;
		aabb->extents = beta.halfDimensions;*/

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

    bool CheckCollisionBoxCircle(const Body& alpha, const Body& beta) {
		return CheckCollisionCircleBox(beta, alpha);
    }

	bool CheckCollisionBoxBorder(const Body& alpha) {
		//const AABB& aabb = static_cast<const AABB&>(alpha.bodyShape);
		//aabb.extents = alpha.halfDimensions;

		//extract min-max
		Vec2 min = { alpha.position.x - alpha.halfDimensions.x, alpha.position.y - alpha.halfDimensions.y };
		Vec2 max = { alpha.position.x + alpha.halfDimensions.x, alpha.position.y + alpha.halfDimensions.y };

		float windowWidth = GRAPHICS::defaultWidthF;
		float windowHeight = GRAPHICS::defaultHeightF;

		//assuming camera is not involved!!
		if ((max.x < windowWidth) || (max.y < windowHeight)
			|| (min.x > 0.f) || (min.y > 0.f))
		{
			return 0;
		}
		else
			return 1;

		/**
		//if camera is involved (dynamic?)
		vmath::Vector2 cameraOffset{};
		float viewportWidth{};
		float viewportHeight{};
		if (((aabb.max.x + cameraOffset.x) < viewportWidth) || ((aabb.max.y + cameraOffset.y) < viewportHeight)
			|| ((aabb.min.x + cameraOffset.x) > 0.f) || ((aabb.min.y + cameraOffset.y) > 0.f))
		{
			return 0;
		}
		else
			return 1;
		**/

	}

	bool CheckCollisionCircleBorder(const Body& alpha) {
		vmath::Vector2 circleCenter = alpha.position;

		//circle->radius = alpha.radius;

		float windowWidth = GRAPHICS::defaultWidthF;
		float windowHeight = GRAPHICS::defaultHeightF;

		//assuming camera is not involved!!
		if (((circleCenter.x + alpha.radius) < windowWidth) 
		||	((circleCenter.y + alpha.radius) < windowHeight)
		||	((circleCenter.x - alpha.radius) > 0.f) 
		||	((circleCenter.y - alpha.radius) > 0.f))
		{
			return 0;
		}
		else
			return 1;

		/**
		//if camera is involved (dynamic?)
		vmath::Vector2 cameraOffset{};
		float viewportWidth{};
		float viewportHeight{};
		vmath::Vector2 circleOffset = cameraOffset + circle.center;

		if (((circleOffset.x + circle.radius) < windowWidth)
		|| ((circleOffset.y + circle.radius) < windowHeight)
		|| ((circleOffset.x - circle.radius) > 0.f)
		|| ((circleOffset.y - circle.radius) > 0.f))
		{
			return 0;
		}
		else
			return 1;
		**/
	}

    CollisionManager::CollisionManager() {
        
    }

	bool CollisionManager::CheckBodyCollision(const Body& alpha, const Body& beta) {
		
		SHAPE_ID alphaShape = alpha.bodyShape;
		SHAPE_ID betaShape = beta.bodyShape;

		if ((alphaShape == SHAPE_BOX) && (betaShape == SHAPE_BOX)) {
			return CheckCollisionBoxBox(alpha, beta);
		}
		else if ((alphaShape == SHAPE_CIRCLE) && (betaShape == SHAPE_CIRCLE)) {
			return CheckCollisionCircleCircle(alpha, beta);
		}
		else if ((alphaShape == SHAPE_CIRCLE) && (betaShape == SHAPE_BOX)) {
			return CheckCollisionCircleBox(alpha, beta);
		}
		else if ((alphaShape == SHAPE_BOX) && (betaShape == SHAPE_CIRCLE)) {
			return CheckCollisionBoxCircle(alpha, beta);
		}
		else
		{
			//assert maybe?
			return false;
		}

		//collision response
		//if beta isStatic is true, call OnCollideSnap()
		//if beta isVulnerable is true, call OnCollideDestroy()
	}

	bool CollisionManager::CheckBorderCollision(const Body& alpha) {
		SHAPE_ID alphaShape = alpha.bodyShape;
		if (alphaShape == SHAPE_BOX) {
			return CheckCollisionBoxBorder(alpha);
		}
		else if (alphaShape == SHAPE_CIRCLE) {
			return CheckCollisionCircleBorder(alpha);
		}
		else
		{
			//assert maybe?
			return false;
		}
	}

}