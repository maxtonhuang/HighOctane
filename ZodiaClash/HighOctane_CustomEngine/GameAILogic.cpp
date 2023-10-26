
//----------------------------------------------------------------------------------------
//DEFINES FOR AI WEIGHTS

const int playerWeight = 1000;
const int enemyWeight = 1000;
const float damageRate = 1.f;
const float healRate = 1.f;
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
				playerChange++;
				effectiveDamage -= c.stats.health;
			}
			if (c.tag == CharacterType::ENEMY) {
				enemyChange++;
				effectiveHealing += c.stats.health;
			}
		}
		for (CharacterStats const& c : start.turnManage.characterList) {
			if (c.tag == CharacterType::PLAYER) {
				playerChange--;
				effectiveDamage += c.stats.health;
			}
			if (c.tag == CharacterType::ENEMY) {
				enemyChange--;
				effectiveHealing -= c.stats.health;
			}
		}
		value += playerWeight * playerChange;
		value += enemyWeight * enemyChange;
		value += (int)(effectiveDamage * damageRate);
		value += (int)(effectiveHealing * healRate);

		return value;
	}
}
