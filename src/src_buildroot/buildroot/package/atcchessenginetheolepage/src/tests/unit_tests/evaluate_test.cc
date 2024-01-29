#include "gtest/gtest.h"

#include "chess_engine/ai/evaluation.hh"

using namespace board;
using namespace ai;

TEST(Evaluate, OnlyKings)
{
    Chessboard board = Chessboard("7k/8/8/8/8/8/8/K7");

    EXPECT_EQ(evaluate(board), 0);
}

TEST(Evaluate, WhiteAdvantage)
{
    Chessboard board = Chessboard("nP6/PP6/8/8/2k5/8/8/7K");

    EXPECT_GT(evaluate(board), 0);
}

TEST(EvaluateMaterial, SimplePawn)
{
    Chessboard white_board = Chessboard("8/1P6/8/8/8/8/8/8");
    Chessboard black_board = Chessboard("8/8/8/8/8/8/1p6/8");

    const int abs_board_value = 100;

    EXPECT_EQ(evaluate_material(white_board), abs_board_value);
    EXPECT_EQ(evaluate_material(black_board), -abs_board_value);
}

TEST(EvaluateMaterial, SimpleQueen)
{
    Chessboard white_board = Chessboard("8/8/8/8/Q7/8/8/8");
    Chessboard black_board = Chessboard("8/8/8/q7/8/8/8/8");

    const int abs_board_value = 900;

    EXPECT_EQ(evaluate_material(white_board), abs_board_value);
    EXPECT_EQ(evaluate_material(black_board), -abs_board_value);
}

TEST(EvaluateMaterial, SimpleRook)
{
    Chessboard white_board = Chessboard("8/7R/8/8/8/8/8/8");
    Chessboard black_board = Chessboard("8/8/8/8/8/8/7r/8");

    const int abs_board_value = 500;

    EXPECT_EQ(evaluate_material(white_board), abs_board_value);
    EXPECT_EQ(evaluate_material(black_board), -abs_board_value);
}

TEST(EvaluateMaterial, InitialBoard)
{
    Chessboard board;

    EXPECT_EQ(evaluate_material(board), 0);
}

TEST(EvaluateMaterial, MultiplePieces)
{
    Chessboard board = Chessboard("2k5/7R/1q1p1p2/8/4N3/8/2B5/8");

    EXPECT_EQ(evaluate_material(board), 500 + 330 + 320 - 20000 - 100 * 2 - 900);
}

TEST(EvaluateSquares, SimplePawn)
{
    Chessboard white_board = Chessboard("8/1PK1k3/8/8/8/8/8/8");
    Chessboard black_board = Chessboard("8/8/8/8/8/8/1pk1K3/8");

    const int abs_board_value = 50 - 10 + 0;

    EXPECT_EQ(evaluate_squares(white_board), abs_board_value);
    EXPECT_EQ(evaluate_squares(black_board), -abs_board_value);
}

TEST(EvaluateSquares, SimpleQueen)
{
    Chessboard white_board = Chessboard("8/8/4K3/8/Q7/4k3/8/8");
    Chessboard black_board = Chessboard("8/8/4k3/q7/8/4K3/8/8");

    const int abs_board_value = 0;

    EXPECT_EQ(evaluate_squares(white_board), abs_board_value);
    EXPECT_EQ(evaluate_squares(black_board), -abs_board_value);
}

TEST(EvaluateSquares, SimpleRook)
{
    Chessboard white_board = Chessboard("8/7R/3k4/8/8/3K4/8/8");
    Chessboard black_board = Chessboard("8/8/3K4/8/8/3k4/7r/8");

    const int abs_board_value = 5;

    EXPECT_EQ(evaluate_squares(white_board), abs_board_value);
    EXPECT_EQ(evaluate_squares(black_board), -abs_board_value);
}

TEST(IsEndGameBoard, MiddleGame)
{
    Chessboard board = Chessboard("2k5/7R/1q1p1p2/8/4N3/8/2B3Q1/3K4");

    EXPECT_FALSE(is_end_game(board));
}

TEST(IsEndGameBoard, EndGameNoQueens)
{
    Chessboard board = Chessboard("2r4k/4n3/1pp5/8/5PPP/4P3/8/1R1K2B1");

    EXPECT_TRUE(is_end_game(board));
}

TEST(IsEndGameBoard, EndGameLonelyQueen)
{
    Chessboard board = Chessboard("1b1k3r/3p4/1n2p3/p1p5/8/8/2B2B2/K2Q4");

    EXPECT_TRUE(is_end_game(board));
}

TEST(IsEndGameBoard, MiddleGameLonelyQueen)
{
    Chessboard board = Chessboard("1b1k3r/3p4/1n2p3/p1p5/8/1P6/2B2B2/K2Q3R");

    EXPECT_FALSE(is_end_game(board));
}

TEST(EvaluateSquares, KingMiddleGame)
{
    Chessboard board = Chessboard("5qnb/3K4/8/8/k7/8/8/5QNB");

    EXPECT_EQ(evaluate_squares(board), -50 - (-30));
}

TEST(EvaluateSquares, KingEndGame)
{
    Chessboard board = Chessboard("8/3K4/8/8/k7/8/8/8");

    EXPECT_EQ(evaluate_squares(board), 0 - (-30));
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
