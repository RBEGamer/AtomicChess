#include "gtest/gtest.h"

#include <vector>
#include <optional>
#include <fstream>
#include <string>

#include "parsing/perft_parser/perft-parser.hh"
#include "parsing/perft_parser/perft-object.hh"
#include "parsing/perft_parser/fen-object.hh"
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
#define FR(A, B, C, D, E, F, G, H) FenRank(opt_vector_t{A, B, C, D, E, F, G, H})
#define EMPTY_LINE FR(nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, nullopt)
#define FULL_PAWN(X) FR(SD(PAWN, X), SD(PAWN, X), SD(PAWN, X), SD(PAWN, X), SD(PAWN, X), SD(PAWN, X), SD(PAWN, X), SD(PAWN, X))
#define BACK_LINE(X) FR(SD(ROOK, X), SD(KNIGHT, X), SD(BISHOP, X), SD(QUEEN, X), SD(KING, X), SD(BISHOP, X), SD(KNIGHT, X), SD(ROOK, X))

static string get_file_content(string path)
{
    ifstream t(path);
    stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

string base_path = "../tests/given_perft/";

// Input : rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 1
TEST(fenobject, test_access)
{
    PerftObject parsed_obj = parse_perft(
                            get_file_content(base_path + "base_depth-1.perft"));
    vector<FenRank> ranks_{ BACK_LINE(BL),
                            FULL_PAWN(BL),
                            EMPTY_LINE,
                            EMPTY_LINE,
                            EMPTY_LINE,
                            EMPTY_LINE,
                            FULL_PAWN(WH),
                            BACK_LINE(WH)
                            };
    Color side_to_move_ = WH;
    vector<char> castling_{'K', 'Q', 'k', 'q'};
    optional<Position> en_passant_target_ = nullopt;
    FenObject fen(ranks_, side_to_move_, castling_, en_passant_target_);

    side_piece_t expected(ROOK, BL);

    Position f(File::A, Rank::ONE);
    Position s(File::C, Rank::THREE);

    EXPECT_EQ(nullopt, fen[s]);
    EXPECT_EQ(expected, *fen[f]);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}