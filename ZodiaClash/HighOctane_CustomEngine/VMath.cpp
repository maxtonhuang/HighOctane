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