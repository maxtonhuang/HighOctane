#include "Collision.h"
#include "Physics.h"
#include "Global.h"

namespace physics {

	// SECTION: COLLISION DETECTION
    bool CheckCollisionBoxBox(const Body& alpha, const Body& beta) {
		std::shared_ptr < AABB> aabb1 = std::reinterpret_pointer_cast<AABB>(alpha.bodyShape);
		std::shared_ptr < AABB> aabb2 = std::reinterpret_pointer_cast<AABB>(beta.bodyShape);

		//extract min-max
		aabb1->min = { alpha.position.x - aabb1->extents.x, alpha.position.y - aabb1->extents.y };
		aabb1->max = { alpha.position.x + aabb1->extents.x, alpha.position.y + aabb1->extents.y };

		aabb2->min = { beta.position.x - aabb2->extents.x, beta.position.y - aabb2->extents.y };
		aabb2->max = { beta.position.x + aabb2->extents.x, beta.position.y + aabb2->extents.y };

		//extract velocities
		vmath::Vector2 vel1 = alpha.velocity;
		vmath::Vector2 vel2 = beta.velocity;


		/*
		Step 1: Check for static collision detection between rectangles (before moving).
					If the check returns no overlap you continue with the following next steps (dynamics).
					Otherwise you return collision true (if all 4 tests fail)
		*/
		if ((aabb1->max.x < aabb2->min.x) || (aabb1->max.y < aabb2->min.y)
			|| (aabb1->min.x > aabb2->max.x) || (aabb1->min.y > aabb2->max.y))
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
				if (aabb1->min.x > aabb2->max.x)
					return 0;
				//case 4
				if (aabb1->max.x < aabb2->min.x)
				{
					float dFirst = aabb1->max.x - aabb2->min.x;
					tFirst = ((dFirst / vB.x) > tFirst) ? (dFirst / vB.x) : tFirst;
				}
				if (aabb1->min.x < aabb2->max.x)
				{
					float dLast = aabb1->min.x - aabb2->max.x;
					tLast = ((dLast / vB.x) < tLast) ? (dLast / vB.x) : tLast;
				}
			}
			if (vB.x > 0.f)
			{
				//case 2
				if (aabb1->min.x > aabb2->max.x)
				{
					float dFirst = aabb1->min.x - aabb2->max.x;
					tFirst = ((dFirst / vB.x) > tFirst) ? (dFirst / vB.x) : tFirst;
				}
				if (aabb1->max.x > aabb2->min.x)
				{
					float dLast = aabb1->max.x - aabb2->min.x;
					tLast = ((dLast / vB.x) < tLast) ? (dLast / vB.x) : tLast;
				}
				//case 3
				if (aabb1->max.x < aabb2->min.x)
					return 0;
			}
			if (tFirst > tLast)
				return 0;

			/* Step 4: Repeat step 3 on the y - axis */
			if (vB.y < 0.f)
			{
				//case 1
				if (aabb1->min.y > aabb2->max.y)
					return 0;
				//case 4
				if (aabb1->max.y < aabb2->min.y)
				{
					float dFirst = aabb1->max.y - aabb2->min.y;
					tFirst = ((dFirst / vB.y) > tFirst) ? (dFirst / vB.y) : tFirst;
				}
				if (aabb1->min.y < aabb2->max.y)
				{
					float dLast = aabb1->min.y - aabb2->max.y;
					tFirst = ((dLast / vB.y) < tLast) ? (dLast / vB.y) : tLast;
				}
			}
			if (vB.y > 0.f)
			{
				//case 2
				if (aabb1->min.y > aabb2->max.y)
				{
					float dFirst = aabb1->min.y - aabb2->max.y;
					tFirst = ((dFirst / vB.y) > tFirst) ? (dFirst / vB.y) : tFirst;
				}
				if (aabb1->max.y > aabb2->min.y)
				{
					float dLast = aabb1->max.y - aabb2->min.y;
					tFirst = ((dLast / vB.y) < tLast) ? (dLast / vB.y) : tLast;
				}
				//case 3
				if (aabb1->max.y < aabb2->min.y)
					return 0;
			}
			if (tFirst > tLast)
				return 0;

			/* Step 5: Otherwise the rectangles intersect */
			return 1;
		}
    }

    bool CheckCollisionCircleCircle(const Body& alpha, const Body& beta) {
		std::shared_ptr<Circle> circle1 = std::reinterpret_pointer_cast<Circle>(alpha.bodyShape);
		std::shared_ptr<Circle> circle2 = std::reinterpret_pointer_cast<Circle>(beta.bodyShape);

		vmath::Vector2 centerA = alpha.position;
		vmath::Vector2 centerB = beta.position;
		float radiusA = circle1->radius;
		float radiusB = circle2->radius;

		float radiusSum = radiusA + radiusB;
		float centerDistance = centerA.distance(centerB);

		return (centerDistance <= radiusSum);
    }

    bool CheckCollisionCircleBox(const Body& alpha, const Body& beta) {
		std::shared_ptr<Circle> circle = std::reinterpret_pointer_cast<Circle>(alpha.bodyShape);
		std::shared_ptr <AABB> aabb = std::reinterpret_pointer_cast<AABB>(beta.bodyShape);

		vmath::Vector2 circleCenter = alpha.position;

		//extract min-max
		aabb->min = { beta.position.x - aabb->extents.x, beta.position.y - aabb->extents.y };
		aabb->max = { beta.position.x + aabb->extents.x, beta.position.y + aabb->extents.y };
		
		// 1. identify closest sides of AABB to circle
		float closestX = std::max(aabb->min.x, std::min(circleCenter.x, aabb->max.x));
		float closestY = std::max(aabb->min.y, std::min(circleCenter.y, aabb->max.y));
		vmath::Vector2 closestPoint{ closestX, closestY };

		// 2. compare distance between circle and closest point
		float distance = circleCenter.distance(closestPoint);

		return (distance < circle->radius);
    }

    bool CheckCollisionBoxCircle(const Body& alpha, const Body& beta) {
		return CheckCollisionCircleBox(beta, alpha);
    }

	bool CheckCollisionBoxBorder(const Body& alpha) {
		std::shared_ptr<AABB> aabb = std::reinterpret_pointer_cast<AABB>(alpha.bodyShape);

		//extract min-max
		aabb->min = { alpha.position.x - aabb->extents.x, alpha.position.y - aabb->extents.y };
		aabb->max = { alpha.position.x + aabb->extents.x, alpha.position.y + aabb->extents.y };

		//hardcoded values, to update with vars
		float windowWidth = 1000.f;
		float windowHeight = 1000.f;

		//assuming camera is not involved!!
		if ((aabb->max.x < windowWidth) || (aabb->max.y < windowHeight)
			|| (aabb->min.x > 0.f) || (aabb->min.y > 0.f))
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
		std::shared_ptr<Circle> circle = std::reinterpret_pointer_cast<Circle>(alpha.bodyShape);

		vmath::Vector2 circleCenter = alpha.position;

		//hardcoded values, to update with vars
		float windowWidth = 1000.f;
		float windowHeight = 1000.f;

		//assuming camera is not involved!!
		if (((circleCenter.x + circle->radius) < windowWidth) 
		||	((circleCenter.y + circle->radius) < windowHeight)
		||	((circleCenter.x - circle->radius) > 0.f) 
		||	((circleCenter.y - circle->radius) > 0.f))
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
		
		Shape::SHAPE_ID alphaShape = alpha.bodyShape->id;
		Shape::SHAPE_ID betaShape = beta.bodyShape->id;

		if ((alphaShape == Shape::SHAPE_BOX) && (betaShape == Shape::SHAPE_BOX)) {
			return CheckCollisionBoxBox(alpha, beta);
		}
		else if ((alphaShape == Shape::SHAPE_CIRCLE) && (betaShape == Shape::SHAPE_CIRCLE)) {
			return CheckCollisionCircleCircle(alpha, beta);
		}
		else if ((alphaShape == Shape::SHAPE_CIRCLE) && (betaShape == Shape::SHAPE_BOX)) {
			return CheckCollisionCircleBox(alpha, beta);
		}
		else if ((alphaShape == Shape::SHAPE_BOX) && (betaShape == Shape::SHAPE_CIRCLE)) {
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
		Shape::SHAPE_ID alphaShape = alpha.bodyShape->id;
		if (alphaShape == Shape::SHAPE_BOX) {
			return CheckCollisionBoxBorder(alpha);
		}
		else if (alphaShape == Shape::SHAPE_CIRCLE) {
			return CheckCollisionCircleBorder(alpha);
		}
		else
		{
			//assert maybe?
			return false;
		}
	}

}