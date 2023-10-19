#pragma once
#include <vector>
#include "GameAILogic.h"

class Node {
public:
	Node(Gamestate initial); //PARENT CONSTRUCTOR
	Node(Node* previous);	 //CHILD CONSTRUCTOR
	~Node();
	Node* GetFront();
	std::vector<Node*> GetBack();
	Gamestate previousState{};
	Gamestate currentState{};
	Attack selectedAttack;
	Character selectedTarget;
	Node* previous{};
	std::vector<Node*> next{};
	size_t depth{};
	int eval{};
private:
	void Advance(Node* node, std::vector<Node*> input);
};

class TreeManager {
public:
	void Initialize(Gamestate startState);
	void Search(Gamestate const& start, Attack& selectedAttack, Character& selectedTarget);
	std::vector<Node> parents;
};
