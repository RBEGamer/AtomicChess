/*
 * test_movelookup_king.cpp
 *
 *  Created on: 25 Sep 2015
 *      Author: miles
 */

#include "util/movelookup.hpp"
#include "util/util.hpp"
#include "gtest/gtest.h"

using bitchess::move_lookup;
using bitchess::Bitboard;

TEST(MoveLookup_King, GetsCentreMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 28); //corresponds to e4
	Bitboard expected = Bitboard(0x3828380000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsNorthMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 60); //e8
	Bitboard expected = Bitboard(0x2838000000000000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsSouthMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 4); //e1
	Bitboard expected = Bitboard(0x3828);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsEastMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 31); //h4
	Bitboard expected = Bitboard(0x0c040c00000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsWestMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 24); //a4
	Bitboard expected = Bitboard(0x302030000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsNoWestMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 63); //h8
	Bitboard expected = Bitboard(0x40c0000000000000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsNoEastMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 56); //a8
	Bitboard expected = Bitboard(0x203000000000000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsSoEastMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 0); //a1
	Bitboard expected = Bitboard(0x302);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_King, GetsSoWestMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KING, 7); //h1
	Bitboard expected = Bitboard(0xc040);
	ASSERT_EQ(expected,result);
}


