/*
 * pawn.hpp
 *
 *  Created on: 25 Sep 2015
 *      Author: miles
 */

#include "pieces.hpp"
#include "position/move.hpp"

#include <vector>

using bitchess::Bitboard;
using bitchess::Move;
using bitchess::pieces::Pawn;
using bitchess::Colour;

Bitboard Pawn::get_single_moves(Colour colour,Bitboard all_occupancy) {
	Bitboard occ_wo_pawns = all_occupancy & ~occupancy;

	Bitboard shifted_one;
	if(colour==WHITE) {
		shifted_one = occupancy.nortOne();
	} else {
		shifted_one = occupancy.soutOne();
	}

	return shifted_one & ~occ_wo_pawns;
}

Bitboard Pawn::get_double_moves(Colour colour,Bitboard all_occupancy) {
	Bitboard unoccupied = ~all_occupancy;

	Bitboard shifted;
	if(colour==WHITE) {
		// shift one
		shifted = (occupancy & 0xFF00).nortOne();
		shifted &= unoccupied; //remove any blocked pawns
		shifted = shifted.nortOne() & unoccupied; //shift again and remove blocked
	} else {
		shifted = (occupancy & 0xFF000000000000ULL).soutOne();
		shifted &= unoccupied;
		shifted = shifted.soutOne() & unoccupied;
	}

	return shifted;
}

Bitboard Pawn::get_captures(Colour colour,Bitboard opp_occupancy) {
	Bitboard shifted;

	if(colour==WHITE) {
		//shift diagonal
		shifted = occupancy.noEaOne() | occupancy.noWeOne();
	} else {
		shifted = occupancy.soEaOne() | occupancy.soWeOne();
	}
	return (shifted & opp_occupancy);
}

Bitboard Pawn::get_promotions(Colour colour,Bitboard single_moves) {
	Bitboard shifted;
	if(colour==WHITE) {
		//get with backrank mask
		shifted = single_moves & 0xff00000000000000;
	} else {
		shifted = single_moves & 0xff;
	}
	return shifted;
}


std::vector<Move> Pawn::get_pseudolegal_moves(
			Bitboard all_occupancy, Bitboard own_occupancy,
			Bitboard opp_occupancy ) {
	return std::vector<Move>();
}
