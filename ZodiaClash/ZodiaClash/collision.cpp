#include "Collision.h"

extern float dt;

namespace physics {


    bool CollisionManager::CheckCollisionBoxBox(const AABB& aabb1, const vmath::Vector2& vel1, const AABB& aabb2, const vmath::Vector2& vel2) {
		/*
		Step 1: Check for static collision detection between rectangles (before moving).
					If the check returns no overlap you continue with the following next steps (dynamics).
					Otherwise you return collision true (if all 4 tests fail)
		*/
		if ((aabb1.max.x < aabb2.min.x) || (aabb1.max.y < aabb2.min.y)
			|| (aabb1.min.x > aabb2.max.x) || (aabb1.min.y > aabb2.max.y))
		{
			return 0;
		}
		else
		{
			/*
			Step 2: Initialize and calculate the new velocity of Vb
			*/
			float tFirst = 0.f;
			float tLast = dt;

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
				if (aabb1.min.x > aabb2.max.x)
					return 0;
				//case 4
				if (aabb1.max.x < aabb2.min.x)
				{
					float dFirst = aabb1.max.x - aabb2.min.x;
					tFirst = ((dFirst / vB.x) > tFirst) ? (dFirst / vB.x) : tFirst;
				}
				if (aabb1.min.x < aabb2.max.x)
				{
					float dLast = aabb1.min.x - aabb2.max.x;
					tLast = ((dLast / vB.x) < tLast) ? (dLast / vB.x) : tLast;
				}
			}
			if (vB.x > 0.f)
			{
				//case 2
				if (aabb1.min.x > aabb2.max.x)
				{
					float dFirst = aabb1.min.x - aabb2.max.x;
					tFirst = ((dFirst / vB.x) > tFirst) ? (dFirst / vB.x) : tFirst;
				}
				if (aabb1.max.x > aabb2.min.x)
				{
					float dLast = aabb1.max.x - aabb2.min.x;
					tLast = ((dLast / vB.x) < tLast) ? (dLast / vB.x) : tLast;
				}
				//case 3
				if (aabb1.max.x < aabb2.min.x)
					return 0;
			}
			if (tFirst > tLast)
				return 0;

			/* Step 4: Repeat step 3 on the y - axis */
			if (vB.y < 0.f)
			{
				//case 1
				if (aabb1.min.y > aabb2.max.y)
					return 0;
				//case 4
				if (aabb1.max.y < aabb2.min.y)
				{
					float dFirst = aabb1.max.y - aabb2.min.y;
					tFirst = ((dFirst / vB.y) > tFirst) ? (dFirst / vB.y) : tFirst;
				}
				if (aabb1.min.y < aabb2.max.y)
				{
					float dLast = aabb1.min.y - aabb2.max.y;
					tFirst = ((dLast / vB.y) < tLast) ? (dLast / vB.y) : tLast;
				}
			}
			if (vB.y > 0.f)
			{
				//case 2
				if (aabb1.min.y > aabb2.max.y)
				{
					float dFirst = aabb1.min.y - aabb2.max.y;
					tFirst = ((dFirst / vB.y) > tFirst) ? (dFirst / vB.y) : tFirst;
				}
				if (aabb1.max.y > aabb2.min.y)
				{
					float dLast = aabb1.max.y - aabb2.min.y;
					tFirst = ((dLast / vB.y) < tLast) ? (dLast / vB.y) : tLast;
				}
				//case 3
				if (aabb1.max.y < aabb2.min.y)
					return 0;
			}
			if (tFirst > tLast)
				return 0;

			/* Step 5: Otherwise the rectangles intersect */
			return 1;
		}
    }

    bool CollisionManager::CheckCollisionCircleCircle(const Circle& circle1, const vmath::Vector2& vel1, const Circle& circle2, const vmath::Vector2& vel2) {

		vmath::Vector2 centerA = circle1.center;
		vmath::Vector2 centerB = circle2.center;
		float radiusA = circle1.radius;
		float radiusB = circle2.radius;

		float radiusSum = radiusA + radiusB;
		float centerDistance = centerA.distance(centerB);

		return (centerDistance <= radiusSum);
    }

    bool CollisionManager::CheckCollisionCircleBox(const Circle& circle, const vmath::Vector2& vel1, const AABB& aabb, const vmath::Vector2& vel2) {
		
		// 1. identify closest sides of AABB to circle
		float closestX = std::max(aabb.min.x, std::min(circle.center.x, aabb.max.x));
		float closestY = std::max(aabb.min.y, std::min(circle.center.y, aabb.max.y));
		vmath::Vector2 closestPoint{ closestX, closestY };

		// 2. compare distance between circle and closest point
		float distance = circle.center.distance(closestPoint);

		return (distance < circle.radius);
    }

    bool CollisionManager::CheckCollisionBoxCircle(const AABB& aabb, const vmath::Vector2& vel1, const Circle& circle, const vmath::Vector2& vel2) {
		return CollisionManager::CheckCollisionCircleBox(circle, vel2, aabb, vel1);
    }

	bool CollisionManager::CheckCollisionBoxBorder(const AABB& aabb, const vmath::Vector2& vel) {
		//hardcoded values, to update with vars
		float windowWidth = 1000.f;
		float windowHeight = 1000.f;

		//assuming camera is not involved!!
		if ((aabb.max.x < windowWidth) || (aabb.max.y < windowHeight)
			|| (aabb.min.x > 0.f) || (aabb.min.y > 0.f))
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

	bool CollisionManager::CheckCollisionBCircleBorder(const Circle& circle, const vmath::Vector2& vel) {
		//hardcoded values, to update with vars
		float windowWidth = 1000.f;
		float windowHeight = 1000.f;

		//assuming camera is not involved!!
		if (((circle.center.x + circle.radius) < windowWidth) 
		||	((circle.center.y + circle.radius) < windowHeight)
		||	((circle.center.x - circle.radius) > 0.f) 
		||	((circle.center.y - circle.radius) > 0.f))
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

	void CollisionManager::OnCollideSnap(Shape::SHAPE_ID alpha, Shape::SHAPE_ID beta) {
		// snap colliding object's position such that it touches nicely against a non-colliding object body
	}

	void CollisionManager::OnCollideDestroy(Shape::SHAPE_ID alpha, Shape::SHAPE_ID beta) {
		// remove this shape from vector bodies
	}


    CollisionManager::CollisionManager() {
        
    }

	void CollisionManager::CheckCollision(Shape::SHAPE_ID alpha, Shape::SHAPE_ID beta) {
		if ((alpha == Shape::SHAPE_BOX) && (beta == Shape::SHAPE_BOX)) {
			//check if alpha isStatic is false first?
			//CheckCollisionBoxBox();

			//collision response
			//if beta isStatic is true, call OnCollideSnap()
			//if beta isVulnerable is true, call OnCollideDestroy()
		}
		else if ((alpha == Shape::SHAPE_BOX) && (beta == Shape::SHAPE_CIRCLE)) {
			//check if alpha isStatic is false first?
			//CheckCollisionBoxCircle();

			//collision response
			//if beta isStatic is true, call OnCollideSnap()
			//if beta isVulnerable is true, call OnCollideDestroy()
		}
		else if ((alpha == Shape::SHAPE_CIRCLE) && (beta == Shape::SHAPE_CIRCLE)) {
			//check if alpha isStatic is false first?
			//CheckCollisionBoxCircle();

			//collision response
			//if beta isStatic is true, call OnCollideSnap()
			//if beta isVulnerable is true, call OnCollideDestroy()
		}
		else if ((alpha == Shape::SHAPE_CIRCLE) && (beta == Shape::SHAPE_BOX)) {
			//check if alpha isStatic is false first?
			//CheckCollisionCircleBox();

			//collision response
			//if beta isStatic is true, call OnCollideSnap()
			//if beta isVulnerable is true, call OnCollideDestroy()
		}

		//separate checking system for border collision?
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