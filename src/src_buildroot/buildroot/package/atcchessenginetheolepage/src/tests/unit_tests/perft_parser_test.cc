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
TEST(perft_parser_test, first_basic_case)
{
    PerftObject parsed_obj = parse_perft(
                            get_file_content(base_path + "base_depth-1.perft"));

    vector<FenRank> ranks_{
                            BACK_LINE(WH),
                            FULL_PAWN(WH),
                            EMPTY_LINE,
                            EMPTY_LINE,
                            EMPTY_LINE,
                            EMPTY_LINE,
                            FULL_PAWN(BL),
                            BACK_LINE(BL)
                            };

    Color side_to_move_ = WH;
    vector<char> castling_{'K', 'Q', 'k', 'q'};
    optional<Position> en_passant_target_ = nullopt;
    int depth = 1;

    FenObject fen(ranks_, side_to_move_, castling_, en_passant_target_);
    PerftObject expected(fen, depth);

    EXPECT_EQ(expected, parsed_obj);
}

TEST(perft_parser_test, hard_case)
{
    PerftObject parsed_obj = parse_perft("rnb1kbNr/2pPPppp/7r/P4P2/3N4/8/1Q1Q1Q1Q/6r1 b - f6 999 999 25");

    vector<FenRank> ranks_{
FR(nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, SD(ROOK, BL),  nullopt),
FR(nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH)),
EMPTY_LINE,
FR(nullopt, nullopt, nullopt, SD(KNIGHT, WH), nullopt, nullopt, nullopt, nullopt),
FR(SD(PAWN, WH), nullopt, nullopt, nullopt, nullopt, SD(PAWN, WH), nullopt, nullopt),
FR(nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, SD(ROOK, BL)),
FR(nullopt, nullopt, SD(PAWN, BL), SD(PAWN, WH), SD(PAWN, WH), SD(PAWN, BL), SD(PAWN, BL), SD(PAWN, BL)),
FR(SD(ROOK, BL), SD(KNIGHT, BL), SD(BISHOP, BL), nullopt, SD(KING, BL), SD(BISHOP, BL), SD(KNIGHT, WH), SD(ROOK, BL))
};

    Color side_to_move_ = BL;
    vector<char> castling_;
    optional<Position> en_passant_target_ = Position(File::F, Rank::SIX);
    int depth = 25;

    FenObject fen(ranks_, side_to_move_, castling_, en_passant_target_);
    PerftObject expected(fen, depth);

    EXPECT_EQ(expected, parsed_obj);
}

TEST(perft_parser, hard_case_neq)
{
        PerftObject parsed_obj = parse_perft("rNb1kbNr/2pPPppp/7r/P4P2/3N4/8/1Q1Q1Q1Q/6r1 b - f6 999 999 25");

    vector<FenRank> ranks_{
FR(SD(ROOK, BL), SD(KNIGHT, BL), SD(BISHOP, BL), nullopt, SD(KING, BL), SD(BISHOP, BL), SD(KNIGHT, WH), SD(ROOK, BL)),
FR(nullopt, nullopt, SD(PAWN, BL), SD(PAWN, WH), SD(PAWN, WH), SD(PAWN, BL), SD(PAWN, BL), SD(PAWN, BL)),
FR(nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, SD(ROOK, BL)),
FR(SD(PAWN, WH), nullopt, nullopt, nullopt, nullopt, SD(PAWN, WH), nullopt, nullopt),
FR(nullopt, nullopt, nullopt, SD(KNIGHT, WH), nullopt, nullopt, nullopt, nullopt),
EMPTY_LINE,
FR(nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH), nullopt, SD(QUEEN, WH)),
FR(nullopt, nullopt, nullopt, nullopt, nullopt, nullopt, SD(ROOK, BL),  nullopt)
};

    Color side_to_move_ = BL;
    vector<char> castling_;
    optional<Position> en_passant_target_ = Position(File::F, Rank::SIX);
    int depth = 25;

    FenObject fen(ranks_, side_to_move_, castling_, en_passant_target_);
    PerftObject expected(fen, depth);

    EXPECT_NE(expected, parsed_obj);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}