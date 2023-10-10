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
*	@file		Viewport.h
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains class and functions for game viewport
*
******************************************************************************/

#pragma once
#include "graphlib.h"

class Viewport {
public:
	void SetViewport(int input_x, int input_y, unsigned int input_w, unsigned int input_h);

	int GetX();
	int GetY();
	unsigned int GetW();
	unsigned int GetH();
private:
	int x;
	int y;
	unsigned int w;
	unsigned int h;
};
