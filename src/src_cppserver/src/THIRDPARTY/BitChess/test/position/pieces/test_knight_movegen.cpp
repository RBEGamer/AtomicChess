/*
 * test_king_movegen.cpp
 *
 *  Created on: 27 Sep 2015
 *      Author: miles
 */


#include "util/util.hpp"
#include "util/movelookup.hpp"
#include "position/pieces/pieces.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>

#include <iostream>

using bitchess::pieces::Knight;

using bitchess::PieceType::PieceType;
using bitchess::Bitboard;
using bitchess::Colour;
using bitchess::Move;
using std::vector;

using namespace::testing;

namespace {

	const Bitboard WHITE_OCCUPANCY = Bitboard(0x0802280500240001);
	/**
	h	g	f	e	d	c	b	a
	.	.	.	.	.	.	.	1	1
	.	.	.	.	.	.	.	.	2
	.	.	1	.	.	1	.	.	3
	.	.	.	.	.	.	.	.	4
	.	.	.	.	.	1	.	1	5
	.	.	1	.	1	.	.	.	6
	.	.	.	.	.	.	1	.	7
	.	.	.	.	1	.	.	.	8
	 */

	const Bitboard BLACK_OCCUPANCY = Bitboard(0x5088008808000800);
	/**
	h	g	f	e	d	c	b	a
	.	.	.	.	.	.	.	.	1
	.	.	.	.	1	.	.	.	2
	.	.	.	.	.	.	.	.	3
	.	.	.	.	1	.	.	.	4
	1	.	.	.	1	.	.	.	5
	.	.	.	.	.	.	.	.	6
	1	.	.	.	1	.	.	.	7
	.	1	.	1	.	.	.	.	8
	 */

	const Bitboard ALL_OCCUPANCY = WHITE_OCCUPANCY | BLACK_OCCUPANCY;

}

TEST(MoveGen_Knight, knight_with_no_moves) {
	Knight knight(Bitboard::with_bit_set_at(49)); //b7, is blocked

	vector<Move> moves = knight.get_pseudolegal_moves(ALL_OCCUPANCY,WHITE_OCCUPANCY,BLACK_OCCUPANCY);

	ASSERT_THAT(moves.size(), Eq(0));
}

TEST(MoveGen_Knight, knight_with_two_moves) {
	Knight knight(Bitboard::with_bit_set_at(0)); //a1, has two moves to c2 (10) and b3(17)


	vector<Move> expected = {
							 Move(0,10,false,PieceType::KNIGHT),
							 Move(0,17,false,PieceType::KNIGHT)
	};

	vector<Move> result = knight.get_pseudolegal_moves(ALL_OCCUPANCY,WHITE_OCCUPANCY,BLACK_OCCUPANCY);

	ASSERT_THAT(result, UnorderedElementsAreArray(expected));
}

//TEST(MoveGen_Knight, knight_with_free_move) {
//	Knight knight(Bitboard::with_bit_set_at(49)); //b7, has a move in each direction = 8 moves
//
//	vector<Move> expected = {
//							 Move(49,50,false,PieceType::KING), //b7-c7
//							 Move(49,48,false,PieceType::KING), //b7-a7
//							 Move(49,42,false,PieceType::KING), //b7-c6
//							 Move(49,41,false,PieceType::KING), //b7-b6
//							 Move(49,40,false,PieceType::KING), //b7-a6
//							 Move(49,58,false,PieceType::KING), //b7-c8
//							 Move(49,57,false,PieceType::KING), //b7-b8
//							 Move(49,56,false,PieceType::KING)  //b7-a8
//	};
//
//	vector<Move> result = knight.get_pseudolegal_moves(ALL_OCCUPANCY,WHITE_OCCUPANCY,BLACK_OCCUPANCY);
//	ASSERT_THAT(result, UnorderedElementsAreArray(expected));
//
//}
//
//TEST(MoveGen_Knight, knight_at_edge) {
//	Knight knight(Bitboard::with_bit_set_at(24)); //a4 (at edge, with 5 moves available)
//
//	vector<Move> expected = {
//							 Move(24,16,false,PieceType::KING), //a4-a3
//							 Move(24,17,false,PieceType::KING), //a4-b3
//							 Move(24,25,false,PieceType::KING), //a4-b4
//							 Move(24,32,false,PieceType::KING), //a4-a5
//							 Move(24,33,false,PieceType::KING), //a4-b5
//	};
//
//	vector<Move> result = knight.get_pseudolegal_moves(ALL_OCCUPANCY,WHITE_OCCUPANCY,BLACK_OCCUPANCY);
//	ASSERT_THAT(result, UnorderedElementsAreArray(expected));
//}
//
//TEST(MoveGen_Knight, knight_with_one_capture) {
//	Knight knight(Bitboard::with_bit_set_at(29)); //f4, 8 moves available with cap on g5
//
//		vector<Move> expected = {
//								 Move(29,38,true,PieceType::KING),  //f4xg5
//								 Move(29,30,false,PieceType::KING), //f4-g4
//								 Move(29,22,false,PieceType::KING), //f4-g3
//								 Move(29,21,false,PieceType::KING), //f4-f3
//								 Move(29,20,false,PieceType::KING), //f4-e3
//								 Move(29,28,false,PieceType::KING), //f4-e4
//								 Move(29,36,false,PieceType::KING), //f4-e5
//								 Move(29,37,false,PieceType::KING), //f4-f5
//		};
//
//		vector<Move> result = knight.get_pseudolegal_moves(ALL_OCCUPANCY,WHITE_OCCUPANCY,BLACK_OCCUPANCY);
//		ASSERT_THAT(result, UnorderedElementsAreArray(expected));
//}
//
//TEST(MoveGen_Knight, knight_with_multiple_captures) {
//	Knight knight(Bitboard::with_bit_set_at(11)); //d2, 8 moves available with 7 caps
//
//		vector<Move> expected = {
//								 Move(11,2,true,PieceType::KING),  //d2xc1
//								 Move(11,3,true,PieceType::KING), //d2xd1
//								 Move(11,4,true,PieceType::KING), //d2xe1
//								 Move(11,10,true,PieceType::KING), //d2xc2
//								 Move(11,12,true,PieceType::KING), //d2xe2
//								 Move(11,18,true,PieceType::KING), //d2xc3
//								 Move(11,19,true,PieceType::KING), //d2xd3
//								 Move(11,20,false,PieceType::KING), //d2-e3
//		};
//
//		vector<Move> result = knight.get_pseudolegal_moves(ALL_OCCUPANCY,WHITE_OCCUPANCY,BLACK_OCCUPANCY);
//		ASSERT_THAT(result, UnorderedElementsAreArray(expected));
//
//}
