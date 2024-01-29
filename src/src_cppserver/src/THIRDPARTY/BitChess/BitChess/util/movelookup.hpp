/*
 * movelookup.hpp
 *
 *  Created on: 25 Sep 2015
 *      Author: miles
 */

#ifndef UTIL_MOVELOOKUP_HPP_
#define UTIL_MOVELOOKUP_HPP_

#include "bitboard.hpp"
#include "util.hpp"

#include <array>
#include <map>
#include <cassert>

namespace {

/**
 * Generates knight moves for a specified square.
 * @param sq Square index [0-64] to get a move for.
 * @return Bitboard with 1 indicating a valid move.
 */
static bitchess::Bitboard __gen__knight_move__( short sq ) {
	using bitchess::Bitboard;

	// get an occupancy bitboard
	Bitboard occ = Bitboard::with_bit_set_at(sq);

	// get bitboards shifted by one space NESW
	Bitboard n = occ.nortOne(), e = occ.eastOne(), s = occ.soutOne(), w =
			occ.westOne();

	Bitboard b;

	b = n.noWeOne() | n.noEaOne() | e.noEaOne() | e.soEaOne() | s.soEaOne()
		| s.soWeOne() | w.soWeOne() | w.noWeOne();

	return b;
}

/**
 * Generates king moves for a specified square.
 * @param sq Square index [0-64] to get a move for.
 * @return Bitboard with 1 indicating a valid move.
 */
static bitchess::Bitboard __gen_king_move__( short sq ) {

	// get a bitboard with the occupancy of the king
	bitchess::Bitboard occ = bitchess::Bitboard::with_bit_set_at(sq);

	// shift the occupancy bitboard in each direction to represent
	// a king move

	return occ.nortOne() | occ.noEaOne() | occ.eastOne() | occ.soEaOne()
			| occ.soutOne() | occ.soWeOne() | occ.westOne() | occ.noWeOne();;
}

/**
 * Generates a Bitboard of white pawn captures from a specific square.
 * @param sq Square index [0-64] to get a move for.
 * @return Bitboard with 1 indicating a valid move.
 */
static bitchess::Bitboard __gen_white_pawn_caps__( short sq ) {
	// get the occupancy bitboard
	bitchess::Bitboard occ = bitchess::Bitboard::with_bit_set_at(sq);

	//shift noEast and noWest to get caps
	return occ.noWeOne() | occ.noEaOne();
}

/**
 * Generates a Bitboard of black pawn captures from a specific square.
 * @param sq Square index [0-64] to get a move for.
 * @return Bitboard with 1 indicating a valid move.
 */
static bitchess::Bitboard __gen_black_pawn_caps__( short sq ) {
	// get the occupancy bitboard
	bitchess::Bitboard occ = bitchess::Bitboard::with_bit_set_at(sq);

	//shift soEast and soWest to get caps
	return occ.soWeOne() | occ.soEaOne();
}

/**
 * Gets an array of king moves, indexed by square.
 * @return
 */
static std::array<bitchess::Bitboard, 64> __init_king_move_arr__() {
	std::array<bitchess::Bitboard, 64> arr;
	// generate a move bitboard for each square on the board
	for ( int sq = 0; sq < 64; ++sq ) {
		arr[sq] = __gen_king_move__(sq);
	}
	return arr;
}

/**
 * Gets an array of knight moves, indexed by square.
 * @return
 */
static std::array<bitchess::Bitboard, 64> __init_knight_move_arr__() {
	std::array<bitchess::Bitboard, 64> arr;
	// generate a move bitboard for each square on the board
	for ( int sq = 0; sq < 64; ++sq ) {
		arr[sq] = __gen__knight_move__(sq);
	}
	return arr;
}

static std::array<bitchess::Bitboard, 64> __init_pawn_caps_arr__(bitchess::Colour colour) {
	std::array<bitchess::Bitboard, 64> arr;
	for( int sq= 0; sq < 64; ++sq) {
		arr[sq] = (colour==bitchess::Colour::WHITE) ? __gen_white_pawn_caps__(sq) : __gen_black_pawn_caps__(sq);
	}
	return arr;
}

static std::map<bitchess::Colour, std::array<bitchess::Bitboard, 64>> __lookup_pawn_cap__() {
	static std::map<bitchess::Colour, std::array<bitchess::Bitboard, 64>> map =
	{{bitchess::Colour::WHITE, __init_pawn_caps_arr__(bitchess::Colour::WHITE)},
	{bitchess::Colour::BLACK, __init_pawn_caps_arr__(bitchess::Colour::BLACK)}};
	return map;
}

/**
 * Used to shorten the declaration. May be used map[piece][sq].
 */
typedef std::map<bitchess::PieceType::PieceType,
		std::array<bitchess::Bitboard, 64>> PieceBitBoardMoveMap;

/**
 * @return the static piece map. Is used map[piece][sq]
 */
static PieceBitBoardMoveMap __lookup_piece_map__() {
	static PieceBitBoardMoveMap map =
			{ { bitchess::PieceType::PieceType::KING, __init_king_move_arr__() },
				{ bitchess::PieceType::PieceType::KNIGHT,
					__init_knight_move_arr__() } };
	return map;
}

}

namespace bitchess {

/**
 * Gets a bitboard of possible moves depending on the piece and the square.
 * @param piece Piece to get moves for. If a piece is invalid (e.g. NO_PIECE) or
 * unimplemented, an assertion will fail. Currently only KING moves are supported.
 * @param sq Board square index in range [0,64]/
 * @return bitboard of possible moves.
 */
inline bitchess::Bitboard move_lookup( bitchess::PieceType::PieceType piece,
										short sq ) {
	// check that piece is in lookup map.
	assert(__lookup_piece_map__().count(piece) > 0);
	return __lookup_piece_map__()[piece][sq];
}

inline bitchess::Bitboard lookup_pawn_capture( bitchess::Colour colour, short sq ) {
	return __lookup_pawn_cap__()[colour][sq];
}

}

#endif /* UTIL_MOVELOOKUP_HPP_ */
