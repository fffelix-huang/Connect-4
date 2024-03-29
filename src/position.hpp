#ifndef POSITION_HPP
#define POSITION_HPP

#include <iostream>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <string>

#include "move.hpp"

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

	Position() : current_position(0), mask(0), moves(0), pass_moves(0), result(3) {}

	// Returns true if the column is playable.
	bool canPlay(Move col) const {
		if(isTerminal()) {
			return false;
		}
		return (mask & top_mask(col)) == 0;
	}

	void pass() {
		current_position ^= mask;
		moves++;
		pass_moves++;
	}

	// Place a piece at the given column.
	void play(Move col) {
		if(isWinningMove(col)) {
			result = nbMoves() % 2;
		}
		current_position ^= mask;
		mask |= mask + bottom_mask(col);
		moves++;
		if(moves - pass_moves == Position::HEIGHT * Position::WIDTH) {
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

	void display(std::ostream& out) const {
		for(int i = 0; i < Position::WIDTH; i++) {
			out << i << " ";
		}
		out << std::endl;
		for(int i = 0, start_id = Position::HEIGHT - 1; i < Position::HEIGHT; i++, start_id--) {
			for(int j = 0, id = start_id; j < Position::WIDTH; j++, id += Position::HEIGHT + 1) {
				if(mask >> id & 1) {
					out << "XO"[(current_position >> id & 1) ^ (nbMoves() % 2)];
				} else {
					out << ".";
				}
				out << " ";
			}
			out << std::endl;
		}
	}

private:
	bitboard_t current_position;
	bitboard_t mask;
	int moves;
	int pass_moves;
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
