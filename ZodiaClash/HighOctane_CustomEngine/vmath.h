/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		vmath.h
@author		Liu Wan Ting
@Email		wanting.liu@digipen.edu
@course		CSD 2401
@section	Section A
@date		26/ 9/ 2023
@brief		This file contains the overloading of the vectors 2 and 3

IF ANYTHING IS CALCULATED WRONGLY PLEASE LET ME KNOW THANK YOU VERY MUCH

More important overloads
' % ' cross product
' * ' scalar product
' / ' scalar division
' == ' equality
' != ' inequality
' ! ' boolean not
' [] ' subscript

' dot()' dot product
' negate() ' negate vector
' magnitude() ' magnitude
' normalize() ' normalize
' midpoint(Vector2& b) ' midpoint of two vectors
' distance(Vector2& b) ' distance between two vectors
' inRange(Vector2& b, float distRange) ' test if the vector is in the range of the object

*//*______________________________________________________________________*/
#pragma once

#include "CommonHeaderFileList.h"
#include <cmath>
#include <iostream>

namespace vmath {

	constexpr float PI = 3.141592653589793238463f;
	constexpr float NINETYDEGREES = 1.5707963267948966192315f;

	/**************************************************************************/
	/*!
		@class Vector2
		@brief A class representing 2D vectors.

		This class provides utility functions and operators tailored for 2D
		mathematical vector operations
	*/
	/**************************************************************************/
	class Vector2 {
		public:
			float x;
			float y;

			// First constructor
			Vector2() {
				x = 0;
				y = 0;
			}

			// Second constructor
			Vector2(float xIn, float yIn) : x{ xIn }, y{ yIn } { }

			// Copy constructor
			Vector2(const Vector2& b) {
				this->x = b.x;
				this->y = b.y;
			}
			
			// Overloading =
			Vector2& operator=(const Vector2& b) {
				this->x = b.x;
				this->y = b.y;
				return *this;
			}

			// Overloading + and +=
			Vector2 operator+(const Vector2& b) {
				Vector2 vector;
				vector.x = this->x + b.x;
				vector.y = this->y + b.y;
				return vector;
			}

			Vector2& operator+=(const Vector2& b) {
				this->x += b.x;
				this->y += b.y;
				return *this;
			}

			// Overloading - and -=
			Vector2 operator-(const Vector2& b) {
				Vector2 vector;
				vector.x = this->x - b.x;
				vector.y = this->y - b.y;
				return vector;
			}

			Vector2& operator-=(const Vector2& b) {
				this->x -= b.x;
				this->y -= b.y;
				return *this;
			}

			// Overloading * and *=
			Vector2 operator*(float const& scale) {
				Vector2 vector;
				vector.x = this->x * scale;
				vector.y = this->y * scale;
				return vector;
			}

			Vector2& operator*=(float const& scale) {
				this->x *= scale;
				this->y *= scale;
				return *this;
			}

			// Overloading / and /=
			Vector2 operator/(float const& scale) {
				Vector2 vector;
				vector.x = this->x / scale;
				vector.y = this->y / scale;
				return vector;
			}

			Vector2& operator/=(float const& scale) {
				this->x /= scale;
				this->y /= scale;
				return *this;
			}

			/**************************************************************************/
			/*!
				In this function, it will return the negated vector
			*/
			/**************************************************************************/
			Vector2 negate() const
			{
				return Vector2(-x, -y);
			}

			/**************************************************************************/
			/*!
				In this function, it will return the magnitude of the vector
			*/
			/**************************************************************************/
			float magnitude() const {
				return static_cast<float>(sqrt(pow(this->x, 2) + pow(this->y, 2)));
			}
			
			/**************************************************************************/
			/*!
				This function returns the dot product between 2 vectors
			*/
			/**************************************************************************/
			static float dot(const Vector2& a, const Vector2& b) {
				return a.x * b.x + a.y * b.y;
			}

			/**************************************************************************/
			/*!
				In this function, it will return the unit vector
			*/
			/**************************************************************************/
			Vector2 normalize() const {
				Vector2 vector;
				vector.x = this->x / this->magnitude();
				vector.y = this->y / this->magnitude();
				return vector;
			}

			/**************************************************************************/
			/*!
				In this function, it will return the midpoint of 2 2D points
			*/
			/**************************************************************************/
			Vector2 midpoint(Vector2& b) const {
				Vector2 vector;
				vector.x = (this->x + b.x) / 2;
				vector.y = (this->y + b.y) / 2;
				return vector;
			}

			/**************************************************************************/
			/*!
				In this function, it will return the distance between 2 2D points 
			*/
			/**************************************************************************/
			float distance(Vector2& b) const {
				return static_cast<float>(sqrt(pow(this->x - b.x, 2) + pow(this->y - b.y, 2)));
			}

			/**************************************************************************/
			/*!
				This function will return the squared distance of a line.
			*/
			/**************************************************************************/
			static float LengthSquared(const Vector2& a) {
				return a.x * a.x + a.y * a.y;
			}

			/**************************************************************************/
			/*!
				Test if the vector is in the range of the object
			*/
			/**************************************************************************/
			bool inRange(Vector2& b, float distRange) const {
				if (this->distance(b) <= distRange) {
					return true;
				}
				else {
					return false;
				}
			}

			/**************************************************************************/
			/*!
				Returns the vector from two points
			*/
			/**************************************************************************/
			static Vector2 VectorFromTwoPoints(const Vector2& a, const Vector2& b) {
				return { a.x - b.x, a.y - b.y };
			}

			/**************************************************************************/
			/*!
				Returns the outward normal of the vector
			*/
			/**************************************************************************/
			static Vector2 GetOutwardNormal(const Vector2& a) {
				return { a.y, -(a.x) };
			}

			/**************************************************************************/
			/*!
				Returns the point projected from a point onto a line
			*/
			/**************************************************************************/
			static Vector2 ProjectedPointOnLine(Vector2 head, Vector2 tail, Vector2 point);

			/**************************************************************************/
			/*!
				Returns the distance between two points
			*/
			/**************************************************************************/
			static float DistanceBetweenPoints(const Vector2& a, const Vector2& b) {
				return sqrt(((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
			}

			/**************************************************************************/
			/*!
				Returns the squared distance between two points
			*/
			/**************************************************************************/
			static float SquaredDistanceBetweenPoints(const Vector2& a, const Vector2& b) {
				return (((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
			}


			static bool IsPointOutside(Vector2 head, Vector2 tail, Vector2 point);


			/*----------------------------------------------------
						Mostly for debugging purposes          
			----------------------------------------------------*/
			//overloading <<
			friend std::ostream& operator<<(std::ostream& os, const Vector2& b) {
				os << "(" << b.x << ", " << b.y << ")";
				return os;
			}

			//overloading >>
			friend std::istream& operator>>(std::istream& is, Vector2& b) {
				is >> b.x >> b.y;
				return is;
			}

			// Overloading ==
			bool operator==(const Vector2& b) {
				if (this->x == b.x && this->y == b.y) {
					return true;
				}
				else {
					return false;
				}
			}

			// Overloading !=
			bool operator!=(const Vector2& b) {
				if (this->x != b.x || this->y != b.y) {
					return true;
				}
				else {
					return false;
				}
			}

			//overloading []
			float& operator[](int index) {
				if (index == 0) {
					return this->x;
				}
				else if (index == 1) {
					return this->y;
				}
				else {
					std::cout << "Index out of range 1" << std::endl;
					return this->x;
				}
			}

			//overloading ()
			float operator()(int index) {
				if (index == 0) {
					return this->x;
				}
				else if (index == 1) {
					return this->y;
				}
				else {
					std::cout << "Index out of range 2" << std::endl;
					return this->x;
				}
			}

			//overloading !
			bool operator!() {
				if (this->x == 0 && this->y == 0) {
					return true;
				}
				else {
					return false;
				}
			}

			static std::string getName() {
				std::string test = typeid(Vector2).name();
				std::string retString = test.substr(6, test.length() - 6);
				return retString;
			}
	};


	/**************************************************************************/
	/*!
		Binary Operators
	*/
	/**************************************************************************/
	Vector2	operator+(const Vector2& lhs, const Vector2& rhs);

	Vector2	operator-(const Vector2& lhs, const Vector2& rhs);

	Vector2	operator*(const Vector2& lhs, float rhs);

	Vector2	operator*(float lhs, const Vector2& rhs);

	Vector2	operator/(const Vector2& lhs, float rhs);

	/**************************************************************************/
	/*!
		@class Vector3
		@brief A class representing 3D vectors.

		This class provides utility functions and operators tailored for 3D
		mathematical vector operations
	*/
	/**************************************************************************/
	class Vector3 {
		public:
			float x;
			float y;
			float z;

			// First constructor
			Vector3() {
				x = 0;
				y = 0;
				z = 0;
			}

			// Second constructor
			Vector3(float xIn, float yIn, float zIn) : x{ xIn }, y{ yIn }, z{ zIn } { }

			// Copy constructor
			Vector3(const Vector3& b) {
				this->x = b.x;
				this->y = b.y;
				this->z = b.z;
			}

			// Overloading =
			Vector3& operator=(const Vector3& b) {
				this->x = b.x;
				this->y = b.y;
				this->z = b.z;
				return *this;
			}

			// Overloading + and +=
			Vector3 operator+(const Vector3& b) {
				Vector3 vector;
				vector.x = this->x + b.x;
				vector.y = this->y + b.y;
				vector.z = this->z + b.z;
				return vector;
			}

			Vector3& operator+=(const Vector3& b) {
				this->x += b.x;
				this->y += b.y;
				this->z += b.z;
				return *this;
			}

			// Overloading - and -=
			Vector3 operator-(const Vector3& b) {
				Vector3 vector;
				vector.x = this->x - b.x;
				vector.y = this->y - b.y;
				vector.z = this->z - b.z;
				return vector;
			}

			Vector3& operator-=(const Vector3& b) {
				this->x -= b.x;
				this->y -= b.y;
				this->z -= b.z;
				return *this;
			}

			// Overloading * and *=
			Vector3 operator*(float const& scale) {
				Vector3 vector;
				vector.x = this->x * scale;
				vector.y = this->y * scale;
				vector.z = this->z * scale;
				return vector;
			}

			Vector3& operator*=(float const& scale) {
				this->x *= scale;
				this->y *= scale;
				this->z *= scale;
				return *this;
			}

			// Overloading / and /=
			Vector3 operator/(float const& scale) {
				Vector3 vector;
				vector.x = this->x / scale;
				vector.y = this->y / scale;
				vector.z = this->z / scale;
				return vector;
			}

			Vector3& operator/=(float const& scale) {
				this->x /= scale;
				this->y /= scale;
				this->z /= scale;
				return *this;
			}

			// Overload % for the cross product
			Vector3 operator%(const Vector3& b) {
				Vector3 vector;
				vector.x = this->y * b.z - this->z * b.y;
				vector.y = this->z * b.x - this->x * b.z;
				vector.z = this->x * b.y - this->y * b.x;
				return vector;
			}

			/**************************************************************************/
			/*!
				In this function, it will return the negated vector
			*/
			/**************************************************************************/
			Vector3 negate() const
			{
				return Vector3(-x, -y, -z);
			}

			/**************************************************************************/
			/*!
				This function returns the dot product between 2 vectors
			*/
			/**************************************************************************/
			static float dot(const Vector3& a, const Vector3& b) {
				return a.x * b.x + a.y * b.y + a.z * b.z;
			}

			/**************************************************************************/
			/*!
				In this function, it will return the magnitude of the vector
			*/
			/**************************************************************************/
			float magnitude() const {
				return static_cast<float>(sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2)));
			}

			/**************************************************************************/
			/*!
				In this function, it will return the unit vector
			*/
			/**************************************************************************/
			Vector3 normalize() const {
				Vector3 vector;
				vector.x = this->x / this->magnitude();
				vector.y = this->y / this->magnitude();
				vector.z = this->z / this->magnitude();
				return vector;
			}
			
			/**************************************************************************/
			/*!
				In this function, it will return the midpoint of 2 2D points
			*/
			/**************************************************************************/
			Vector3 midpoint(const Vector3& b) const {
				Vector3 vector;
				vector.x = (this->x + b.x) / 2;
				vector.y = (this->y + b.y) / 2;
				vector.z = (this->z + b.z) / 2;
				return vector;
			}

			/**************************************************************************/
			/*!
				In this function, it will return the distance between 2 2D points
			*/
			/**************************************************************************/
			float distance(const Vector3& b) const {
				return static_cast<float>(sqrt(pow(this->x - b.x, 2) + pow(this->y - b.y, 2) + pow(this->z - b.z, 2)));
			}

			/**************************************************************************/
			/*!
				Test if the vector is in the range of the object
			*/
			/**************************************************************************/
			bool inRange(const Vector3& b, float range) const {
				if (this->distance(b) <= range) {
					return true;
				}
				else {
					return false;
				}
			}

			/*----------------------------------------------------
						Mostly for debugging purposes
			----------------------------------------------------*/

			//overloading <<
			friend std::ostream& operator<<(std::ostream& os, const Vector3& b) {
				os << "(" << b.x << ", " << b.y << ", " << b.z << ")";
				return os;
			}

			//overloading >>
			friend std::istream& operator>>(std::istream& is, Vector3& b) {
				is >> b.x >> b.y >> b.z;
				return is;
			}

			// Overloading ==
			bool operator==(const Vector3& b) {
				if (this->x == b.x && this->y == b.y && this->z == b.z) {
					return true;
				}
				else {
					return false;
				}
			}

			// Overloading !=
			bool operator!=(const Vector3& b) {
				if (this->x != b.x || this->y != b.y || this->z != b.z) {
					return true;
				}
				else {
					return false;
				}
			}

			//overloading []
			float operator[](int index) {
				if (index == 0) {
					return this->x;
				}
				else if (index == 1) {
					return this->y;
				}
				else if (index == 2) {
					return this->z;
				}
				else {
					std::cout << "Index out of range 3" << std::endl;
					return this->x;
				}
			}

			//overloading !
			bool operator!() {
				if (this->x == 0 && this->y == 0 && this->z == 0) {
					return true;
				}
				else {
					return false;
				}
			}

			//overloading ()
			Vector3 operator()(float xIn, float yIn, float zIn) {
				Vector3 vector;
				vector.x = this->x + xIn;
				vector.y = this->y + yIn;
				vector.z = this->z + zIn;
				return vector;
			}

			static std::string getName() {
				std::string test = typeid(Vector3).name();
				std::string retString = test.substr(6, test.length() - 6);
				return retString;
			}
	};
}