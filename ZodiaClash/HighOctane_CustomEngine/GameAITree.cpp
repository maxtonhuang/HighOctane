#include "GameAITree.h"

const int MAXDEPTH = 3;

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
	std::vector<std::vector<Node*>> backNodes;
	int currentEval{ 0 };
	Node* selectedNode{ nullptr };
	for (Node& n: parents) {
		backNodes.push_back(n.GetBack());
	}
	for (std::vector<Node*> v : backNodes) {
		for (Node* n : v) {

			//If enemy can win, it always goes for it
			if (n->currentState.gameStatus == GAMESTATUS::LOSE) {
				selectedAttack = n->GetFront()->selectedAttack;
				selectedCharacter = n->GetFront()->selectedTarget;
				return;
			}
			int eval = GameAILogic::Evaluate(n->GetFront()->previousState, n->currentState);
			if (currentEval == 0) {
				currentEval = eval;
				selectedNode = n;
			}
			else {
				if (currentEval < eval) {
					currentEval = eval;
					selectedNode = n;
				}
			}
			
		}
	}
	selectedAttack = selectedNode->GetFront()->selectedAttack;
	selectedCharacter = selectedNode->GetFront()->selectedTarget;
	return;
}
