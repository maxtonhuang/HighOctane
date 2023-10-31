#pragma once
#include <vector>
#include "GameAILogic.h"
#include "CharacterStats.h"
#include "Attack.h"

class Node {
public:
	Node(BattleSystem const& initial); //PARENT CONSTRUCTOR
	Node(Node* previous);	 //CHILD CONSTRUCTOR
	Node* GetFront();
	Node* GetChosen();
	std::vector<Node*> GetBack(int* nodeCount = nullptr);
	BattleSystem battlesystem{};
	Entity selectedTarget;
	CharacterStats* nodeCharacter;
	Node* previous{};
	std::vector<Node> next{};
	size_t depth{};
	int eval{};
private:
	void Advance(Node* node, std::vector<Node*>& input, int* nodeCount);
};

class TreeManager {
public:
	//void Initialize(BattleSystem startState);
	void Search(BattleSystem* start);
	//std::vector<Node> parents;
private:
	void MakeDecision(Node*);
	BattleSystem* original;
};
