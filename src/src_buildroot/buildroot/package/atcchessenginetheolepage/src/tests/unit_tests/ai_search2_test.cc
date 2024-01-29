#include "gtest/gtest.h"

#include "chess_engine/ai/ai-mini.hh"
#include "chess_engine/board/chessboard.hh"

using namespace board;

TEST(Check, minimax_search_simple_checkmat4)
{
    ai::AiMini our_ai = ai::AiMini();
    //Chessboard chessboard = Chessboard(parse_perft("r2k1b1Q/pppn3p/3p4/1B5n/5pb1/5N2/PPPP1qPP/RNBKR3 w - - 0 1 0"));
    Chessboard chessboard = Chessboard(parse_perft("r2k1b1Q/p1pn3p/8/1B6/8/8/8/K3R3 w - - 0 1 0"));
    std::optional<Move> bestmove = our_ai.search(chessboard, 4);
    EXPECT_EQ(PieceType::QUEEN, bestmove.value().get_piece());
    EXPECT_EQ(Rank::EIGHT, bestmove.value().get_end().get_rank());
    EXPECT_EQ(File::F, bestmove.value().get_end().get_file());
}
int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}