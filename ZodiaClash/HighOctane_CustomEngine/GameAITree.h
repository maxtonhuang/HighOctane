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
	//Constructor for the initial node of the AI tree
	Node(BattleSystem const& initial);

	//Constructor for the child nodes of the AI tree
	Node(Node* previous);

	//Gets the frontmost node of the tree
	Node* GetFront();

	//Gets the node that the AI tree has chosen as its move
	Node* GetChosen();

	//Gets the backmost nodes of the AI tree, returns the amount of nodes in nodeCount
	std::vector<Node*> GetBack(int* nodeCount = nullptr);

	//Copy of the battle system stored in this node
	BattleSystem battlesystem{};

	//Selected entity for this node
	Entity selectedTarget;

	//Selected target for this node
	CharacterStats* nodeCharacter;

	//Previous node
	Node* previous{};

	//Next node
	std::vector<Node> next{};

	//Current depth in the AI tree
	size_t depth{};

	//Evaluation score of the node
	int eval{};
private:
	//Used for getting the backmost nodes, advances to the next node and puts the current node into the input vector
	void Advance(Node* node, std::vector<Node*>& input, int* nodeCount);
};

class TreeManager {
public:
	void Search(BattleSystem* start);
private:
	void MakeDecision(Node*);
	BattleSystem* original;
};
