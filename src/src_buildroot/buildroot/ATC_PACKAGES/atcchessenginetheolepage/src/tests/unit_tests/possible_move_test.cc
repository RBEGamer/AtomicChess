#include "gtest/gtest.h"

#include "chess_engine/board/chessboard.hh"
#include "parsing/perft_parser/perft-parser.hh"
#include "chess_engine/board/move-generation.hh"

using namespace board;
using namespace move_generation;
using namespace std;

/*
TEST(PossibleMove, Queen)
{
    // This is the subject example
    Chessboard board = Chessboard("8/8/8/3Q4/8/8/8/8");
    auto moves = generate_moves(board, PieceType::QUEEN);
    EXPECT_EQ(27, moves.size());

    board = Chessboard("7Q/8/8/8/8/8/8/8");
    moves = generate_moves(board, PieceType::QUEEN);
    EXPECT_EQ(21, moves.size());

    board = Chessboard("6P1/8/8/3Q4/8/8/8/8");
    moves = generate_moves(board, PieceType::QUEEN);
    EXPECT_EQ(26, moves.size());

    board = Chessboard("6P1/8/8/1P1Q4/4P3/8/8/8");
    moves = generate_moves(board, PieceType::QUEEN);
    EXPECT_EQ(20, moves.size());

    board = Chessboard("3p2P1/8/4p3/1P1Q4/8/8/8/4P3");
    moves = generate_moves(board, PieceType::QUEEN);
    EXPECT_EQ(23, moves.size());

    board = Chessboard("8/8/8/8/4R3/8/7p/6pQ");
    moves = generate_moves(board, PieceType::QUEEN);
    EXPECT_EQ(4, moves.size());

    board = Chessboard("3b3P/8/5Q2/P7/3p4/2Q2R2/1P6/8");
    moves = generate_moves(board, PieceType::QUEEN);
    EXPECT_EQ(15 + 18, moves.size());
}

TEST(PossibleMove, Pawn)
{
    Chessboard board = Chessboard("8/4P3/8/8/8/8/8/8");
    auto moves = generate_pawn_moves(board);
    EXPECT_EQ(4, moves.size());

    board = Chessboard("2ppp1p1/PPPPPP2/7P/8/8/8/8/8");
    moves = generate_pawn_moves(board);
    EXPECT_EQ(4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 1, moves.size());
}
*/

// TODO FIX
TEST(PossibleMove, Castling)
{
    std::vector<Move> res;
    Chessboard board = Chessboard(parse_perft("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1 1"));
    generate_king_moves(board, res);
    EXPECT_EQ(4, res.size());
}

/*
TEST(PossibleMove, EnPassant)
{
    Chessboard board = Chessboard(parse_perft("8/8/8/4Pp2/8/8/8/8 w - f6 0 1 1"));
    auto moves = generate_pawn_moves(board);
    EXPECT_EQ(2, moves.size());


    board = Chessboard(parse_perft("1p5p/P6P/8/3Pp3/8/8/8/8 w - e6 0 1 1"));
    moves = generate_pawn_moves(board);
    EXPECT_EQ(10, moves.size());
}
*/

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}