#include "GameAITree.h"
#include <random>
#include <limits>

//----------------------------------------------------------------------------------------
//DEFINES FOR AI SEARCH SETTINGS

const int MAXDEPTH = 3;
const int DEVIATION = 0;
//----------------------------------------------------------------------------------------

Node::Node(BattleSystem const& initial) {
	previous = nullptr;
	battlesystem = initial;
	depth = 0;
	battlesystem.m_Entities.clear();
}

Node::Node(Node* node) {
	previous = node;
	battlesystem = node->battlesystem;
	depth = node->depth + 1;
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

std::vector<Node*> Node::GetBack() {
	std::vector<Node*> output;
	Advance(this, output);
	return output;
}

void Node::Advance(Node* node, std::vector<Node*>& input) {
	if (node->next.size() == 0) {
		input.push_back(node);
		return;
	}
	for (Node& n : node->next) {
		Advance(&n, input);
	}
	return;
}

//void TreeManager::Initialize(BattleSystem startState) {
//	std::vector<Node> initial;
//	initial.push_back(Node(startState));
//	parents = initial;
//}

void TreeManager::MakeDecision(Node* chosenNode) {
	original ->activeCharacter->action.selectedSkill = chosenNode->GetChosen()->battlesystem.activeCharacter->action.selectedSkill;
	for (CharacterStats& chosenChar : original->turnManage.characterList) {
		if (chosenChar.entity == chosenNode->GetChosen()->battlesystem.activeCharacter->action.targetSelect.selectedTarget->entity) {
			original->activeCharacter->action.targetSelect.selectedTarget = &chosenChar;
		}
	}
	original->activeCharacter->action.entityState = ATTACKING;
}

void TreeManager::Search(BattleSystem* start) {
	original = start;
	std::list<Node*> currentNodes;
	Node parent(*start);
	currentNodes.push_back(&parent);
	//for (Attack const& a : start->activeCharacter->action.skills) {
	//	for (CharacterStats& c : start->turnManage.characterList) {
	//		parent.next.push_back(Node{ parent });
	//		Node& n{ parent.next[parent.next.size() - 1] };
	//		n.battlesystem.activeCharacter->action.selectedSkill = a;
	//		n.battlesystem.activeCharacter->action.targetSelect.selectedTarget = &c;
	//		n.battlesystem.activeCharacter->action.entityState = ATTACKING;
	//		n.battlesystem.Update();
	//		while (n.battlesystem.battleState != PLAYERTURN && n.battlesystem.battleState != ENEMYTURN && n.battlesystem.battleState != WIN) {
	//			if (n.battlesystem.battleState == LOSE) {
	//				start->activeCharacter->action.selectedSkill = n.GetChosen()->battlesystem.activeCharacter->action.selectedSkill;
	//				for (CharacterStats& chosenChar : start->turnManage.characterList) {
	//					if (chosenChar.entity == n.GetChosen()->battlesystem.activeCharacter->action.targetSelect.selectedTarget->entity) {
	//						start->activeCharacter->action.targetSelect.selectedTarget = &chosenChar;
	//					}
	//				}
	//				start->activeCharacter->action.entityState = ATTACKING;
	//				return;
	//			}
	//			n.battlesystem.Update();
	//		}
	//		currentNodes.push_back(&n);
	//	}
	//}
	
	bool createFinish = false;
	while (createFinish == false) {
		std::vector<Node*> toRemove{};
		
		auto pointer = currentNodes.begin();
		size_t currentSize = currentNodes.size();
		for (size_t i = 0; i < currentSize; i++) {
			Node* n = *pointer;
			if (n->depth > MAXDEPTH) {
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
				for (CharacterStats* c : targetList) {
					n->next.push_back(Node{ n });
					Node* child{ &n->next[n->next.size() - 1] };
					child->battlesystem.activeCharacter->action.selectedSkill = a;
					CharacterStats* target = c;
					for (CharacterStats& t : child->battlesystem.turnManage.characterList) {
						if (t.entity == c->entity) {
							child->battlesystem.activeCharacter->action.targetSelect.selectedTarget = &t;
						}
					}
					child->battlesystem.activeCharacter->action.entityState = ATTACKING;
					child->battlesystem.Update();
					while (child->battlesystem.battleState != PLAYERTURN && child->battlesystem.battleState != ENEMYTURN && child->battlesystem.battleState != WIN) {
						if (child->battlesystem.battleState == LOSE) {
							MakeDecision(child);
							return;
						}
						child->battlesystem.Update();
					}
					if (child->battlesystem.battleState != WIN) {
						currentNodes.push_back(child);
					}
				}
			}
			toRemove.push_back(n);
			pointer++;
		}
		if (toRemove.size() == 0) {
			createFinish = true;
		}
		for (Node* n : toRemove) {
			currentNodes.remove(n);
		}
		toRemove.clear();
	}

	int currentEval{ INT_MIN };
	std::vector<Node*> selectedNodes;
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
	MakeDecision(chosenNode);
	return;
}
