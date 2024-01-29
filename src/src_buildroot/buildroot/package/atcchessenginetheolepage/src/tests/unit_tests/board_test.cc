#include "gtest/gtest.h"

#include "chess_engine/board/entity/color.hh"
#include "chess_engine/board/entity/piece-type.hh"
#include "chess_engine/board/board.hh"
#include "chess_engine/board/chessboard.hh"

using namespace board;

TEST(board_test, init)
{
    Board board;
    auto pawn = board(PieceType::PAWN);
    auto queen = board(PieceType::QUEEN);
    auto black = board(Color::BLACK);
    auto white = board(Color::WHITE);

    EXPECT_EQ(0x0ULL, pawn);
    EXPECT_EQ(0x0ULL, black);
    EXPECT_EQ(0x0ULL, queen);
    EXPECT_EQ(0x0ULL, white);
}

TEST(board_test, simple_get)
{
    Board board = Board();
    board.set_piece(Position(0, 0), PieceType::PAWN, Color::BLACK);
    auto pawn = board(PieceType::PAWN);
    auto queen = board(PieceType::QUEEN);
    auto black = board(Color::BLACK);
    auto white = board(Color::WHITE);

    EXPECT_EQ(0x1ULL, pawn);
    EXPECT_EQ(0x1ULL, black);
    EXPECT_EQ(0x0ULL, queen);
    EXPECT_EQ(0x0ULL, white);
}

TEST(board_test, put_pawns)
{
    Board board;
    board.set_piece(Position(0, 0), PieceType::PAWN, Color::BLACK);
    board.set_piece(Position(1, 0), PieceType::PAWN, Color::BLACK);
    board.set_piece(Position(2, 0), PieceType::PAWN, Color::BLACK);
    board.set_piece(Position(3, 0), PieceType::PAWN, Color::BLACK);
    board.set_piece(Position(4, 0), PieceType::PAWN, Color::BLACK);
    board.set_piece(Position(5, 0), PieceType::PAWN, Color::BLACK);
    board.set_piece(Position(6, 0), PieceType::PAWN, Color::BLACK);
    board.set_piece(Position(7, 0), PieceType::PAWN, Color::BLACK);
    auto pawn = board(PieceType::PAWN);
    auto queen = board(PieceType::QUEEN);
    auto black = board(Color::BLACK);
    auto white = board(Color::WHITE);

    EXPECT_EQ(0xffULL, pawn);
    EXPECT_EQ(0xffULL, black);
    EXPECT_EQ(0x0ULL, queen);
    EXPECT_EQ(0x0ULL, white);

    for (int i = 0; i < 8; ++i)
    {
        auto pawn1 = board[Position(i, 0)];
        EXPECT_EQ(true, pawn1.has_value());
        EXPECT_EQ(PieceType::PAWN, pawn1->first);
        EXPECT_EQ(Color::BLACK, pawn1->second);
    }
}

TEST(board_test, get_all_pawns)
{
    Board board;
    board.init_default();
    int res = 0;

    auto pawn1 = board[Position(File::G, Rank::THREE)];
    EXPECT_EQ(false, pawn1.has_value());

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Position pos(file, rank);
            if (board[pos].has_value()
                && board[pos].value().first == PieceType::PAWN
                && board[pos].value().second == Color::WHITE)
            {
                res++;
            }
        }
    }
    
    EXPECT_EQ(8, res);
}

TEST(board_test, constructor)
{
    Chessboard cb;
    Board b = cb.get_board();

    for (int i = 0; i < 8; ++i)
    {
        auto pawn1 = b[Position(i, 1)];
        EXPECT_EQ(true, pawn1.has_value());
        EXPECT_EQ(PieceType::PAWN, pawn1->first);
        EXPECT_EQ(Color::WHITE, pawn1->second);
    }

    for (int i = 0; i < 8; ++i)
    {
        auto pawn1 = b[Position(i, 6)];
        EXPECT_EQ(true, pawn1.has_value());
        EXPECT_EQ(PieceType::PAWN, pawn1->first);
        EXPECT_EQ(Color::BLACK, pawn1->second);
    }

    //auto rook = b[Position(File::A, Rank::Hei)]
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}