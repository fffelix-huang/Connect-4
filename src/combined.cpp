#include <bits/stdc++.h>
#ifndef MOVE_HPP
#define MOVE_HPP

namespace felix {

using Move = int;

} // namespace felix

#endif // MOVE_HPP

#ifndef POSITION_HPP
#define POSITION_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <string>

namespace felix {

/*
* .  .  .  .  .  .  .
* 5 12 19 26 33 40 47
* 4 11 18 25 32 39 46
* 3 10 17 24 31 38 45
* 2  9 16 23 30 37 44
* 1  8 15 22 29 36 43
* 0  7 14 21 28 35 42 
*/

class Position {
public:
	static constexpr int WIDTH = 9;
	static constexpr int HEIGHT = 7;

	using bitboard_t = typename std::conditional<WIDTH * (HEIGHT + 1) <= 64, uint64_t, __int128>::type;

	static_assert(WIDTH < 10, "Board's width must be less than 10");
	static_assert(WIDTH * (HEIGHT + 1) <= sizeof(bitboard_t) * 8, "Board does not fit into bitboard_t bitmask");

	static constexpr int MIN_SCORE = -(WIDTH * HEIGHT) / 2 + 3;
	static constexpr int MAX_SCORE = (WIDTH * HEIGHT + 1) / 2 - 3;

	Position() : current_position(0), mask(0), moves(0), result(3) {}

	// Returns true if the column is playable.
	bool canPlay(Move col) const {
		if(isTerminal()) {
			return false;
		}
		return (mask & top_mask(col)) == 0;
	}

	// Place a piece at the given column.
	void play(Move col) {
		if(isWinningMove(col)) {
			result = moves % 2;
		}
		current_position ^= mask;
		if(col >= 0 && col < Position::WIDTH) {
			mask |= mask + bottom_mask(col);
			moves++;
		}
		if(moves == Position::HEIGHT * Position::WIDTH) {
			result = 2;
		}
	}

	// Plays a seqence of moves.
	int play(std::string seq) {
		for(int i = 0; i < (int) seq.size(); i++) {
			Move col = seq[i] - '1';
			if(col < 0 || col >= WIDTH || !canPlay(col) || isWinningMove(col)) {
				return i;
			}
			play(col);
		}
		return seq.size();
	}

	bool isWinningMove(Move col) const {
		if(col == -1) return false;
		bitboard_t pos = current_position;
		pos |= (mask + bottom_mask(col)) & column_mask(col);
		return Position::alignment(pos);
	}

	int nbMoves() const {
		return moves;
	}

	bool isTerminal() const { return result != 3; }
	bool isDraw() const { return result == 2; }
	int winner() const { return result; }

private:
	bitboard_t current_position;
	bitboard_t mask;
	int moves;
	int result; // 2: draw, 3: non-terminal

	// Given bitboard of one player's cells, returns true if the player has a 4-alignment;
	static bool alignment(bitboard_t pos) {
		// horizontal 
		bitboard_t m = pos & (pos >> (HEIGHT + 1));
		if(m & (m >> (2 * (HEIGHT + 1)))) {
			return true;
		}
		// diagonal 1
		m = pos & (pos >> HEIGHT);
		if(m & (m >> (2 * HEIGHT))) {
			return true;
		}
		// diagonal 2 
		m = pos & (pos >> (HEIGHT + 2));
		if(m & (m >> (2 * (HEIGHT + 2)))) {
			return true;
		}
		// vertical;
		m = pos & (pos >> 1);
		if(m & (m >> 2)) {
			return true;
		}
		return false;
	}

	// Returns a bitmask with single 1 bit on corresponding to the top cell of the input column.
	static bitboard_t top_mask(Move col) {
		return (bitboard_t(1) << (HEIGHT - 1)) << (col * (HEIGHT + 1));
	}

	// Returns a bitmask with single 1 bit on corresponding to the bottom cell of the input column.
	static bitboard_t bottom_mask(Move col) {
		return bitboard_t(1) << (col * (HEIGHT + 1));
	}

	// Returns a bitmask with all the cells on the input column on.
	static bitboard_t column_mask(Move col) {
		return ((bitboard_t(1) << HEIGHT) - 1) << (col * (HEIGHT + 1));
	}
};

} // namespace felix

#endif // POSITION_HPP

#ifndef MOVELIST_HPP
#define MOVELIST_HPP

#include <algorithm>

namespace felix {

class MoveList {
public:
	MoveList() {}
	MoveList(Position* p) : pos(p), move(), head(0), tail(0) {
		for(int i = 0; i < Position::WIDTH; i++) {
			if(pos->canPlay(i)) {
				move[tail++] = i;
			}
		}
	}

	int size() const { return tail - head; }
	bool empty() const { return head == tail; }
	int total() const { return tail; }

	void sort(); // Sort moves heuristically.

	Move nextMove() { return move[head++]; }
	Move getMove(int i) const { return move[i]; }

private:
	Position* pos;
	Move move[Position::WIDTH];
	int head, tail;
};

} // namespace felix

#endif // MOVELIST_HPP

namespace felix {

void MoveList::sort() {
	std::sort(move + head, move + tail, [&](Move a, Move b) {
		if(pos->isWinningMove(a)) {
			return true;
		}
		if(pos->isWinningMove(b)) {
			return false;
		}
		// Heuristic, center gets higher priority.
		return std::abs(a - Position::WIDTH / 2) < std::abs(b - Position::WIDTH / 2);
	});
}

} // namespace felix

#ifndef MCTS_HPP
#define MCTS_HPP

#include <iostream>
#include <vector>

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

	void simulate();

	Move getBestMove() const;

	int nbSimulations() const {
		return simulations;
	}

	void info(std::ostream&) const;

private:
	double calculateUCB(double) const;
	Node* select();
	bool isFullyExpanded() const;
	double rollout() const;
	void backpropagate(double w, int n);

	Node* parent;
	Move move_from_parent;
	Position* pos;
	MoveList list;
	double score;
	int simulations;
	Node* children[Position::WIDTH];
};

} // namespace MCTS

} // namespace felix

#endif // MCTS_HPP

#include <iostream>
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
		Move move = moves.getMove(rng(0, moves.total()));
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

void Node::info(std::ostream& out) const {
	out << "Number of simulations: " << simulations << std::endl;
	for(int i = 0; i < list.total(); i++) {
		out << "Move " << list.getMove(i) << ": " << children[i]->winRate() << " " << children[i]->nbSimulations() << std::endl;
	}
}

} // namespace MCTS

} // namespace felix

using namespace std;
using namespace felix;
using namespace MCTS;

struct timer {
	void start() {
		o = rdtsc();
	}
	inline double get_time() const {
		return (rdtsc() - o) * SECONDS_PER_CLOCK;
	}
	static constexpr double SECONDS_PER_CLOCK = 1.0 / 3E9;
	unsigned long long o;
	inline static unsigned long long rdtsc() {
		unsigned long long lo, hi;
		__asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
		return (hi << 32) | lo;
	}
};

int main(int argc, char* argv[]) {
	int my_id, opp_id;
	cin >> my_id >> opp_id;
	Position pos;
	while(true) {
		int foo; cin >> foo;
		for(int i = 0; i < Position::HEIGHT; i++) {
			string s;
			cin >> s;
		}
		cin >> foo;
		for(int i = 0; i < foo; i++) {
			int x;
			cin >> x;
		}
		int last_move;
		cin >> last_move;
		pos.play(last_move);
		Node* root = new Node(&pos);
		timer t;
		t.start();
		while(t.get_time() < 0.095) {
			root->simulate();
		}
		root->info(cerr);
		cout << root->getBestMove() << endl;
		pos.play(root->getBestMove());
	}
	return 0;
}
