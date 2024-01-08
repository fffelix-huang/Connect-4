#include "mcts.hpp"

#include <cassert>
#include <limits>
#include <cmath>

namespace felix {

namespace MCTS {

Node::Node() : parent(nullptr), move_from_parent(-1), pos(nullptr), list(nullptr), score(0), simulations(0), children() {}

Node::Node(Position* p) : parent(nullptr), move_from_parent(-1), pos(p), list(nullptr), score(0), simulations(0), children() {}

Node::Node(Node* par, Move mfp, Position* p) : parent(par), move_from_parent(mfp), pos(p), list(nullptr), score(0), simulations(0), children() {}

Node::~Node() {}

double Node::calculateUCB(double C = 2) const {
	static constexpr double INF = std::numeric_limits<double>::max() / 2;
	assert(parent != nullptr);
	if(simulations == 0) {
		return INF;
	}
	return winRate() + C * std::sqrt(std::log(parent->nbSimulations()) / simulations);
}

Move Node::getBestMove() const {
	double best_rate = 100;
	Move best_move = -1;
	for(int i = 0; i < Position::WIDTH; i++) {
		// Pick the move that minimize opponent's win rate.
		if(children[i]->winRate() < best_rate) {
			best_rate = children[i]->winRate();
			best_move = children[i]->move_from_parent;
		}
	}
	return best_move;
}

} // namespace MCTS

} // namespace felix
