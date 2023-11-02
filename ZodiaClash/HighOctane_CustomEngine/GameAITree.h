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
*	@file		GameAITree.h
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
*	This file contains AI for the game using an evaluation decision tree
*	calculating all possible moves
*
*	Works as follows:
*	For all possible moves and targets, create a new node and increase its
*	depth counter, and add it to list of nodes to continue branching if
*	the game state is still ongoing. After that, advance the gamestate
*	stored in the node to the next turn.
*
*	Continue branching nodes until all nodes reaches the MAXDEPTH value or
*	until the AI finds a state that it can win
*
*	If the AI finds a win state, it chooses that move.
*	If not, it evaluates all the final nodes in all branches and then
*	picks the node with the highest evaluated value.
*
******************************************************************************/

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
