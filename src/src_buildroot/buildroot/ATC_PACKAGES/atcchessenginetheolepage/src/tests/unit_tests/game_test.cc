#include "gtest/gtest.h"

#include "chess_engine/board/chessboard.hh"
#include "parsing/perft_parser/perft-parser.hh"

using namespace board;
using namespace std;

TEST(DrawPGN, CastlingIllegal)
{
    Chessboard board = Chessboard(parse_perft("rn1qk2r/pb1pbppp/1p2pn2/8/2PP1B2/P1N2N2/1P2PPPP/R2QKB1R b KQkq - 0 1 1"));
    Move castling(Position(File::E, Rank::EIGHT),
                    Position(File::G, Rank::EIGHT),
                    PieceType::KING,
                    false,
                    false,
                    false,
                    true,
                    false);

    EXPECT_TRUE(board.is_move_possible(castling));
    EXPECT_TRUE(board.is_possible_move_legal(castling));
    EXPECT_TRUE(board.is_move_possible(castling));
    EXPECT_TRUE(board.is_possible_move_legal(castling));
}