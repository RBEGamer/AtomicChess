/*
 * test_index_to_algebraic.cpp
 *
 *  Created on: 28 Sep 2015
 *      Author: miles
 */


#include "util/util.hpp"

#include <string>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
using std::string;

using bitchess::index_to_square;

TEST(Util,ConvertBoardIndexToSquare) {

	EXPECT_THAT(index_to_square(0), StrEq("a1"));
	EXPECT_THAT(index_to_square(9), StrEq("b2"));
	EXPECT_THAT(index_to_square(18), StrEq("c3"));
	EXPECT_THAT(index_to_square(27), StrEq("d4"));
	EXPECT_THAT(index_to_square(36), StrEq("e5"));
	EXPECT_THAT(index_to_square(45), StrEq("f6"));
	EXPECT_THAT(index_to_square(54), StrEq("g7"));
	EXPECT_THAT(index_to_square(63), StrEq("h8"));

	// the below added to test a bug where the board was flipped
	// as testing the diagonal is not enough

	EXPECT_THAT(index_to_square(56), StrEq("a8"));
	EXPECT_THAT(index_to_square(49), StrEq("b7"));
	EXPECT_THAT(index_to_square(42), StrEq("c6"));
	EXPECT_THAT(index_to_square(35), StrEq("d5"));
	EXPECT_THAT(index_to_square(28), StrEq("e4"));
	EXPECT_THAT(index_to_square(21), StrEq("f3"));
	EXPECT_THAT(index_to_square(14), StrEq("g2"));
	EXPECT_THAT(index_to_square(7), StrEq("h1"));

}
