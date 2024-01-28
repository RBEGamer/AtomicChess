/*
 * test_movelookup_knight.cpp
 *
 *  Created on: 25 Sep 2015
 *      Author: miles
 */


#include "util/movelookup.hpp"
#include "util/util.hpp"
#include "gtest/gtest.h"

using bitchess::move_lookup;
using bitchess::Bitboard;


TEST(MoveLookup_Knight, GetsCentreMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 28); //corresponds to e4
	Bitboard expected = Bitboard(0x284400442800);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsNorthMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 60); //e8
	Bitboard expected = Bitboard(0x44280000000000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsSouthMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 4); //e1
	Bitboard expected = Bitboard(0x284400);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsEastMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 31); //h4
	Bitboard expected = Bitboard(0x402000204000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsWestMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 24); //a4
	Bitboard expected = Bitboard(0x20400040200);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsNoWestMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 63); //h8
	Bitboard expected = Bitboard(0x20400000000000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsNoEastMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 56); //a8
	Bitboard expected = Bitboard(0x04020000000000);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsSoEastMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 0); //a1
	Bitboard expected = Bitboard(0x20400);
	ASSERT_EQ(expected,result);
}

TEST(MoveLookup_Knight, GetsSoWestMove) {
	Bitboard result = move_lookup(bitchess::PieceType::KNIGHT, 7); //h1
	Bitboard expected = Bitboard(0x402000);
	ASSERT_EQ(expected,result);
}
