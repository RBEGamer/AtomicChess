/*
 * util.h
 *
 *  Created on: 24 Sep 2015
 *      Author: miles
 */

#ifndef UTIL_UTIL_HPP_
#define UTIL_UTIL_HPP_

#include <string>
#include <array>

#include <iostream>

namespace bitchess {

/**
 * Represents a side in chess.
 */
enum Colour { WHITE, BLACK };
namespace PieceType {
/**
 * Represents a type of piece, or NO_PIECE.
 */
enum PieceType { ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN, NO_PIECE };



}

inline std::ostream& operator<<(std::ostream& out, const PieceType::PieceType piece) {
	static std::array<std::string, 7> pieces = {{"R","N","B","Q","K", "p", "Empty"}};
	return out << pieces[piece];
}

inline std::string index_to_square(int index) {
	/*  with little endian file-rank mapping, the rank is the dividend
	 *  and the file is the remainder. E.g. (35/8) ~= 4 and 35 % 8 = 4.
	 *  By adding 97 the remainder is converted to an ascii letter, with
	 *  0 = a, 1 = b, 2 = c etc. As for the dividend, as it is zero indexed,
	 *  just add one to get the dividend string. E.g. 4 -> 5 hence
	 *  index_to_square(35) = "e5".
	 */
	int div = (index/8);
	short rem = (index % 8)+97;
	return std::string(1,rem) + std::to_string(div + 1);
}

}


#endif /* UTIL_UTIL_HPP_ */
