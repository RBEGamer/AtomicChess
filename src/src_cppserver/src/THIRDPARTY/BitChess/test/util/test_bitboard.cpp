/*
 * test_Bitboard.cpp
 *
 *  Created on: 25 Sep 2015
 *      Author: miles
 */

#include "gtest/gtest.h"
#include "util/bitboard.hpp"


using bitchess::Bitboard;

TEST( Bitboard, ShiftsNorth) {
	// 0 0 0 0			0 0 0 0
	// 0 0 0 0			0 0 0 0
	// 0 0 0 0 ...to...	f f f f
	// f f f f			0 0 0 0
	Bitboard b = Bitboard(0x0000000000000000ffULL);
	Bitboard expected = Bitboard(0x000000000000ff00ULL);
	Bitboard result = b.nortOne();
	ASSERT_EQ(expected,result);

	b = Bitboard(0xff00000000000000ULL);
	expected = Bitboard(0x0000000000000000ULL);
	result = b.nortOne();
	ASSERT_EQ(expected,result);
}

TEST( Bitboard, ShiftsSouth) {
	// 0 0 0 0			0 0 0 0
	// 0 0 0 0			0 0 0 0
	// f f f f ...to...	0 0 0 0
	// 0 0 0 0			f f f f
	Bitboard b = 0x0000000000000000ffULL;
	Bitboard expected = 0x0000000000000000ULL;
	Bitboard result = b.soutOne();
	ASSERT_EQ(Bitboard(expected),Bitboard(result));

	b = Bitboard(0xffeeffeeffeeffeeULL);
	expected = Bitboard(0x00ffeeffeeffeeffULL);
	result = b.soutOne();
	ASSERT_EQ(Bitboard(expected),Bitboard(result));
}

TEST( Bitboard, ShiftsWest) {
	// 0 f 0 0			0 f 0 0
	// 0 f 0 0			0 f 0 0
	// 0 f 0 0 ...to...	0 f 0 0
	// 0 f 0 0			0 f 0 0
	Bitboard b = 0x4040404040404040ULL;
	Bitboard expected = 0x2020202020202020ULL;
	Bitboard result = b.westOne();
	ASSERT_EQ(Bitboard(expected),result);

	b = Bitboard(0x0101010101010101ULL);
	expected = Bitboard(0x0ULL);
	result = b.westOne();
	ASSERT_EQ(Bitboard(expected),result);
}


TEST( Bitboard, ShiftsEast) {
	// 0 f 0 0			f 0 0 0
	// 0 f 0 0			f 0 0 0
	// 0 f 0 0 ...to...	f 0 0 0
	// 0 f 0 0			f 0 0 0
	Bitboard b = 0x4040404040404040ULL;
	Bitboard expected = 0x8080808080808080ULL;
	Bitboard result = b.eastOne();
	ASSERT_EQ(Bitboard(expected),result);

	b = Bitboard(0x8080808080808080ULL);
	expected = Bitboard(0x0ULL);
	result = b.eastOne();
	ASSERT_EQ(Bitboard(expected),result);
}

TEST( Bitboard, ShiftsSouthEast) {
	// 0 0 0 0			0 0 0 0
	// 0 f f 0			0 0 0 0
	// 0 f f 0 ...to...	f f 0 0
	// 0 0 0 0			f f 0 0
	Bitboard b = 0x0000001818000000ULL;
	Bitboard expected = 0x0000000030300000ULL;
	Bitboard result = b.soEaOne();
	ASSERT_EQ(Bitboard(expected),result);

	b = Bitboard(0x000000000000c0c0ULL);
	expected = Bitboard(0x80ULL);
	result = b.soEaOne();
	ASSERT_EQ(Bitboard(expected),result);

}

TEST( Bitboard, ShiftsNorthEast) {
	// 0 0 0 0			f f 0 0
	// 0 f f 0			f f 0 0
	// 0 f f 0 ...to...	0 0 0 0
	// 0 0 0 0			0 0 0 0
	Bitboard b = 0x0000001818000000ULL;
	Bitboard expected = 0x0000303000000000ULL;
	Bitboard result = b.noEaOne();
	ASSERT_EQ(Bitboard(expected),result);

	b = Bitboard(0xc0c0000000000000ULL);
	expected = Bitboard(0x8000000000000000ULL);
	result = b.noEaOne();
	ASSERT_EQ(Bitboard(expected),result);

}

TEST( Bitboard, ShiftsNorthWest) {
	// 0 0 0 0			0 0 f f
	// 0 f f 0			0 0 f f
	// 0 f f 0 ...to...	0 0 0 0
	// 0 0 0 0			0 0 0 0
	Bitboard b = 0x0000001818000000ULL;
	Bitboard expected = 0x00000c0c00000000ULL;
	Bitboard result = b.noWeOne();
	ASSERT_EQ(Bitboard(expected),result);

	b = Bitboard(0x0303000000000000ULL);
	expected = Bitboard(0x0100000000000000ULL);
	result = b.noWeOne();
	ASSERT_EQ(Bitboard(expected),result);

}


TEST( Bitboard, ShiftsSouthWest) {
	// 0 0 0 0			0 0 0 0
	// 0 f f 0			0 0 0 0
	// 0 f f 0 ...to...	0 0 f f
	// 0 0 0 0			0 0 f f
	Bitboard b = 0x0000001818000000ULL;
	Bitboard expected = 0x000000000c0c0000ULL;
	Bitboard result = b.soWeOne();
	ASSERT_EQ(Bitboard(expected),result);

	b = Bitboard(0x0000000000000303ULL);
	expected = Bitboard(0x0000000000000001ULL);
	result = b.soWeOne();
	ASSERT_EQ(Bitboard(expected),result);

}

TEST( Bitboard, Bitscan_LSB) {
	short expected = 5;

	Bitboard b = Bitboard::with_bit_set_at(expected);
	short result = b.ls1b();

	ASSERT_EQ(expected, result);
}

TEST(Bitboard, Bitscan_and_remove) {
	Bitboard expected = Bitboard(0xFFFFFFFE);
	Bitboard result = Bitboard(0xFFFFFFFF).bitscan_and_remove().second;

	ASSERT_EQ(expected,result);
}
