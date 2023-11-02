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
*	@file		GameAILogic.h
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
*	@date		19 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains evaluation formula used for the
*	AI to calculate the best possible move
*
*	PRIORITISED TO BE SCRIPTED SO THAT DESIGNER WILL BE ABLE TO HAVE
*	MORE CONTROL OVER THE AI
******************************************************************************/

#pragma once
#include <list>
#include "Battle.h"

namespace GameAILogic {
	int Evaluate(BattleSystem const& start, BattleSystem const& end);
};
