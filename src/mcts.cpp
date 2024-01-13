#include "mcts.hpp"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <limits>
#include <cmath>
#include <random>
#include <chrono>

namespace felix {

namespace MCTS {

Node::Node() : parent(nullptr), move_from_parent(-1), pos(nullptr), list(), score(0), simulations(0), children() {}

Node::Node(Position* p) : parent(nullptr), move_from_parent(-1), pos(p), list(p), score(0), simulations(0), children() {}

Node::Node(Node* par, Move mfp, Position* p) : parent(par), move_from_parent(mfp), pos(p), list(p), score(0), simulations(0), children() {}

Node::~Node() {}

double Node::calculateUCB(double C = 2) const {
	static constexpr double INF = std::numeric_limits<double>::max() / 2;
	assert(parent != nullptr);
	if(simulations == 0) {
		return INF;
	}
	return (1 - winRate()) + C * std::sqrt(std::log(parent->nbSimulations()) / simulations);
}

Node* Node::select() {
	if(!isFullyExpanded()) {
		int id = list.total() - list.size();
		Move move = list.nextMove();
		Position* new_pos = new Position(*pos);
		new_pos->play(move);
		children[id] = new Node(this, move, new_pos);
		return children[id];
	}
	if(pos->isTerminal()) {
		return this;
	}
	double best_ucb = 0;
	Node* best_child = nullptr;
	for(int i = 0; i < list.total(); i++) {
		double cur_ucb = children[i]->calculateUCB(1.414);
		if(cur_ucb > best_ucb) {
			best_ucb = cur_ucb;
			best_child = children[i];
		}
	}
	return best_child->select();
}

bool Node::isFullyExpanded() const {
	return list.empty();
}

double Node::rollout() const {
	auto rng = [](int l, int r) {
		static std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
		return std::uniform_int_distribution<>(l, r)(gen);
	};
	Position game = Position(*pos);
	while(!game.isTerminal()) {
		MoveList moves(&game);
		Move move = moves.getMove(rng(0, moves.total() - 1));
		game.play(move);
	}
	if(game.isDraw()) {
		return 0.5;
	}
	if(pos->nbMoves() % 2 == game.nbMoves() % 2) {
		// Lose
		return 0;
	} else {
		// Win
		return 1;
	}
}

void Node::backpropagate(double w, int n) {
	score += w;
	simulations += n;
	if(parent != nullptr) {
		parent->backpropagate(n - w, n);
	}
}

void Node::simulate() {
	Node* leaf = select();
	double result = leaf->rollout();
	leaf->backpropagate(result, 1);
}

Node* Node::advanceTree(Move move) const {
	for(int i = 0; i < list.total(); i++) {
		if(list.getMove(i) == move) {
			if(i >= list.total() - list.size()) {
				Position* new_pos = new Position(*pos);
				new_pos->play(move);
				return new Node(new_pos);
			}
			children[i]->parent = nullptr;
			return children[i];
		}
	}
}

Move Node::getBestMove() const {
	double best_rate = 100;
	Move best_move = -1;
	for(int i = 0; i < list.total(); i++) {
		// Pick the move that minimize opponent's win rate.
		if(children[i]->winRate() < best_rate) {
			best_rate = children[i]->winRate();
			best_move = children[i]->move_from_parent;
		}
	}
	return best_move;
}

void Node::info(std::ostream& out) const {
	out << "Number of simulations: " << simulations << std::endl;
	double win_rate = 100;
	for(int i = 0; i < list.total(); i++) {
		out << "Move " << list.getMove(i) << ": " << children[i]->winRate() << " " << children[i]->nbSimulations() << std::endl;
		win_rate = std::min(win_rate, children[i]->winRate());
	}
	out << "Win Rate: " << std::fixed << std::setprecision(2) << (1 - win_rate) * 100 << "%" << std::endl;
}

} // namespace MCTS

} // namespace felix
