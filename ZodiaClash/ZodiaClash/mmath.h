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

More important overloads
' ^ ' dot product
' % ' cross product
' ~ ' inverse
' magnitude() ' magnitude
' normalize() ' normalize
' * ' scalar product
' / ' scalar division
' == ' equality
' != ' inequality
' ! ' boolean not
' [] ' subscript


*//*______________________________________________________________________*/
#pragma once

#include "commonheaderfilelist.h"
#include "vmath.h"

#include <cmath>
#include <iostream>

namespace mmath {
	class Matrix2 {
		public: 
			struct {
				vmath::Vector2 vector{0, 10};
				float m00, m01;
				float m10, m11;
		};
	};
}

