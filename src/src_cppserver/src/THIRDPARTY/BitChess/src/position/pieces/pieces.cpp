/*
 * king.cpp
 *
 *  Created on: 29 Sep 2015
 *      Author: miles
 */

#include "pieces.hpp"
#include "position/move.hpp"

#include <vector>

using bitchess::pieces::King;

/**
 * Gets all pseudolegal moves available to the Piece. Some moves may result in the
 * king moving into or being in check, therefore not being legal; these must be checked.
 * @return a vector of pseudolegal moves.
 */
std::vector<bitchess::Move> bitchess::pieces::SingleMovePiece::get_pseudolegal_moves(
		Bitboard all_occupancy, Bitboard own_occupancy,
		Bitboard opp_occupancy ) {

	assert(occupancy.bits.count() == 1); // kings should always be on the board!

	//get king location
	short loc = occupancy.ls1b();
	assert(loc >= 0); //king_loc would be -1 if no bits are set, however this is impossible

	// calc bitboards of moves and captures
	Bitboard moves_bb = get_moves_bitboard(loc) & ~all_occupancy;
	Bitboard caps_bb = get_moves_bitboard(loc) & opp_occupancy;

	// count number of moves and captures
	short num_moves = moves_bb.bits.count();
	short num_caps = caps_bb.bits.count();

	// prealloc vector with resulting pseudolegal moves
	std::vector<bitchess::Move> moves(num_moves + num_caps);

	// iterate over bits of moves and create moves, allocating to moves[i]
	for ( int i = 0; i < num_moves; ++i ) {
		__scan_moves__(loc, moves_bb, moves[i], false);
	}
	// same for caps
	for ( int i = 0; i < num_caps; ++i ) {
		__scan_moves__(loc, caps_bb, moves[num_moves + i], true);
	}

	return moves;
}
