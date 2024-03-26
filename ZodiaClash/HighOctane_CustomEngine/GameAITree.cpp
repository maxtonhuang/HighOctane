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
*	@file		GameAITree.cpp
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


#include "GameAITree.h"
#include <random>
#include <limits>

//----------------------------------------------------------------------------------------
//DEFINES FOR AI SEARCH SETTINGS

//WARNING: INCREASING THIS VALUE RESULTS IN EXPONENTIALLY HIGHER SEARCH TIMES
const int MAXDEPTH = 0;

const int DEVIATION = 1000;
//----------------------------------------------------------------------------------------

Node::Node(BattleSystem const& initial) {
	previous = nullptr;
	battlesystem = initial;
	depth = 0;
	battlesystem.m_Entities.clear();
	next.reserve(100);
}

Node::Node(Node* node) {
	previous = node;
	battlesystem = node->battlesystem;
	depth = node->depth + 1;
	next.reserve(100);
}

Node* Node::GetFront() {
	Node* current = this;
	while (current->previous != nullptr) {
		current = current->previous;
	}
	return current;
}

Node* Node::GetChosen() {
	Node* current = this;
	while (current->previous->previous != nullptr) {
		current = current->previous;
	}
	return current;
}

std::vector<Node*> Node::GetBack(int* nodeCount) {
	std::vector<Node*> output;
	Advance(this, output, nodeCount);
	return output;
}

void Node::Advance(Node* node, std::vector<Node*>& input, int* nodeCount) {
	if (nodeCount != nullptr) {
		(*nodeCount)++;
	}
	if (node->next.size() == 0) {
		input.push_back(node);
		return;
	}
	for (Node& n : node->next) {
		Advance(&n, input, nodeCount);
	}
	return;
}

void TreeManager::MakeDecision(Node* chosenNode) {
	original ->activeCharacter->action.selectedSkill = chosenNode->GetChosen()->nodeCharacter->action.selectedSkill;
	Node* chosen = chosenNode->GetChosen();
	for (CharacterStats& chosenChar : original->turnManage.characterList) {
		if (chosenChar.entity == chosen->selectedTarget) {
			original->activeCharacter->action.targetSelect.selectedTarget = &chosenChar;
		}
	}
	original->activeCharacter->action.entityState = ATTACKING;
}

void TreeManager::Search(BattleSystem* start) {
	int currentEval{ INT_MIN };
	std::vector<Node*> selectedNodes;
	original = start;
	std::list<Node*> currentNodes;
	Node parent(*start);
	currentNodes.push_back(&parent);
	Node* backup{}; //IN CASE ENEMY CANT FIND ANY NODES

	bool createFinish = false;
	while (createFinish == false) {
		std::vector<Node*> toRemove{};
		
		auto pointer = currentNodes.begin();
		size_t currentSize = currentNodes.size();
		for (auto i = 0; i < currentSize; i++) {
			Node* n = *pointer;
			if (n->depth > MAXDEPTH) {
				pointer++;
				continue;
			}

			//FOR ALL POSSIBLE MOVES, CREATE A NEW CHILD AND ADD TO CURRENTNODES
			for (Attack const& a : n->battlesystem.activeCharacter->action.skills) {
				
				std::vector<CharacterStats*> targetList{};

				for (auto& c : n->battlesystem.turnManage.characterList) {
					targetList.push_back(&c);
				}

				//FOR ALL POSSIBLE TARGETS, CREATE A NEW CHILD AND ADD TO CURRENTNODES
				for (CharacterStats* c : targetList) {
					//Do not enable self-targeting
					if (a.attacktype != AttackType::ALLYSELF && n->battlesystem.activeCharacter->entity == c->entity) {
						continue;
					}

					n->next.push_back(Node{ n });
					Node* child{ &n->next[n->next.size() - 1] };

					//ASSIGN SKILL
					child->battlesystem.activeCharacter->action.selectedSkill = a;

					//ASSIGN TO CHILDS VERSION OF THE CHARACTER
					for (CharacterStats& t : child->battlesystem.turnManage.characterList) {
						if (t.entity == c->entity) {
							child->battlesystem.activeCharacter->action.targetSelect.selectedTarget = &t;
						}
					}

					//IF TARGET DOES NOT EXIST
					if (child->battlesystem.activeCharacter->action.targetSelect.selectedTarget == nullptr) {
						continue;
					}

					//SET ACTIVE CHARACTER STATE
					child->battlesystem.activeCharacter->action.entityState = ATTACKING;

					//ASSIGN CURRENT CHARACTER TO NODE
					child->nodeCharacter = child->battlesystem.activeCharacter;
					child->selectedTarget = child->battlesystem.activeCharacter->action.targetSelect.selectedTarget->entity;
					
					//UPDATE TO NEXT TURN
					//THIS UPDATES ACTIVE CHARACTER TO NEXT TURNS ACTIVE CHARACTER
					while (child->battlesystem.activeCharacter->action.entityState != WAITING) {
						if (child->battlesystem.battleState == WIN) {
							break;
						}
						if (child->battlesystem.battleState == LOSE) {
							MakeDecision(child);
							return;
						}
						child->battlesystem.Update();
					}
					//IF ENEMY LOSES, DONT PUSH NODE
					if (child->battlesystem.battleState == WIN) {
						child->eval = GameAILogic::Evaluate(*start, child->battlesystem);
						if (backup == nullptr) {
							backup = child;
						}
						else if (child->eval > backup->eval) {
							backup = child;
						}
						break;
					}

					//EVALUATE NODES
					child->eval = GameAILogic::Evaluate(*start, child->battlesystem);
					currentNodes.push_back(child);
				}
			}
			toRemove.push_back(n);
			pointer++;
		}

		//REMOVE EVALUATED NODES
		if (toRemove.size() == 0) {
			createFinish = true;
		}
		for (Node* n : toRemove) {
			currentNodes.remove(n);
		}
		toRemove.clear();
	}

	//IF ENEMY CANT FIND ANY VALID MOVES
	if (currentNodes.size() == 0) {
		MakeDecision(backup);
		return;
	}

	//EVALUATE NODES
	for (Node* n : currentNodes) {
		n->eval = GameAILogic::Evaluate(*start, n->battlesystem);
		if (n->eval > currentEval) {
			std::vector<Node*> newSelectedNodes;
			currentEval = n->eval;
			newSelectedNodes.push_back(n);
			for (Node* sn : selectedNodes) {
				if (sn->eval > currentEval - DEVIATION) {
					newSelectedNodes.push_back(sn);
				}
			}
			selectedNodes = newSelectedNodes;
		}
		else if (n->eval > currentEval - DEVIATION) {
			selectedNodes.push_back(n);
		}
	}

	//Randomly choose an attack among the selected attacks
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> rand_node(0, selectedNodes.size() - 1);
	size_t chosenNum{ rand_node(gen) };
	Node* chosenNode = selectedNodes[chosenNum];
	std::vector<Node*> branch{};
	Node* reverse{ chosenNode };
	while (reverse->previous != nullptr) {
		branch.push_back(reverse);
		reverse = reverse->previous;
	}
	MakeDecision(chosenNode);

	int nodeCount = 0;
	parent.GetBack(&nodeCount);

	return;
}
