#include "movelist.hpp"

#include <cmath>

#include "position.hpp"

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
