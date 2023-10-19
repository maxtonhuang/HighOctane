#pragma once
#include <list>

enum GAMESTATUS {
	INPROGRESS = 0, WIN, LOSE
};

class Attack {
public:
	bool AOE;
};

class Character {
public:
	float maxHealth;
	float health;
	float attack;
	float defense;
	int speed;
	std::list<Attack> attacks;
};

class Gamestate {
public:
	void Update();
	std::list<Character> playerCharacters;
	std::list<Character> enemyCharacters;
	Character* currentCharacter;
	int gameStatus;
};

namespace GameAILogic {
	int Evaluate(Gamestate start, Gamestate end);
};
