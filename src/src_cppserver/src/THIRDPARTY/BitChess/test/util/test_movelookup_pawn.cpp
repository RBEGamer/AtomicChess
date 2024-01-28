

#include "util/movelookup.hpp"
#include "util/util.hpp"
#include "gtest/gtest.h"

using bitchess::Bitboard;
using bitchess::lookup_pawn_capture;
using bitchess::Colour;

TEST(MoveGen_Pawn, WhiteCaptureLookup_d4) {
	Bitboard expected = Bitboard(0x1400000000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	1	.	1	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard result = lookup_pawn_capture(Colour::WHITE,27); //d4

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, WhiteCaptureLookup_h1) {
	Bitboard expected = Bitboard(0x4000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	1	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard result = lookup_pawn_capture(Colour::WHITE,7); //h1

	ASSERT_EQ(expected,result);
}


TEST(MoveGen_Pawn, WhiteCaptureLookup_c8) {
	Bitboard expected = Bitboard(0x0);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard result = lookup_pawn_capture(Colour::WHITE,58); //c8

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, BlackCaptureLookup_d4) {
	Bitboard expected = Bitboard(0x140000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	1	.	1	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard result = lookup_pawn_capture(Colour::BLACK,27); //d4

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, BlackCaptureLookup_h8) {
	Bitboard expected = Bitboard(0x40000000000000);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	1	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard result = lookup_pawn_capture(Colour::BLACK,63); //d4

	ASSERT_EQ(expected,result);
}

TEST(MoveGen_Pawn, BlackCaptureLookup_g1) {
	Bitboard expected = Bitboard(0x0);
	/*
		h	g	f	e	d	c	b	a
		.	.	.	.	.	.	.	.	1
		.	.	.	.	.	.	.	.	2
		.	.	.	.	.	.	.	.	3
		.	.	.	.	.	.	.	.	4
		.	.	.	.	.	.	.	.	5
		.	.	.	.	.	.	.	.	6
		.	.	.	.	.	.	.	.	7
		.	.	.	.	.	.	.	.	8
	 */

	Bitboard result = lookup_pawn_capture(Colour::BLACK,6); //g1

	ASSERT_EQ(expected,result);
}
