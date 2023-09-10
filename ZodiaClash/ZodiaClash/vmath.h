/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		vmath.h
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		30 August 2023
@brief		This file contains the overloading of the vectors 2 and 3

IF ANYTHING IS CALCULATED WRONGLY PLEASE LET ME KNOW THANK YOU VERY MUCH

More important overloads
' ^ ' dot product
' % ' cross product
' * ' scalar product
' / ' scalar division
' == ' equality
' != ' inequality
' ! ' boolean not
' [] ' subscript

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
		
			// Overloading ^ for dot product
			float operator^(const Vector2& b) {
				return this->x * b.x + this->y * b.y;
			}

			// Negation
			Vector2 negate()
			{
				return Vector2(-x, -y);
			}

			// Magnitude of the vector
			float magnitude() {
				return static_cast<float>(sqrt(pow(this->x, 2) + pow(this->y, 2)));
			}
			
			// Normalize the vector
			Vector2 normalize() {
				Vector2 vector;
				vector.x = this->x / this->magnitude();
				vector.y = this->y / this->magnitude();
				return vector;
			}

			// Mid point of two vectors
			Vector2 midpoint(Vector2& b) {
				Vector2 vector;
				vector.x = (this->x + b.x) / 2;
				vector.y = (this->y + b.y) / 2;
				return vector;
			}

			// Distance between two vectors
			float distance(Vector2& b) {
				return static_cast<float>(sqrt(pow(this->x - b.x, 2) + pow(this->y - b.y, 2)));
			}

			// Test if the vector is in the range of the object
			bool inRange(Vector2& b, float distRange) {
				if (this->distance(b) <= distRange) {
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
					std::cout << "Index out of range" << std::endl;
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
					std::cout << "Index out of range" << std::endl;
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
	};

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

			// Overloading ^ for dot product
			float operator^(const Vector3& b) {
				return this->x * b.x + this->y * b.y + this->z * b.z;
			}

			// Overload % for the cross product
			Vector3 operator%(const Vector3& b) {
				Vector3 vector;
				vector.x = this->y * b.z - this->z * b.y;
				vector.y = this->z * b.x - this->x * b.z;
				vector.z = this->x * b.y - this->y * b.x;
				return vector;
			}

			// Negation vector
			Vector3 negate()
			{
				return Vector3(-x, -y, -z);
			}

			// Magnitude of the vector
			float magnitude() {
				return static_cast<float>(sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2)));
			}

			// Normalize the vector
			Vector3 normalize() {
				Vector3 vector;
				vector.x = this->x / this->magnitude();
				vector.y = this->y / this->magnitude();
				vector.z = this->z / this->magnitude();
				return vector;
			}
			
			// Mid point of two vectors
			Vector3 midpoint(const Vector3& b) {
				Vector3 vector;
				vector.x = (this->x + b.x) / 2;
				vector.y = (this->y + b.y) / 2;
				vector.z = (this->z + b.z) / 2;
				return vector;
			}

			// Distance between two vectors
			float distance(const Vector3& b) {
				return static_cast<float>(sqrt(pow(this->x - b.x, 2) + pow(this->y - b.y, 2) + pow(this->z - b.z, 2)));
			}

			// Test if the vector is in the range of the object
			bool inRange(const Vector3& b, float range) {
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
					std::cout << "Index out of range" << std::endl;
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
	};
}