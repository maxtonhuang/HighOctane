#include "GameAITree.h"
#include <random>

//----------------------------------------------------------------------------------------
//DEFINES FOR AI SEARCH SETTINGS

const int MAXDEPTH = 3;
const int DEVIATION = 0;
//----------------------------------------------------------------------------------------

Node::Node(Gamestate initial) {
	previous = nullptr;
	previousState = initial;
	depth = 0;
}

Node::Node(Node* node) {
	previous = node;
	previousState = node->currentState;
	depth = node->depth + 1;
}

Node::~Node() {
	//Orphaning all pruned branches for now
	for (Node* n : next) {
		n->previous = nullptr;
	}
}

Node* Node::GetFront() {
	Node* current = this;
	while (current->previous != nullptr) {
		current = current->previous;
	}
	return current;
}

std::vector<Node*> Node::GetBack() {
	std::vector<Node*> output;
	Advance(this, output);
	return output;
}

void Node::Advance(Node* node, std::vector<Node*> input) {
	if (node->next.size() == 0) {
		input.push_back(node);
		return;
	}
	for (Node* n : node->next) {
		Advance(n, input);
	}
	return;
}

void TreeManager::Initialize(Gamestate startState) {
	std::vector<Node> initial;
	initial.push_back(Node(startState));
	parents = initial;
}

void TreeManager::Search(Gamestate const& start, Attack& selectedAttack, Character& selectedCharacter) {
	(void)start;
	std::list<Node*> currentNodes;
	Node parent(start);
	currentNodes.push_back(&parent);
	bool createFinish = false;
	while (createFinish == false) {
		for (Node* n : currentNodes) {
			if (n->depth - n->GetFront()->depth > MAXDEPTH) {
				continue;
			}

			//FOR ALL POSSIBLE MOVES, CREATE A NEW CHILD AND ADD TO CURRENTNODES
			if (n->currentState.gameStatus == GAMESTATUS::INPROGRESS) {
				
			}
			else if (n->currentState.gameStatus == GAMESTATUS::LOSE) {
				//If enemy can win, it always goes for it
				selectedAttack = n->GetFront()->selectedAttack;
				selectedCharacter = n->GetFront()->selectedTarget;
				return;
			}

			currentNodes.remove(n);
		}
	}

	int currentEval{ 0 };
	std::vector<Node*> selectedNodes;
	for (Node* n : currentNodes) {
		n->eval = GameAILogic::Evaluate(n->GetFront()->previousState, n->currentState);
		if (currentEval == 0) {
			currentEval = n->eval;
			selectedNodes.push_back(n);
		}
		else {
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
	}

	//Randomly choose an attack among the selected attacks
	std::default_random_engine rng;
	std::uniform_int_distribution<size_t> rand_node(0, selectedNodes.size() - 1);
	selectedAttack = selectedNodes[rand_node(rng)]->GetFront()->selectedAttack;
	selectedCharacter = selectedNodes[rand_node(rng)]->GetFront()->selectedTarget;
	return;
}
