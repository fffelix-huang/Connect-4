#ifndef MCTS_HPP
#define MCTS_HPP

#include <vector>

#include "move.hpp"
#include "movelist.hpp"
#include "position.hpp"

namespace felix {

namespace MCTS {

class Node {
public:
	Node();
	Node(Position*);
	Node(Node*, Move, Position*);
	~Node();

	double winRate() const {
		return score / simulations;
	}

	Move getBestMove() const;

	int nbSimulations() const {
		return simulations;
	}

private:
	bool isLeaf() const { return true; } // TODO
	double calculateUCB(double) const;
	Node* select() const;
	void isFullyExpand

	Node* parent;
	Move move_from_parent;
	Position* pos;
	MoveList* list;
	double score;
	int simulations;
	Node* children[Position::WIDTH];
};

} // namespace MCTS

} // namespace felix

#endif // MCTS_HPP
