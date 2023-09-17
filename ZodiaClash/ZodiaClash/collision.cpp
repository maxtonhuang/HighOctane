#include "Collision.h"
#include "Physics.h"

extern float g_dt;

namespace physics {

	// SECTION: COLLISION DETECTION
    bool CheckCollisionBoxBox(const Body& alpha, const Body& beta) {
		AABB* aabb1 = static_cast<AABB*>(alpha.bodyShape);
		AABB* aabb2 = static_cast<AABB*>(beta.bodyShape);

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
		Circle* circle1 = static_cast<Circle*>(alpha.bodyShape);
		Circle* circle2 = static_cast<Circle*>(beta.bodyShape);

		vmath::Vector2 centerA = alpha.position;
		vmath::Vector2 centerB = beta.position;
		float radiusA = circle1->radius;
		float radiusB = circle2->radius;

		float radiusSum = radiusA + radiusB;
		float centerDistance = centerA.distance(centerB);

		return (centerDistance <= radiusSum);
    }

    bool CheckCollisionCircleBox(const Body& alpha, const Body& beta) {
		Circle* circle = static_cast<Circle*>(alpha.bodyShape);
		AABB* aabb = static_cast<AABB*>(beta.bodyShape);

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
		AABB* aabb = static_cast<AABB*>(alpha.bodyShape);

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
		Circle* circle = static_cast<Circle*>(alpha.bodyShape);

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


	// SECTION: COLLISION DETECTION
	void OnCollideSnap(Body& alpha) {
		// snap colliding object's position such that it touches nicely against a non-colliding object body
		// to test: prevPos may still have some gap w to be colliding obj
		//alpha.position = alpha.prevPosition;

		// 1. calc remaining distance
		vmath::Vector2 dir = alpha.position - alpha.prevPosition;
		float dist = dir.magnitude();

		// 2. set interpolation factor
		float interpolateSpd = 0.1f;

		// 3. apply linear interpolation
		alpha.prevPosition = alpha.position;
		alpha.position = alpha.position + (dir * interpolateSpd);

		// 4. check dist again
		if (dist < 0.01f) {
			alpha.position = alpha.prevPosition;
		}

	}

	void OnCollideDestroy(Body& alpha) {
		// remove this shape from vector bodies
		// note: to confirm if its safe to delete here!!
		
		//most likely done in Body destructor?
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



	/// ARCHIVINGS PENDATIONS

    //void CollisionManager::CollisionDetectionAndResponse() {
    //    //for each body, check for collision with the all other bodies in the container
    //    for (size_t i = 0; i < bodies.size(); ++i) {
    //        for (size_t j = i + 1; j < bodies.size(); ++j) {
    //            //if collied, execute collsion response
    //            if (CheckCollision(bodies[i], bodies[j])) {
    //                CollisionResponse(bodies[i], bodies[j]);
    //            }
    //        }
    //    }
    //}

    //bool CollisionManager::CheckCollision(const Body& bodyA, const Body& bodyB) {
    //    // Compare AABBs (Axis-Aligned Bounding Boxes) of the bodies
    //    float half_sumWidth = (bodyA.size.x + bodyB.size.x) * 0.5f, half_sumHeight = (bodyA.size.y + bodyB.size.y) * 0.5f;

    //    // Horizontal: CHeck the difference between the x coordinate of the body's center positions and bodies' width
    //    bool xOverlap = abs(bodyA.position.x - bodyB.position.x) <= half_sumWidth; //overlap if less than half width

    //    // Vertical: CHeck the difference between the y coordinate of the body's center positions and bodies' heigh
    //    bool yOverlap = abs(bodyA.position.y - bodyB.position.y) <= half_sumHeight; //overlap if less than half height

    //    // if both x and y axes overlap, a collision is detected, return true
    //    return xOverlap && yOverlap;
    //}


    //void CollisionManager::CollisionResponse(Body& bodyA, Body& bodyB) {
    //    // Calculate relative velocity , how the bodies are moving relative to each other after the collision
    //    vmath::Vector2 relativeVelocity = bodyB.velocity - bodyA.velocity;

    //    // Calculate the collision direction vector (direction from A to B), normalized so length = 1
    //    vmath::Vector2 collisionDirection = (bodyB.position - bodyA.position).normalize();

    //    //measure how fast the bodies are moving towards each other along the direction of collision
    //    float velocityAlongDirection = relativeVelocity ^ (collisionDirection);

    //    // checking if the relative motion is away from the collision point (bodies moving away from each other)
    //    if (velocityAlongDirection > 0) {
    //        return; // bodies are already separating, no need to resolve (not gonna colide)
    //    }

    //    // use the bounciness of both bodies to calculate the scalar value to be applied 
    //    // for collision response IN OPPOSITE DIRECTION hence -ve (0 = no bounce, 1 = full bounce)
    //    float bounceScalar = -(std::min(bodyA.bounciness, bodyB.bounciness)) * velocityAlongDirection;
    //    bounceScalar /= (1 / bodyA.mass + 1 / bodyB.mass); //adjusts the impulse scalar based on the masses of the bodies

    //    // Apply rebounce to each body based on mass and collision direction
    //    vmath::Vector2 bounce = collisionDirection * bounceScalar;
    //    bodyA.velocity -= bounce / bodyA.mass;
    //    bodyB.velocity += bounce / bodyB.mass;

    //}
}