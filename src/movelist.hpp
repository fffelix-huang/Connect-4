#ifndef MOVELIST_HPP
#define MOVELIST_HPP

#include <algorithm>

#include "move.hpp"
#include "position.hpp"

namespace felix {

class MoveList {
public:
	MoveList(Position* p) : pos(p), move(), head(0), tail(0) {}

	int size() const { return tail - head; }
	bool empty() const { return head == tail; }

	void sort(); // Sort moves heuristically.

	void addMove(Move m) {
		move[tail++] = m;
	}

	Move nextMove() {
		return move[head++];
	}

private:
	Position* pos;
	Move move[Position::WIDTH];
	int head, tail;
};

} // namespace felix

#endif // MOVELIST_HPP
