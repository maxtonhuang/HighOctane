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
*	@file		GameAILogic.cpp
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

//----------------------------------------------------------------------------------------
//DEFINES FOR AI WEIGHTS

const int playerWeight = 1000;
const int enemyWeight = 1000;
const float damageRate = 10.f;
const float healRate = 10000.f;
const float speedupRate = 100000.f;
//----------------------------------------------------------------------------------------

#include "GameAILogic.h"
#include "CharacterStats.h"

namespace GameAILogic {
	int Evaluate(BattleSystem const& start, BattleSystem const& end) {
		int value = 0;

		int playerChange = 0;
		int enemyChange = 0;
		float effectiveDamage = 0;
		float effectiveHealing = 0;
		for (CharacterStats const& c : end.turnManage.characterList) {
			if (c.tag == CharacterType::PLAYER) {
				if (c.stats.health != 0) {
					playerChange++;
				}
				effectiveDamage -= c.stats.health;
			}
			if (c.tag == CharacterType::ENEMY) {
				if (c.stats.health != 0) {
					enemyChange++;
				}
				effectiveHealing += c.stats.health;
			}
		}
		for (CharacterStats const& c : start.turnManage.characterList) {
			if (c.tag == CharacterType::PLAYER) {
				if (c.stats.health != 0) {
					playerChange--;
				}
				effectiveDamage += c.stats.health;
			}
			if (c.tag == CharacterType::ENEMY) {
				if (c.stats.health != 0) {
					enemyChange--;
				}
				effectiveHealing -= c.stats.health;
			}
		}
		value += playerWeight * (-playerChange);
		value += enemyWeight * enemyChange;
		value += (int)(effectiveDamage * damageRate);
		value += (int)(effectiveHealing * healRate);

		if (end.speedup && !start.speedup && end.speedupCharacter->tag == CharacterType::ENEMY) {
			value += speedupRate;
		}

		return value;
	}
}
