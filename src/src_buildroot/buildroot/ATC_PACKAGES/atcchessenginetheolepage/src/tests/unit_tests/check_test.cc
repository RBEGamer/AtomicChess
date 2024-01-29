#include "gtest/gtest.h"

#include "chess_engine/board/chessboard.hh"
#include "parsing/perft_parser/perft-parser.hh"

using namespace board;
using namespace std;

TEST(Check, NotMat_ThenMat)
{
    Chessboard board = Chessboard(parse_perft("7r/3K3r/3P1p2/1n6/8/8/8/8 w - - 0 1 1"));
    auto moves = board.generate_legal_moves();
    EXPECT_EQ(2, moves.size());
    EXPECT_TRUE(board.is_check());
    EXPECT_FALSE(board.is_checkmate());

    board = Chessboard(parse_perft("7r/3K3r/3P1p2/1n6/8/7b/8/8 w - - 0 1 1"));
    moves = board.generate_legal_moves();
    EXPECT_EQ(1, moves.size());
    EXPECT_TRUE(board.is_check());
    EXPECT_FALSE(board.is_checkmate());

    board = Chessboard(parse_perft("7r/3K3r/3P1p2/1n6/8/7b/8/2q5 w - - 0 1 1"));
    moves = board.generate_legal_moves();
    EXPECT_EQ(0, moves.size());
    EXPECT_TRUE(board.is_check());
    EXPECT_TRUE(board.is_checkmate());
}

TEST(Draw, AllDaRooks)
{
    Chessboard board = Chessboard(parse_perft("1r1r4/8/8/8/7r/2K5/7r/8 w - - 0 1 1"));
    auto moves = board.generate_legal_moves();
    EXPECT_EQ(0, moves.size());
    EXPECT_FALSE(board.is_check());
    EXPECT_FALSE(board.is_checkmate());
    EXPECT_TRUE(board.is_draw());

    // Add a pawn to remove the draw

    board = Chessboard(parse_perft("1r1r4/8/8/8/7r/2K3P1/7r/8 w - - 0 1 1"));
    moves = board.generate_legal_moves();
    EXPECT_EQ(2, moves.size());
    EXPECT_FALSE(board.is_check());
    EXPECT_FALSE(board.is_checkmate());
    EXPECT_FALSE(board.is_draw());

    // A knight this time to protect him as he is checked

    board = Chessboard(parse_perft("1r1rN3/8/5b2/8/7r/2K5/7r/8 w - - 0 1 1"));
    moves = board.generate_legal_moves();
    EXPECT_EQ(1, moves.size());
    EXPECT_TRUE(board.is_check());
    EXPECT_FALSE(board.is_checkmate());
    EXPECT_FALSE(board.is_draw());
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}