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
*	@file		Utilities.cpp
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
*	@date		3 November 2023
*
* *****************************************************************************
*
*	@brief		Helper Functions
*
*	This file contains helper functions that are used in multiple source files.
*
******************************************************************************/



#include "model.h"
#include "Utilities.h"
#include "vmath.h"

/******************************************************************************
*
*	@brief Checks whether the cursor is within the bounds of an object
*
*	-
*
******************************************************************************/
bool IsWithinObject(Model& model, vmath::Vector2 cursorPos) {

	if ( vmath::Vector2::dot(vmath::Vector2::GetOutwardNormal(vmath::Vector2::VectorFromTwoPoints(model.GetTopLeft(), model.GetTopRight())), vmath::Vector2::VectorFromTwoPoints(cursorPos, model.GetTopRight())) > 0.f ||
		vmath::Vector2::dot(vmath::Vector2::GetOutwardNormal(vmath::Vector2::VectorFromTwoPoints(model.GetBotLeft(), model.GetTopLeft())), vmath::Vector2::VectorFromTwoPoints(cursorPos, model.GetTopLeft())) > 0.f ||
		vmath::Vector2::dot(vmath::Vector2::GetOutwardNormal(vmath::Vector2::VectorFromTwoPoints(model.GetBotRight(), model.GetBotLeft())), vmath::Vector2::VectorFromTwoPoints(cursorPos, model.GetBotLeft())) > 0.f ||
		vmath::Vector2::dot(vmath::Vector2::GetOutwardNormal(vmath::Vector2::VectorFromTwoPoints(model.GetTopRight(), model.GetBotRight())), vmath::Vector2::VectorFromTwoPoints(cursorPos, model.GetBotRight())) > 0.f
		)
	{
		return false;
	}
	return true;

	//if (cursorPos.x < model.GetMin().x || cursorPos.x > model.GetMax().x || cursorPos.y < model.GetMin().y || cursorPos.y > model.GetMax().y) {
	//	return false;
	//}
	//return true;
}

/******************************************************************************
*
*	@brief Calculates the offset between two points
*
*	Calculates the offset of Point 2 from Point 1
*
******************************************************************************/
vmath::Vector2 GetOffset(vmath::Vector2 point1, vmath::Vector2 point2) {
	return { point2.x - point1.x, point2.y - point1.y };
}

/******************************************************************************
*
*	@brief Checks whether two points are within a certain distance
*
*	-
*
******************************************************************************/
bool IsNearby(vmath::Vector2 point1, vmath::Vector2 point2, float distance) {
	if ((((point1.x - point2.x) * (point1.x - point2.x)) + ((point1.y - point2.y) * (point1.y - point2.y))) <= (distance * distance)) {
		return true;
	}
	return false;
}