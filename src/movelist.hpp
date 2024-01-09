#ifndef MOVELIST_HPP
#define MOVELIST_HPP

#include <algorithm>

#include "move.hpp"
#include "position.hpp"

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
