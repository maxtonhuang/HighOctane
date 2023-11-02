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
const int MAXDEPTH = 4;

const int DEVIATION = 0;
//----------------------------------------------------------------------------------------

Node::Node(BattleSystem const& initial) {
	previous = nullptr;
	battlesystem = initial;
	depth = 0;
	battlesystem.m_Entities.clear();
	next.reserve(12);
}

Node::Node(Node* node) {
	previous = node;
	battlesystem = node->battlesystem;
	depth = node->depth + 1;
	next.reserve(12);
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

//void TreeManager::Initialize(BattleSystem startState) {
//	std::vector<Node> initial;
//	initial.push_back(Node(startState));
//	parents = initial;
//}

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
				
				std::vector<CharacterStats*> targetList;
				if (n->battlesystem.activeCharacter->tag == CharacterType::PLAYER) {
					targetList = n->battlesystem.GetEnemies();
				}
				else {
					targetList = n->battlesystem.GetPlayers();
				}

				//FOR ALL POSSIBLE TARGETS, CREATE A NEW CHILD AND ADD TO CURRENTNODES
				for (CharacterStats* c : targetList) {
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
							printf("\nAI found possible win state:\n");
							printf("Chosen skill: %s\n", start->activeCharacter->action.selectedSkill.attackName.c_str());
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
					/*
					if (child->eval > currentEval) {
						std::vector<Node*> newSelectedNodes;
						currentEval = child->eval;
						newSelectedNodes.push_back(child);
						for (Node* sn : selectedNodes) {
							if (sn->eval > currentEval - DEVIATION) {
								newSelectedNodes.push_back(sn);
							}
						}
						selectedNodes = newSelectedNodes;
					}
					else if (child->eval > currentEval - DEVIATION) {
						selectedNodes.push_back(n);
					}*/
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
		printf("\nAI cannot find any possible win conditions\n");
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
	std::default_random_engine rng;
	std::uniform_int_distribution<size_t> rand_node(0, selectedNodes.size() - 1);
	Node* chosenNode = selectedNodes[rand_node(rng)];
	std::vector<Node*> branch{};
	Node* reverse{ chosenNode };
	while (reverse->previous != nullptr) {
		branch.push_back(reverse);
		reverse = reverse->previous;
	}
	MakeDecision(chosenNode);

	int nodeCount = 0;
	parent.GetBack(&nodeCount);

	for (auto& c : start->turnManage.characterList) {
		std::string name = ECS::ecs().GetComponent<Name>(c.entity).name;
		printf("%s remaining health: %f\n", name.c_str(), c.stats.health);
		DEBUG_PRINT("%s remaining health: %f", name.c_str(), c.stats.health);
	}

	printf("\nAI decision made:\n");
	printf("Total nodes created: %d\n", nodeCount);
	printf("Printing branch:\n");
	for (int i = (int)branch.size() - 1; i >= 0; i--) {
		std::string userName = ECS::ecs().GetComponent<Name>(branch[i]->nodeCharacter->entity).name;
		std::string targetName = ECS::ecs().GetComponent<Name>(branch[i]->selectedTarget).name;
		std::string moveUsed = branch[i]->nodeCharacter->action.selectedSkill.attackName;
		printf("%s used %s on %s: Evaluation amount %d\n", userName.c_str(), moveUsed.c_str(), targetName.c_str(), branch[i]->eval);
	}
	printf("Evaluation value: %i\n", (int)currentEval);
	printf("CHOSEN SKILL: %s\n", start->activeCharacter->action.selectedSkill.attackName.c_str());

	DEBUG_PRINT("AI decision made:");
	DEBUG_PRINT("Total nodes created: %d", nodeCount);
	DEBUG_PRINT("Printing branch:");
	for (int i = (int)branch.size() - 1; i >= 0; i--) {
		std::string userName = ECS::ecs().GetComponent<Name>(branch[i]->nodeCharacter->entity).name;
		std::string targetName = ECS::ecs().GetComponent<Name>(branch[i]->selectedTarget).name;
		std::string moveUsed = branch[i]->nodeCharacter->action.selectedSkill.attackName;
		DEBUG_PRINT("%s used %s on %s: Evaluation amount %d", userName.c_str(), moveUsed.c_str(), targetName.c_str(), branch[i]->eval);
	}
	DEBUG_PRINT("Evaluation value: %i", (int)currentEval);
	DEBUG_PRINT("CHOSEN SKILL: %s", start->activeCharacter->action.selectedSkill.attackName.c_str());

	return;
}
