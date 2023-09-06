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
' [] ' subscript

' negate() ' negate matrix
' inverse() ' inverse matrix

TO DO maybe
' magnitude() ' magnitude
' normalize() ' normalize
*//*______________________________________________________________________*/
#pragma once

#include "CommonHeaderFileList.h"
#include "VMath.h"

#include <cmath>
#include <iostream>

namespace mmath {
	class Matrix2 {
		public: 
			struct {
				float m00, m01;
				float m10, m11;
			};

		// First constructor
		Matrix2() {
			m00 = 0, m01 = 0;
			m10 = 0, m11 = 0;
		}

		// Second constructor
		Matrix2(float m00In, float m01In, float m10In, float m11In) : 
			m00{ m00In }, m01{ m01In }, 
			m10{ m10In }, m11{ m11In } 
		{ }

		// Copy constructor
		Matrix2(const Matrix2& b) {
			this->m00 = b.m00;
			this->m01 = b.m01;
			this->m10 = b.m10;
			this->m11 = b.m11;
		}

		// Overloading =
		Matrix2& operator=(const Matrix2& b) {
			this->m00 = b.m00;
			this->m01 = b.m01;
			this->m10 = b.m10;
			this->m11 = b.m11;
			return *this;
		}

		// Overloading +
		Matrix2 operator+(const Matrix2& b) {
			Matrix2 temp;
			temp.m00 = this->m00 + b.m00;
			temp.m01 = this->m01 + b.m01;
			temp.m10 = this->m10 + b.m10;
			temp.m11 = this->m11 + b.m11;
			return temp;
		}

		// Overloading +=
		Matrix2& operator+=(const Matrix2& b) {
			this->m00 += b.m00;
			this->m01 += b.m01;
			this->m10 += b.m10;
			this->m11 += b.m11;
			return *this;
		}

		// Overloading -
		Matrix2 operator-(const Matrix2& b) {
			Matrix2 temp;
			temp.m00 = this->m00 - b.m00;
			temp.m01 = this->m01 - b.m01;
			temp.m10 = this->m10 - b.m10;
			temp.m11 = this->m11 - b.m11;
			return temp;
		}

		// Overloading -=
		Matrix2& operator-=(const Matrix2& b) {
			this->m00 -= b.m00;
			this->m01 -= b.m01;
			this->m10 -= b.m10;
			this->m11 -= b.m11;
			return *this;
		}

		// Overloading * with another matrix
		Matrix2 operator*(const Matrix2& b) {
			Matrix2 temp;
			temp.m00 = this->m00 * b.m00 + this->m01 * b.m10;
			temp.m01 = this->m00 * b.m01 + this->m01 * b.m11;
			temp.m10 = this->m10 * b.m00 + this->m11 * b.m10;
			temp.m11 = this->m10 * b.m01 + this->m11 * b.m11;
			return temp;
		}

		// Overloading *= with another matrix
		Matrix2& operator*=(const Matrix2& b) {
			this->m00 = this->m00 * b.m00 + this->m01 * b.m10;
			this->m01 = this->m00 * b.m01 + this->m01 * b.m11;
			this->m10 = this->m10 * b.m00 + this->m11 * b.m10;
			this->m11 = this->m10 * b.m01 + this->m11 * b.m11;
			return *this;
		}

		// Overloading * with a float
		Matrix2 operator*(const float& b) {
			Matrix2 temp;
			temp.m00 = this->m00 * b;
			temp.m01 = this->m01 * b;
			temp.m10 = this->m10 * b;
			temp.m11 = this->m11 * b;
			return temp;
		}

		// Overloading *= with a float
		Matrix2& operator*=(const float& b) {
			this->m00 *= b;
			this->m01 *= b;
			this->m10 *= b;
			this->m11 *= b;
			return *this;
		}

		// Overloading / with a float
		Matrix2 operator/(const float& b) {
			Matrix2 temp;
			temp.m00 = this->m00 / b;
			temp.m01 = this->m01 / b;
			temp.m10 = this->m10 / b;
			temp.m11 = this->m11 / b;
			return temp;
		}

		// Overloading /= with a float
		Matrix2& operator/=(const float& b) {
			this->m00 /= b;
			this->m01 /= b;
			this->m10 /= b;
			this->m11 /= b;
			return *this;
		}
		
		// Overloading ^ for dot product
		float operator^(const Matrix2& b) {
			float temp;
			temp = this->m00 * b.m00 + this->m01 * b.m01 + this->m10 * b.m10 + this->m11 * b.m11;
		}

		// Negate matrix
		Matrix2 negate() {
			Matrix2 temp;
			temp.m00 = -this->m00;
			temp.m01 = -this->m01;
			temp.m10 = -this->m10;
			temp.m11 = -this->m11;
			return temp;
		}

		// Determinant of the matrix
		float determinant() {
			float det = this->m00 * this->m11 - this->m01 * this->m10;
			return det;
		}
		
		// Overloading of ~ for inverse
		Matrix2 inverse() {
			Matrix2 temp;

			float det = this->m00 * this->m11 - this->m01 * this->m10;

			temp.m00 = this->m11;
			temp.m01 = -this->m01;
			temp.m10 = -this->m10;
			temp.m11 = this->m00;

			temp /= det;
			return temp;
		}

		/*----------------------------------------------------
					Mostly for debugging purposes
		----------------------------------------------------*/

		// Overloading ==
		bool operator==(const Matrix2& b) {
			if (this->m00 == b.m00 && this->m01 == b.m01 && this->m10 == b.m10 && this->m11 == b.m11) {
				return true;
			}
			else {
				return false;
			}
		}

		// Overloading !=
		bool operator!=(const Matrix2& b) {
			if (this->m00 != b.m00 || this->m01 != b.m01 || this->m10 != b.m10 || this->m11 != b.m11) {
				return true;
			}
			else {
				return false;
			}
		}

		// Overloading []
		float& operator[](const int& index) {
			if (index == 0) {
				return m00;
			}
			else if (index == 1) {
				return m01;
			}
			else if (index == 2) {
				return m10;
			}
			else if (index == 3) {
				return m11;
			}
			else {
				std::cout << "Error: Index out of range" << std::endl;
				return m00;
			}
		}

		// Overloading <<
		friend std::ostream& operator<<(std::ostream& os, const Matrix2& b) {
			os << b.m00 << " " << b.m01 << std::endl;
			os << b.m10 << " " << b.m11 << std::endl;
			return os;
		}

		// Overloading >>
		friend std::istream& operator>>(std::istream& is, Matrix2& b) {
			is >> b.m00 >> b.m01 >> b.m10 >> b.m11;
			return is;
		}

	};


	class Matrix3 {
		public:
			struct {
				float m00, m01, m02;
				float m10, m11, m12;
				float m20, m21, m22;
			};

			// First constructor
			Matrix3() {
				m00 = 0, m01 = 0, m02 = 0;
				m10 = 0, m11 = 0, m12 = 0;
				m20 = 0, m21 = 0, m22 = 0;
			}

			// Second constructor
			Matrix3(float m00In, float m01In, float m02In, float m10In, float m11In, float m12In, float m20In, float m21In, float m22In) : 
				m00{ m00In }, m01{ m01In }, m02{ m02In }, 
				m10{ m10In }, m11{ m11In }, m12{ m12In }, 
				m20{ m20In }, m21{ m21In }, m22{ m22In } 
			{ }

			// Copy constructor
			Matrix3(const Matrix3& b) {
				this->m00 = b.m00, this->m01 = b.m01, this->m02 = b.m02;
				this->m10 = b.m10, this->m11 = b.m11, this->m12 = b.m12;
				this->m20 = b.m20, this->m21 = b.m21, this->m22 = b.m22;
			}

			// Overloading =
			Matrix3& operator=(const Matrix3& b) {
				this->m00 = b.m00, this->m01 = b.m01, this->m02 = b.m02;
				this->m10 = b.m10, this->m11 = b.m11, this->m12 = b.m12;
				this->m20 = b.m20, this->m21 = b.m21, this->m22 = b.m22;
				return *this;
			}

			// Overloading + with another matrix
			Matrix3 operator+(const Matrix3& b) {
				Matrix3 temp;
				temp.m00 = this->m00 + b.m00;
				temp.m01 = this->m01 + b.m01;
				temp.m02 = this->m02 + b.m02;
				temp.m10 = this->m10 + b.m10;
				temp.m11 = this->m11 + b.m11;
				temp.m12 = this->m12 + b.m12;
				temp.m20 = this->m20 + b.m20;
				temp.m21 = this->m21 + b.m21;
				temp.m22 = this->m22 + b.m22;
				return temp;
			}

			// Overloading += with another matrix
			Matrix3& operator+=(const Matrix3& b) {
				this->m00 += b.m00;
				this->m01 += b.m01;
				this->m02 += b.m02;
				this->m10 += b.m10;
				this->m11 += b.m11;
				this->m12 += b.m12;
				this->m20 += b.m20;
				this->m21 += b.m21;
				this->m22 += b.m22;
				return *this;
			}

			// Overloading - with another matrix
			Matrix3 operator-(const Matrix3& b) {
				Matrix3 temp;
				temp.m00 = this->m00 - b.m00;
				temp.m01 = this->m01 - b.m01;
				temp.m02 = this->m02 - b.m02;
				temp.m10 = this->m10 - b.m10;
				temp.m11 = this->m11 - b.m11;
				temp.m12 = this->m12 - b.m12;
				temp.m20 = this->m20 - b.m20;
				temp.m21 = this->m21 - b.m21;
				temp.m22 = this->m22 - b.m22;
				return temp;
			}

			// Overloading -= with another matrix
			Matrix3& operator-=(const Matrix3& b) {
				this->m00 -= b.m00;
				this->m01 -= b.m01;
				this->m02 -= b.m02;
				this->m10 -= b.m10;
				this->m11 -= b.m11;
				this->m12 -= b.m12;
				this->m20 -= b.m20;
				this->m21 -= b.m21;
				this->m22 -= b.m22;
				return *this;
			}

			// Overloading * with another matrix
			Matrix3 operator*(const Matrix3& b) {
				Matrix3 temp;
				temp.m00 = this->m00 * b.m00 + this->m01 * b.m10 + this->m02 * b.m20;
				temp.m01 = this->m00 * b.m01 + this->m01 * b.m11 + this->m02 * b.m21;
				temp.m02 = this->m00 * b.m02 + this->m01 * b.m12 + this->m02 * b.m22;
				temp.m10 = this->m10 * b.m00 + this->m11 * b.m10 + this->m12 * b.m20;
				temp.m11 = this->m10 * b.m01 + this->m11 * b.m11 + this->m12 * b.m21;
				temp.m12 = this->m10 * b.m02 + this->m11 * b.m12 + this->m12 * b.m22;
				temp.m20 = this->m20 * b.m00 + this->m21 * b.m10 + this->m22 * b.m20;
				temp.m21 = this->m20 * b.m01 + this->m21 * b.m11 + this->m22 * b.m21;
				temp.m22 = this->m20 * b.m02 + this->m21 * b.m12 + this->m22 * b.m22;
				return temp;
			}

			// Overloading *= with another matrix
			Matrix3& operator*=(const Matrix3& b) {
				Matrix3 temp;
				temp.m00 = this->m00 * b.m00 + this->m01 * b.m10 + this->m02 * b.m20;
				temp.m01 = this->m00 * b.m01 + this->m01 * b.m11 + this->m02 * b.m21;
				temp.m02 = this->m00 * b.m02 + this->m01 * b.m12 + this->m02 * b.m22;
				temp.m10 = this->m10 * b.m00 + this->m11 * b.m10 + this->m12 * b.m20;
				temp.m11 = this->m10 * b.m01 + this->m11 * b.m11 + this->m12 * b.m21;
				temp.m12 = this->m10 * b.m02 + this->m11 * b.m12 + this->m12 * b.m22;
				temp.m20 = this->m20 * b.m00 + this->m21 * b.m10 + this->m22 * b.m20;
				temp.m21 = this->m20 * b.m01 + this->m21 * b.m11 + this->m22 * b.m21;
				temp.m22 = this->m20 * b.m02 + this->m21 * b.m12 + this->m22 * b.m22;
				*this = temp;
				return *this;
			}

			// Overloading * with a vector
			vmath::Vector3 operator*(const vmath::Vector3& b) {
				vmath::Vector3 temp;
				temp.x = this->m00 * b.x + this->m01 * b.y + this->m02 * b.z;
				temp.y = this->m10 * b.x + this->m11 * b.y + this->m12 * b.z;
				temp.z = this->m20 * b.x + this->m21 * b.y + this->m22 * b.z;
				return temp;
			}

			// Overloading * with a scalar
			Matrix3 operator*(const float& b) {
				Matrix3 temp;
				temp.m00 = this->m00 * b;
				temp.m01 = this->m01 * b;
				temp.m02 = this->m02 * b;
				temp.m10 = this->m10 * b;
				temp.m11 = this->m11 * b;
				temp.m12 = this->m12 * b;
				temp.m20 = this->m20 * b;
				temp.m21 = this->m21 * b;
				temp.m22 = this->m22 * b;
				return temp;
			}

			// Overloading *= with a scalar
			Matrix3& operator*=(const float& b) {
				this->m00 *= b;
				this->m01 *= b;
				this->m02 *= b;
				this->m10 *= b;
				this->m11 *= b;
				this->m12 *= b;
				this->m20 *= b;
				this->m21 *= b;
				this->m22 *= b;
				return *this;
			}

			// Overloading / with a scalar
			Matrix3 operator/(const float& b) {
				Matrix3 temp;
				temp.m00 = this->m00 / b;
				temp.m01 = this->m01 / b;
				temp.m02 = this->m02 / b;
				temp.m10 = this->m10 / b;
				temp.m11 = this->m11 / b;
				temp.m12 = this->m12 / b;
				temp.m20 = this->m20 / b;
				temp.m21 = this->m21 / b;
				temp.m22 = this->m22 / b;
				return temp;
			}

			// Overloading /= with a scalar
			Matrix3& operator/=(const float& b) {
				this->m00 /= b;
				this->m01 /= b;
				this->m02 /= b;
				this->m10 /= b;
				this->m11 /= b;
				this->m12 /= b;
				this->m20 /= b;
				this->m21 /= b;
				this->m22 /= b;
				return *this;
			}

			// Overloading ^ for dot product
			float operator^(const Matrix3& b) {
				float temp;
				temp = this->m00 * b.m00 + this->m01 * b.m01 + this->m02 * b.m02 + this->m10 * b.m10 + this->m11 * b.m11 + this->m12 * b.m12 + this->m20 * b.m20 + this->m21 * b.m21 + this->m22 * b.m22;
				return temp;
			}

			// Overloading % for cross product
			Matrix3 operator%(const Matrix3& b) {
				Matrix3 temp;
				temp.m00 = this->m01 * b.m02 - this->m02 * b.m01;
				temp.m01 = this->m02 * b.m00 - this->m00 * b.m02;
				temp.m02 = this->m00 * b.m01 - this->m01 * b.m00;
				temp.m10 = this->m11 * b.m12 - this->m12 * b.m11;
				temp.m11 = this->m12 * b.m10 - this->m10 * b.m12;
				temp.m12 = this->m10 * b.m11 - this->m11 * b.m10;
				temp.m20 = this->m21 * b.m22 - this->m22 * b.m21;
				temp.m21 = this->m22 * b.m20 - this->m20 * b.m22;
				temp.m22 = this->m20 * b.m21 - this->m21 * b.m20;
				return temp;
			}

			// Negate a matrix
			Matrix3 negate() {
				Matrix3 temp;
				temp.m00 = -this->m00;
				temp.m01 = -this->m01;
				temp.m02 = -this->m02;
				temp.m10 = -this->m10;
				temp.m11 = -this->m11;
				temp.m12 = -this->m12;
				temp.m20 = -this->m20;
				temp.m21 = -this->m21;
				temp.m22 = -this->m22;
				return temp;
			}

			// Determinant of a matrix
			float determinant() {
				float det = this->m00 * (this->m11 * this->m22 - this->m12 * this->m21) - this->m01 * (this->m10 * this->m22 - this->m12 * this->m20) + this->m02 * (this->m10 * this->m21 - this->m11 * this->m20);
				return det;
			}

			// inverse a matrix
			Matrix3 inverse() {
				Matrix3 temp;
				float det = this->determinant();
				if (det == 0) {
					return temp;
				}
				else {
					temp.m00 = this->m11 * this->m22 - this->m12 * this->m21;
					temp.m01 = this->m02 * this->m21 - this->m01 * this->m22;
					temp.m02 = this->m01 * this->m12 - this->m02 * this->m11;
					temp.m10 = this->m12 * this->m20 - this->m10 * this->m22;
					temp.m11 = this->m00 * this->m22 - this->m02 * this->m20;
					temp.m12 = this->m02 * this->m10 - this->m00 * this->m12;
					temp.m20 = this->m10 * this->m21 - this->m11 * this->m20;
					temp.m21 = this->m01 * this->m20 - this->m00 * this->m21;
					temp.m22 = this->m00 * this->m11 - this->m01 * this->m10;
					
					temp /= det;
					return temp;
				}
			}

			/*----------------------------------------------------
						Mostly for debugging purposes
			----------------------------------------------------*/


			// Overloading == with another matrix
			bool operator==(const Matrix3& b) {
				if (this->m00 == b.m00 && this->m01 == b.m01 && this->m02 == b.m02 && this->m10 == b.m10 && this->m11 == b.m11 && this->m12 == b.m12 && this->m20 == b.m20 && this->m21 == b.m21 && this->m22 == b.m22) {
					return true;
				}
				else {
					return false;
				}
			}

			// Overloading != with another matrix
			bool operator!=(const Matrix3& b) {
				if (this->m00 != b.m00 || this->m01 != b.m01 || this->m02 != b.m02 || this->m10 != b.m10 || this->m11 != b.m11 || this->m12 != b.m12 || this->m20 != b.m20 || this->m21 != b.m21 || this->m22 != b.m22) {
					return true;
				}
				else {
					return false;
				}
			}

			// Overloading << with a matrix
			friend std::ostream& operator<<(std::ostream& os, const Matrix3& b) {
				os << b.m00 << " " << b.m01 << " " << b.m02 << std::endl;
				os << b.m10 << " " << b.m11 << " " << b.m12 << std::endl;
				os << b.m20 << " " << b.m21 << " " << b.m22 << std::endl;
				return os;
			}

			// Overloading >> with a matrix
			friend std::istream& operator>>(std::istream& is, Matrix3& b) {
				is >> b.m00 >> b.m01 >> b.m02 >> b.m10 >> b.m11 >> b.m12 >> b.m20 >> b.m21 >> b.m22;
				return is;
			}
	};
}

