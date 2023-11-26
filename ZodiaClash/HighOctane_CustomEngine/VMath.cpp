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
*	@file		MultiThreading.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Non-member and static functions for 2D Vectors
*
*	This source file includes overloads for non-member binary operators on 2D
*	Vectors, and other 2D Vector related functions. 
*
******************************************************************************/

#include "vmath.h"
#include "graphics.h"

namespace vmath {

	/**************************************************************************/
	/*!
		Binary Operators
	*/
	/**************************************************************************/
	Vector2	operator+(const Vector2& lhs, const Vector2& rhs) {
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	Vector2	operator-(const Vector2& lhs, const Vector2& rhs) {
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	Vector2	operator*(const Vector2& lhs, float rhs) {
		return { lhs.x * rhs, lhs.y * rhs };
	}

	Vector2	operator*(float lhs, const Vector2& rhs) {
		return { lhs * rhs.x, lhs * rhs.y };
	}

	Vector2	operator/(const Vector2& lhs, float rhs) {
		return { lhs.x / rhs, lhs.y / rhs };
	}

	/**************************************************************************/
	/*!
		Returns the point projected from a point onto a line
	*/
	/**************************************************************************/
	Vector2 Vector2::ProjectedPointOnLine(Vector2 head, Vector2 tail, Vector2 point) {
		
		head.x += GRAPHICS::w / 2.f;
		head.y += GRAPHICS::h / 2.f;
		tail.x += GRAPHICS::w / 2.f;
		tail.y += GRAPHICS::h / 2.f;
		point.x += GRAPHICS::w / 2.f;
		point.y += GRAPHICS::h / 2.f;
		
		Vector2 line = head - tail;
		Vector2 pointToLine = point - tail;
		float ratio = dot(line, pointToLine) / LengthSquared(line);
		Vector2 result = tail + ratio * line;
		return { result.x - (GRAPHICS::w / 2.f), result.y - (GRAPHICS::h / 2.f) };
	}

	/**************************************************************************/
	/*!
		Checks whether point is outside (on the side of the outward normal)
	*/
	/**************************************************************************/
	bool Vector2::IsPointOutside(Vector2 head, Vector2 tail, Vector2 point) {
		
		head.x += GRAPHICS::w / 2.f;
		head.y += GRAPHICS::h / 2.f;
		tail.x += GRAPHICS::w / 2.f;
		tail.y += GRAPHICS::h / 2.f;
		point.x += GRAPHICS::w / 2.f;
		point.y += GRAPHICS::h / 2.f;

		Vector2 line = head - tail;
		Vector2 pointToLine = point - tail;
		Vector2 outwardNormal = Vector2::GetOutwardNormal(line);
		return (Vector2::dot(outwardNormal, pointToLine) > 0.f) ? true : false;
	}

} // namespace vmath