#include "gtest/gtest.h"

#include <vector>
#include <optional>
#include <fstream>
#include <string>

#include "parsing/perft_parser/fen-rank.hh"
#include "chess_engine/board/entity/position.hh"

using namespace board;
using namespace std;
using namespace perft_parser;

using side_piece_t = std::pair<board::PieceType, board::Color>;
using opt_piece_t = std::optional<side_piece_t>;
using opt_vector_t = std::vector<opt_piece_t>;

#define ROOK PieceType::ROOK
#define KNIGHT PieceType::KNIGHT
#define QUEEN PieceType::QUEEN
#define PAWN PieceType::PAWN
#define KING PieceType::KING
#define BISHOP PieceType::BISHOP

#define BL Color::BLACK
#define WH Color::WHITE

#define SD(X, Y) side_piece_t(X, Y)

TEST(fenrank, access_test)
{
    FenRank fenrank(opt_vector_t{nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH)});
    side_piece_t expected(QUEEN, WH);
    File f = File::A;
    File s = File::B;

    EXPECT_EQ(nullopt, fenrank[f]);
    EXPECT_EQ(expected, *fenrank[s]);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}