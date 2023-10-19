
//----------------------------------------------------------------------------------------
//DEFINES FOR AI WEIGHTS

const int playerWeight = 1000;
const int enemyWeight = 1000;
const float damageRate = 1.f;
const float healRate = 1.f;
//----------------------------------------------------------------------------------------

#include "GameAILogic.h"

void Gamestate::Update() {
	if (playerCharacters.size() == 0) {
		gameStatus = 2;
	}
	if (enemyCharacters.size() == 2) {
		gameStatus = 1;
	}
}

namespace GameAILogic {
	int Evaluate(Gamestate start, Gamestate end) {
		int value = 0;

		value += playerWeight * (int)(end.playerCharacters.size() - start.playerCharacters.size());
		value += enemyWeight * (int)(end.enemyCharacters.size() - start.enemyCharacters.size());

		float effectiveDamage = 0;
		for (Character const& c : start.playerCharacters) {
			effectiveDamage += c.health;
		}
		for (Character const& c : end.playerCharacters) {
			effectiveDamage -= c.health;
		}
		value += (int)(effectiveDamage * damageRate);

		float effectiveHealing = 0;
		for (Character const& c : end.enemyCharacters) {
			effectiveHealing += c.health;
		}
		for (Character const& c : start.enemyCharacters) {
			effectiveHealing -= c.health;
		}
		value += (int)(effectiveHealing * healRate);

		return value;
	}
}
