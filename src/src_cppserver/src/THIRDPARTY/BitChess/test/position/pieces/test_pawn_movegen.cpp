/*
 * test_pawn_movegen.cpp
 *
 *  Created on: 25 Sep 2015
 *      Author: miles
 */

#include "util/util.hpp"
#include "position/pieces/pieces.hpp"

#include "gtest/gtest.h"

using bitchess::pieces::Pawn;
using bitchess::Bitboard;
using bitchess::Colour;

/*
 * NB the chess board used here is arranged as such:
 * h	g	f	e	d	c	b	a
 * .	.	.	.	.	.	.	.	1
 * .	.	.	.	.	.	.	.	2
 * .	.	.	.	.	.	.	.	3
 * .	.	.	.	.	.	.	.	4
 * .	.	.	.	.	.	.	.	5
 * .	.	.	.	.	.	.	.	6
 * .	.	.	.	.	.	.	.	7
 * .	.	.	.	.	.	.	.	8
 *
 */



TEST(MoveGen_Pawn, WhiteGetsSingleMoves) {
	Bitboard whitepawn_occ = Bitboard(0xFF00);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		1	1	1	1	1	1	1	1	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard overall_occ = Bitboard(0x4CFF00);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		1	1	1	1	1	1	1	1	2
		.	1	.	.	1	1	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Pawn pawn(whitepawn_occ);

	Bitboard result = pawn.get_single_moves(Colour::WHITE,overall_occ);
	Bitboard expected = 0xB30000;
	/*
	 	h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		1	.	1	1	.	.	1	1	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */


	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, BlackGetsSingleMoves) {
	Bitboard blackpawn_occ = Bitboard(0xFF000000000000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		1	1	1	1	1	1	1	1	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard overall_occ = Bitboard(0xFFF00000000000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		1	1	1	1	.	.	.	.	6
		1	1	1	1	1	1	1	1	7
		.	.	.	.	.	.	.	.	8
	 */
	Pawn pawn(blackpawn_occ);

	Bitboard result = pawn.get_single_moves(Colour::BLACK,overall_occ);
	Bitboard expected = 0x0F0000000000;
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	1	1	1	1	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	ASSERT_EQ(expected,result);
}


TEST(MoveGen_Pawn, WhiteGetsDoubleMoves) {
	Bitboard whitepawn_occ = Bitboard(0x4100CC00);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		1	1	.	.	1	1	.	.	2
		.	.	.	.	.	.	.	.	3
		.	1	.	.	.	.	.	1	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */
	Bitboard overall_occ = Bitboard(0x4104CC00);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		1	1	.	.	1	1	.	.	2
		.	.	.	.	.	1	.	.	3
		.	1	.	.	.	.	.	1	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Pawn pawn (whitepawn_occ);

	Bitboard result = pawn.get_double_moves(Colour::WHITE,overall_occ);
	Bitboard expected = 0x88000000;
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		1	.	.	.	1	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, BlackGetsDoubleMoves) {
	Bitboard blackpawn_occ = Bitboard(0xef400000000000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	1	.	.	.	.	.	.	6
		1	1	1	.	1	1	1	1	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard overall_occ = Bitboard(0xef480200000000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	1	.	5
		.	1	.	.	1	.	.	.	6
		1	1	1	.	1	1	1	1	7
		.	.	.	.	.	.	.	.	8
	 */

	Pawn pawn(blackpawn_occ);

	Bitboard result = pawn.get_double_moves(Colour::BLACK,overall_occ);
	Bitboard expected = 0xa500000000;
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		1	.	1	.	.	1	.	1	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, GetsWhiteCaptures) {
	Bitboard whitepawn_occ(0x4810a700);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		1	.	1	.	.	1	1	1	2
		.	.	.	1	.	.	.	.	3
		.	1	.	.	1	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */
	Bitboard black_occ(0xe810460000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	1	.	.	.	1	1	.	3
		.	.	.	1	.	.	.	.	4
		1	1	1	.	1	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Pawn pawn (whitepawn_occ);

	Bitboard result = pawn.get_captures(Colour::WHITE,black_occ);
	Bitboard expected = Bitboard(0xa000460000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	1	.	.	.	1	1	.	3
		.	.	.	.	.	.	.	.	4
		1	.	1	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, GetsBlackCaptures) {
	Bitboard blackpawn_occ(0x083480200200000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	1	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	1	.	5
		.	1	.	.	1	.	.	.	6
		1	.	.	.	.	.	1	1	7
		.	.	.	.	.	.	.	.	8
	 */
	Bitboard white_occ(0x94a002401002);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	1	.	1
		.	.	.	1	.	.	.	.	2
		.	1	.	.	.	.	.	.	3
		.	.	.	.	.	.	1	.	4
		1	.	1	.	.	.	.	.	5
		1	.	.	1	.	1	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Pawn pawn (blackpawn_occ);

	Bitboard result = pawn.get_captures(Colour::BLACK,white_occ);
	Bitboard expected = Bitboard(0x4a000001000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	1	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		1	.	1	.	.	.	.	.	5
		.	.	.	.	.	1	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, GetsWhitePromotions) {
	Bitboard whitepawn_occ(0x2a400000001000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	1	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	1	.	.	.	.	.	.	6
		.	.	1	.	1	.	1	.	7
		.	.	.	.	.	.	.	.	8
	 */
	Bitboard white_singlemoves(0x2a40000000100000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	1	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	1	.	.	.	.	.	.	7
		.	.	1	.	1	.	1	.	8
	 */

	Pawn pawn (whitepawn_occ);

	Bitboard result = pawn.get_promotions(Colour::WHITE,white_singlemoves);
	Bitboard expected = Bitboard(0x2a00000000000000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	1	.	1	.	1	.	8
	 */

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, GetsBlackPromotions) {
	Bitboard blackpawn_occ(0x01000004208800);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		1	.	.	.	1	.	.	.	2
		.	.	1	.	.	.	.	.	3
		.	.	.	.	.	1	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	1	7
		.	.	.	.	.	.	.	.	8
	 */
	Bitboard black_singlemoves(0x010000042088);
	/*
		h	g	f	e	d	c	b	a
		1	.	.	.	1	.	.	.	1
		.	.	1	.	.	.	.	.	2
		.	.	.	.	.	1	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	1	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Pawn pawn (blackpawn_occ);

	Bitboard result = pawn.get_promotions(Colour::BLACK,black_singlemoves);
	Bitboard expected = Bitboard(0x88);
	/*
		h	g	f	e	d	c	b	a
		1	.	.	.	1	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	ASSERT_EQ(expected,result);
}
