#include "gtest/gtest.h"

#include "chess_engine/board/entity/color.hh"
#include "chess_engine/board/entity/piece-type.hh"
#include "chess_engine/board/entity/position.hh"
#include "chess_engine/board/move-generation.hh"

using namespace board;

TEST(move_generation_test, simple_bishop)
{
    Chessboard b("8/8/8/3B4/8/8/8/8");
    auto res = std::vector<Move>();
    move_generation::generate_bishop_moves(b, res);
    EXPECT_EQ(13, res.size());
    res.clear();

    Chessboard b1("B7/8/8/8/8/8/8/8");
    move_generation::generate_bishop_moves(b1, res);
    EXPECT_EQ(7, res.size());
    res.clear();

    Chessboard b2("8/8/8/8/8/8/4B3/8");
    move_generation::generate_bishop_moves(b2, res);
    EXPECT_EQ(9, res.size());
}

TEST(move_generation_test, simple_bishop_with_blockers)
{
    Chessboard b("8/8/8/8/2p3P1/8/4B3/8");
    auto res = std::vector<Move>();
    move_generation::generate_bishop_moves(b, res);
    EXPECT_EQ(5, res.size());
    res.clear();

    Chessboard b1("8/8/p7/4p2P/8/3p4/3pB2p/3P1p2");
    move_generation::generate_bishop_moves(b1, res);
    EXPECT_EQ(4, res.size());
}

TEST(move_generation_test, simple_rook)
{
    Chessboard b("8/8/8/3R4/8/8/8/8");
    auto res = std::vector<Move>();
    move_generation::generate_rook_moves(b, res);
    EXPECT_EQ(14, res.size());
    res.clear();

    Chessboard b1("8/8/8/8/8/8/8/R7");
    move_generation::generate_rook_moves(b1, res);
    EXPECT_EQ(14, res.size());
    res.clear();

    Chessboard b2("8/8/8/8/8/1R6/8/8");
    move_generation::generate_rook_moves(b2, res);
    EXPECT_EQ(14, res.size());
}

TEST(move_generation_test, simple_rook_with_blockers)
{
    Chessboard b("8/8/1p6/8/8/8/pR4P1/1P6");
    auto res = std::vector<Move>();
    move_generation::generate_rook_moves(b, res);
    EXPECT_EQ(9, res.size());
    res.clear();

    Chessboard b1("4pkP1/1n2R1q1/3pP3/6Q1/8/8/6P1/8");
    move_generation::generate_rook_moves(b1, res);
    EXPECT_EQ(6, res.size());
    res.clear();

    Chessboard b2("8/8/1n1n4/1qqqqR2/1n1r1Q2/1n1PBN2/1nB2B2/8", Color::BLACK);
    move_generation::generate_rook_moves(b2, res);
    EXPECT_EQ(4, res.size());
    res.clear();

    Chessboard b56("8/4p3/8/8/8/8/8/1p2R1p1");
    move_generation::generate_rook_moves(b56, res);
    EXPECT_EQ(11, res.size());
    res.clear();

    Chessboard b8("pp3p2/1pppp2p/1p1p1R1p/8/1p4p1/3p4/5p2/p7");
    move_generation::generate_rook_moves(b8, res);
    EXPECT_EQ(10, res.size());
    res.clear();

    Chessboard b3("R1p3p1/8/8/8/8/8/8/r3R3", Color::BLACK);
    move_generation::generate_rook_moves(b3, res);
    EXPECT_EQ(11, res.size());
    res.clear();

    Chessboard b4("r1p3p1/8/8/8/8/8/8/r3r3", Color::BLACK);
    move_generation::generate_rook_moves(b4, res);
    EXPECT_EQ(29, res.size());
    res.clear();

    Chessboard b5("1rp3p1/8/8/8/8/8/1r2r3/8", Color::BLACK);
    move_generation::generate_rook_moves(b5, res);
    EXPECT_EQ(27, res.size());
    res.clear();

    Chessboard b6("2p3p1/1r6/8/8/8/8/1r2r3/8", Color::BLACK);
    move_generation::generate_rook_moves(b6, res);
    EXPECT_EQ(32, res.size());
    res.clear();

    Chessboard b7("8/1r6/8/8/8/8/1r2r3/8", Color::BLACK);
    move_generation::generate_rook_moves(b7, res);
    EXPECT_EQ(32, res.size());
}

TEST(move_generation_test, simple_queen)
{
    Chessboard b("8/8/8/3Q4/8/8/8/8");
    auto res = std::vector<Move>();
    move_generation::generate_queen_moves(b, res);
    EXPECT_EQ(27, res.size());
    res.clear();

    Chessboard b1("8/8/8/8/8/8/8/q7", Color::BLACK);
    move_generation::generate_queen_moves(b1, res);
    EXPECT_EQ(21, res.size());
    res.clear();

    Chessboard b2("R1p3p1/8/8/8/8/8/8/q3R3", Color::BLACK);
    move_generation::generate_queen_moves(b2, res);
    EXPECT_EQ(18, res.size());
    res.clear();

    Chessboard b3("R1p1q1p1/8/8/8/8/8/8/q3R3", Color::BLACK);
    move_generation::generate_queen_moves(b3, res);
    EXPECT_EQ(34, res.size());
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}