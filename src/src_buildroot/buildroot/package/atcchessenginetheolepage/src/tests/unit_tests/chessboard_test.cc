#include "gtest/gtest.h"

#include <sstream>
#include <unordered_set>

#include "chess_engine/board/chessboard.hh"
#include "utils/utype.hh"

using namespace board;

#define EXPECT_PIECETYPE(SidePiece, PieceType) do {\
    EXPECT_EQ(SidePiece.first, PieceType);\
} while (0)

#define EXPECT_COLOR(SidePiece, Color) do {\
    EXPECT_EQ(SidePiece.second, Color);\
} while (0)

#define EXPECT_PIECE(OptSidePiece, PieceType, Color) do {\
    EXPECT_TRUE(OptSidePiece.has_value());\
    EXPECT_PIECETYPE(OptSidePiece.value(), PieceType);\
    EXPECT_COLOR(OptSidePiece.value(), Color);\
} while (0)

#define EXPECT_NO_PIECE(OptSidePiece) do {\
    EXPECT_FALSE(OptSidePiece.has_value());\
} while (0)

Move dummy_move(const Position& start, const Position& end, const PieceType& piece)
{
    return Move(start, end, piece, false, false, false, false, false, std::nullopt);
}

Move dummy_double_pawn_push_move(const Position& start, const Position& end)
{
    return Move(start, end, PieceType::PAWN, false, true, false, false, false, std::nullopt);
}

Move dummy_en_passant_move(const Position& start, const Position& end)
{
    return Move(start, end, PieceType::PAWN, true, false, false, false, true, std::nullopt);
}

Move dummy_capture_move(const Position& start, const Position& end, PieceType piecetype)
{
    return Move(start, end, piecetype, true, false, false, false, false, std::nullopt);
}

Move dummy_promotion_move(const Position& start, const Position& end, PieceType promotion)
{
    return Move(start, end, PieceType::PAWN, false, false, false, false, false, promotion);
}

Move dummy_promotion_capture(const Position& start, const Position& end, PieceType promotion)
{
    return Move(start, end, PieceType::PAWN, true, false, false, false, false, promotion);
}

Move dummy_castling_move(const Position& king_start, const Position& king_end, bool king_side)
{
    return Move(king_start, king_end, PieceType::KING, false, false, !king_side, king_side, false, std::nullopt);
}

size_t count_pieces(const Chessboard& board)
{
    auto count = 0;

    for (size_t rank_i = 0; rank_i < Chessboard::width; rank_i++)
    {
        auto rank = static_cast<Rank>(rank_i);

        for (size_t file_i = 0; file_i < Chessboard::width; file_i++)
        {
            auto file = static_cast<File>(file_i);

            if (board[Position(file, rank)].has_value())
                count++;
        }
    }

    return count;
}

TEST(InitialPieces, PawnRankTypes)
{
    Chessboard virgin_board;

    const auto pawn_ranks = {Rank::TWO, Rank::SEVEN};

    for (size_t file_i = 0; file_i < Chessboard::width; file_i++)
    {
        auto file = static_cast<File>(file_i);

        for (auto rank : pawn_ranks)
        {
            auto side_piece = virgin_board[Position(file, rank)].value();
            EXPECT_PIECETYPE(side_piece, PieceType::PAWN);
        }
    }
}

// Make sure that the ranks 1 and 8 contains the right piecetypes
TEST(InitialPieces, EndRankTypes)
{
    Chessboard virgin_board;

    const auto end_ranks = {Rank::ONE, Rank::EIGHT};

    const size_t nb_piecetypes = 5;

    const std::array<PieceType, nb_piecetypes> piecetypes =
        { PieceType::ROOK,
          PieceType::KNIGHT,
          PieceType::BISHOP,
          PieceType::QUEEN,
          PieceType::KING,
    };

    std::vector<File> rook_files = {File::A, File::H};
    std::vector<File> knight_files = {File::B, File::G};
    std::vector<File> bishop_files = {File::C, File::F};
    std::vector<File> queen_files = {File::D};
    std::vector<File> king_files = {File::E};

    const std::array<std::vector<File>, nb_piecetypes> files =
        { rook_files,
          knight_files,
          bishop_files,
          queen_files,
          king_files
    };

    auto count = 0;

    for (size_t i = 0; i < nb_piecetypes; i++)
        for (auto file : files[i])
            for (auto rank : end_ranks)
            {
                auto side_piece = virgin_board[Position(file, rank)].value();
                EXPECT_PIECETYPE(side_piece, piecetypes[i]);
                count++;
            }

    EXPECT_EQ(count, 16);
}

TEST(InitialPieces, Colors)
{
    Chessboard virgin_board;

    const auto white_ranks = {Rank::ONE, Rank::TWO};
    const auto black_ranks = {Rank::SEVEN, Rank::EIGHT};

    for (size_t file_i = 0; file_i < Chessboard::width; file_i++)
    {
        auto file = static_cast<File>(file_i);

        for (auto rank : white_ranks)
        {
            auto white_side_piece = virgin_board[Position(file, rank)].value();
            EXPECT_COLOR(white_side_piece, Color::WHITE);
        }

        for (auto rank : black_ranks)
        {
            auto black_side_piece = virgin_board[Position(file, rank)].value();
            EXPECT_COLOR(black_side_piece, Color::BLACK);
        }
    }
}

TEST(InitialPieces, EmptyMid)
{
    Chessboard virgin_board;

    for (size_t rank_i = 2; rank_i < Chessboard::width - 2; rank_i++)
    {
        auto rank = static_cast<Rank>(rank_i);

        for (size_t file_i = 0; file_i < Chessboard::width; file_i++)
        {
            auto file = static_cast<File>(file_i);

            EXPECT_NO_PIECE(virgin_board[Position(file, rank)]);
        }
    }
}

TEST(InitialPieces, PieceCount)
{
    Chessboard board;

    EXPECT_EQ(count_pieces(board), 32);
}

// FIXME Black and white seems to be inverted in fen string parser
TEST(Constructor, FenString)
{
    Chessboard board = Chessboard("4B3/8/2Q1p3/1k3N2/8/b1n3R1/8/K7");

    std::stringstream ss;
    ss << board;

    std::string expected_string =
        "8         B      \n"
        "7                \n"
        "6     Q   p      \n"
        "5   k       N    \n"
        "4                \n"
        "3 b   n       R  \n"
        "2                \n"
        "1 K              \n"
        "  A B C D E F G H";

    EXPECT_EQ(ss.str(), expected_string);
}

TEST(Constructor, PerftObjectCastling1)
{
    Chessboard castling_test_board = Chessboard(parse_perft("8/8/8/8/8/8/8/8 w kQ - 0 0 0"));

    EXPECT_TRUE(castling_test_board.get_king_castling(Color::BLACK));
    EXPECT_TRUE(castling_test_board.get_queen_castling(Color::WHITE));
    EXPECT_FALSE(castling_test_board.get_king_castling(Color::WHITE));
    EXPECT_FALSE(castling_test_board.get_queen_castling(Color::BLACK));
}

TEST(Constructor, PerftObjectCastling2)
{
    Chessboard castling_test_board = Chessboard(parse_perft("8/8/8/8/8/8/8/8 w kQKq - 0 0 0"));

    EXPECT_TRUE(castling_test_board.get_king_castling(Color::BLACK));
    EXPECT_TRUE(castling_test_board.get_queen_castling(Color::WHITE));
    EXPECT_TRUE(castling_test_board.get_king_castling(Color::WHITE));
    EXPECT_TRUE(castling_test_board.get_queen_castling(Color::BLACK));
}

TEST(Constructor, PerftObjectTurn)
{
    Chessboard white_turn_board = Chessboard(parse_perft("8/8/8/8/8/8/8/8 w - - 0 0 0"));
    Chessboard black_turn_board = Chessboard(parse_perft("8/8/8/8/8/8/8/8 b - - 0 0 0"));

    EXPECT_TRUE(white_turn_board.get_white_turn());
    EXPECT_FALSE(black_turn_board.get_white_turn());
}

TEST(ToFenString, AfterConstruction)
{
    auto fen_strings = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
        "r2k3r/pppppppp/8/8/8/8/PPPPPPPP/R2K3R",
        "4k2r/8/8/8/8/8/8/R3K3",
        "r3k2r/pp1ppPpp/8/2pP4/2P2P2/1P4P1/P6P/R3K2R",
        "r3k2r/pp1pp1pp/8/2pPPp2/2P2P2/1P4P1/P6P/R3K2R",
        "4r3/8/4k3/8/4K3/8/8/4R3",
        "1r4K1/r7/k7/8/8/8/8/8",
        "4k2r/3PP3/3P4/8/8/8/8/4K3",
        "4k2r/8/8/3PP3/8/8/8/4K3",
        "r3k2r/8/8/8/8/8/8/2R1K2R",
        "4k3/8/8/8/8/8/8/R3K2R",
        "8/1n4N1/2k5/8/8/5K2/1N4n1/8",
        "B6b/8/8/8/2K5/4k3/8/b6B",
        "7k/RR6/8/8/8/8/rr6/7K",
        "7k/8/8/1p6/P7/8/8/7K",
        "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N",
        "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N",
        "r3k2r/8/8/8/8/8/8/1R2K2R",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R"
    };

    for (auto fen_string : fen_strings)
        EXPECT_EQ(Chessboard(fen_string).to_fen_string(), fen_string);
}

TEST(ToFenString, AfterMove)
{
    Chessboard board;

    auto white_knight_pos_1 = Position(File::B, Rank::ONE);
    auto white_knight_pos_2 = Position(File::A, Rank::THREE);
    auto white_1_to_2 = dummy_move(white_knight_pos_1, white_knight_pos_2, PieceType::KNIGHT);
    auto white_2_to_1 = dummy_move(white_knight_pos_2, white_knight_pos_1, PieceType::KNIGHT);

    auto black_knight_pos_1 = Position(File::G, Rank::EIGHT);
    auto black_knight_pos_2 = Position(File::H, Rank::SIX);
    auto black_1_to_2 = dummy_move(black_knight_pos_1, black_knight_pos_2, PieceType::KNIGHT);
    //auto black_2_to_1 = dummy_move(black_knight_pos_2, black_knight_pos_1, PieceType::KNIGHT);

    std::unordered_set<std::string> states;

    states.insert(board.to_fen_string());

    board.do_move(white_1_to_2);

    states.insert(board.to_fen_string());

    board.do_move(black_1_to_2);

    states.insert(board.to_fen_string());

    board.do_move(white_2_to_1);

    states.insert(board.to_fen_string());

    EXPECT_EQ(states.size(), 4);
}

TEST(Copy, DoMove)
{
    Chessboard board;
    Chessboard board_copy = board;

    auto start = Position(File::A, Rank::TWO);
    auto end = Position(File::A, Rank::THREE);

    board_copy.do_move(dummy_move(start, end, PieceType::PAWN));

    EXPECT_PIECE(board_copy[end], PieceType::PAWN, Color::WHITE);
    EXPECT_FALSE(board_copy.get_white_turn());

    EXPECT_NO_PIECE(board[end]);
    EXPECT_TRUE(board.get_white_turn());
}

TEST(Copy, State)
{
    Chessboard board = Chessboard("4k2r/3PP3/3P4/8/8/8/8/4K3");
    Chessboard board_copy = board;

    EXPECT_EQ(board.to_fen_string(), board_copy.to_fen_string());
}

TEST(Copy, Threefold)
{
    Chessboard board;
    Chessboard board_copy = board;

    auto white_knight_pos_1 = Position(File::B, Rank::ONE);
    auto white_knight_pos_2 = Position(File::A, Rank::THREE);
    auto white_1_to_2 = dummy_move(white_knight_pos_1, white_knight_pos_2, PieceType::KNIGHT);
    auto white_2_to_1 = dummy_move(white_knight_pos_2, white_knight_pos_1, PieceType::KNIGHT);

    auto black_knight_pos_1 = Position(File::G, Rank::EIGHT);
    auto black_knight_pos_2 = Position(File::H, Rank::SIX);
    auto black_1_to_2 = dummy_move(black_knight_pos_1, black_knight_pos_2, PieceType::KNIGHT);
    auto black_2_to_1 = dummy_move(black_knight_pos_2, black_knight_pos_1, PieceType::KNIGHT);

    const auto nb_turns = 4;
    for (auto half_turn = 0; half_turn < 2 * nb_turns; half_turn++)
    {
        EXPECT_FALSE(board_copy.is_draw());
        EXPECT_FALSE(board_copy.threefold_repetition());

        if (board_copy.get_white_turn())
        {
            if (board_copy[white_knight_pos_1].has_value())
                board_copy.do_move(white_1_to_2);
            else
                board_copy.do_move(white_2_to_1);
        }
        else
        {
            if (board_copy[black_knight_pos_1].has_value())
                board_copy.do_move(black_1_to_2);
            else
                board_copy.do_move(black_2_to_1);
        }
    }

    EXPECT_TRUE(board_copy.is_draw());
    EXPECT_TRUE(board_copy.threefold_repetition());

    EXPECT_FALSE(board.is_draw());
    EXPECT_FALSE(board.threefold_repetition());
}

TEST(DoMove, SimplePawnMove)
{
    Chessboard board;

    auto start_pos = Position(File::A, Rank::TWO);
    auto end_pos = Position(File::A, Rank::THREE);

    EXPECT_TRUE(board.get_white_turn());

    //  Move the leftmost white pawn one rank up
    board.do_move(dummy_move(start_pos, end_pos, PieceType::PAWN));

    EXPECT_FALSE(board.get_white_turn());
    EXPECT_NO_PIECE(board[start_pos]);

    EXPECT_PIECE(board[end_pos], PieceType::PAWN, Color::WHITE);

    EXPECT_EQ(count_pieces(board), 32);
}

TEST(DoMove, DoublePawnPush)
{
    Chessboard board;

    auto start_pos = Position(File::B, Rank::TWO);
    auto start_pos_front = Position(File::B, Rank::THREE);
    auto end_pos = Position(File::B, Rank::FOUR);

    EXPECT_TRUE(board.get_white_turn());

    board.do_move(dummy_double_pawn_push_move(start_pos, end_pos));

    EXPECT_FALSE(board.get_white_turn());

    EXPECT_NO_PIECE(board[start_pos]);
    EXPECT_NO_PIECE(board[start_pos_front]);

    EXPECT_PIECE(board[end_pos], PieceType::PAWN, Color::WHITE);

    EXPECT_EQ(count_pieces(board), 32);
}

TEST(DoMove, SimpleCapture)
{
    Chessboard board;

    auto start_pos = Position(File::A, Rank::TWO);
    auto end_pos = Position(File::A, Rank::THREE);

    EXPECT_TRUE(board.get_white_turn());

    //  Move the leftmost white pawn one rank up
    board.do_move(dummy_move(start_pos, end_pos, PieceType::PAWN));

    EXPECT_FALSE(board.get_white_turn());

    EXPECT_NO_PIECE(board[start_pos]);

    EXPECT_PIECE(board[end_pos], PieceType::PAWN, Color::WHITE);

    auto left_black_rook_pos = Position(File::A, Rank::EIGHT);

    board.do_move(dummy_capture_move(left_black_rook_pos, end_pos, PieceType::ROOK));

    EXPECT_TRUE(board.get_white_turn());

    EXPECT_NO_PIECE(board[left_black_rook_pos]);

    EXPECT_PIECE(board[end_pos], PieceType::ROOK, Color::BLACK);

    EXPECT_EQ(count_pieces(board), 31);
}

TEST(DoMove, ForgetEnPassant)
{
    Chessboard board;

    auto white_pawn_start = Position(File::B, Rank::TWO);
    auto white_pawn_end = Position(File::B, Rank::FOUR);
    auto en_passant_pos = Position(File::B, Rank::THREE);

    EXPECT_FALSE(board.get_en_passant().has_value());

    board.do_move(dummy_double_pawn_push_move(white_pawn_start, white_pawn_end));

    EXPECT_EQ(board.get_en_passant().value(), en_passant_pos);

    auto black_knight_start = Position(File::G, Rank::EIGHT);
    auto black_knight_end = Position(File::F, Rank::SIX);

    board.do_move(dummy_move(black_knight_start, black_knight_end, PieceType::ROOK));

    EXPECT_FALSE(board.get_en_passant().has_value());
}

TEST(DoMove, EnPassantCapture)
{
    Chessboard board;

    auto white_pawn_start = Position(File::C, Rank::TWO);
    auto white_pawn_end = Position(File::C, Rank::FIVE);

    board.do_move(dummy_move(white_pawn_start, white_pawn_end, PieceType::PAWN));

    auto black_pawn_start = Position(File::B, Rank::SEVEN);
    auto black_pawn_end = Position(File::B, Rank::FIVE);
    auto en_passant_pos = Position(File::B, Rank::SIX);

    board.do_move(dummy_double_pawn_push_move(black_pawn_start, black_pawn_end));

    EXPECT_EQ(board.get_en_passant().value(), en_passant_pos);

    board.do_move(dummy_en_passant_move(white_pawn_end, en_passant_pos));

    EXPECT_FALSE(board.get_en_passant().has_value());

    EXPECT_EQ(count_pieces(board), 31);
}

TEST(DoMove, PromotionMove)
{
    Chessboard board("8/6P1/8/8/8/8/8/8");

    auto white_pawn_start = Position(File::G, Rank::SEVEN);
    auto white_pawn_end = Position(File::G, Rank::EIGHT);

    board.do_move(dummy_promotion_move(white_pawn_start, white_pawn_end, PieceType::QUEEN));

    EXPECT_NO_PIECE(board[white_pawn_start]);
    EXPECT_PIECE(board[white_pawn_end], PieceType::QUEEN, Color::WHITE);
}

TEST(DoMove, PromotionCapture)
{
    Chessboard board("5p2/6P1/8/8/8/8/8/8");

    auto white_pawn_start = Position(File::G, Rank::SEVEN);
    auto white_pawn_end = Position(File::F, Rank::EIGHT);

    board.do_move(dummy_promotion_capture(white_pawn_start, white_pawn_end, PieceType::QUEEN));

    EXPECT_NO_PIECE(board[white_pawn_start]);
    EXPECT_PIECE(board[white_pawn_end], PieceType::QUEEN, Color::WHITE);
}

TEST(Draw, InitialState)
{
    Chessboard board;

    EXPECT_FALSE(board.is_draw());
}

TEST(Draw, Stalemate)
{
    // This is the subject example
    Chessboard board = Chessboard("8/8/8/1k6/8/b1n5/8/K7", Color::WHITE);

    EXPECT_TRUE(!board.has_legal_moves());
    EXPECT_TRUE(!board.is_check());
    EXPECT_TRUE(board.is_draw());
    EXPECT_TRUE(board.is_pat());
}

TEST(Draw, DoubleStalemate1)
{
    // Comes from wikipedia
    Chessboard board = Chessboard(parse_perft("NBk5/PpP1p3/1P2P3/8/8/3p1p2/3PpPpp/4Kbrq w - - 0 0 0"));

    EXPECT_TRUE(board.is_pat());

    board.set_white_turn(!board.get_white_turn());

    EXPECT_TRUE(board.is_pat());
}

TEST(Draw, DoubleStalemate2)
{
    // Comes from wikipedia too
    Chessboard board = Chessboard(parse_perft("8/1p6/bp6/rp6/qp6/kp6/1p6/1K6 w - - 0 0 0"));

    EXPECT_TRUE(board.is_pat());

    board.set_white_turn(!board.get_white_turn());

    EXPECT_TRUE(board.is_pat());
}

TEST(Draw, DoubleStalemate3)
{
    // Comes from wikipedia too
    Chessboard board = Chessboard(parse_perft("8/8/8/2p2p1p/2P2P1k/4pP1P/4P1KP/5BNR w - - 0 0 0"));

    EXPECT_TRUE(board.is_pat());

    board.set_white_turn(!board.get_white_turn());

    EXPECT_TRUE(board.is_pat());
}

// NOTE Will not pass anymore if we implement the threefold repetition bonus
TEST(Draw, DISABLED_FiftyLastTurns1)
{
    Chessboard board;

    auto white_knight_pos_1 = Position(File::B, Rank::ONE);
    auto white_knight_pos_2 = Position(File::A, Rank::THREE);
    auto white_1_to_2 = dummy_move(white_knight_pos_1, white_knight_pos_2, PieceType::KNIGHT);
    auto white_2_to_1 = dummy_move(white_knight_pos_2, white_knight_pos_1, PieceType::KNIGHT);

    auto black_knight_pos_1 = Position(File::G, Rank::EIGHT);
    auto black_knight_pos_2 = Position(File::H, Rank::SIX);
    auto black_1_to_2 = dummy_move(black_knight_pos_1, black_knight_pos_2, PieceType::KNIGHT);
    auto black_2_to_1 = dummy_move(black_knight_pos_2, black_knight_pos_1, PieceType::KNIGHT);

    const auto nb_turns = 50;
    for (auto half_turn = 0; half_turn < 2 * nb_turns; half_turn++)
    {
        EXPECT_FALSE(board.is_draw());

        if (board.get_white_turn())
        {
            if (board[white_knight_pos_1].has_value())
                board.do_move(white_1_to_2);
            else
                board.do_move(white_2_to_1);
        }
        else
        {
            if (board[black_knight_pos_1].has_value())
                board.do_move(black_1_to_2);
            else
                board.do_move(black_2_to_1);
        }
    }

    EXPECT_TRUE(board.get_white_turn());
    EXPECT_TRUE(board.is_draw());
}

// NOTE Will not pass anymore if we implement the threefold repetition bonus
TEST(Draw, DISABLED_FiftyLastTurns2)
{
    Chessboard board;

    auto white_knight_pos_1 = Position(File::B, Rank::ONE);
    auto white_knight_pos_2 = Position(File::A, Rank::THREE);
    auto white_1_to_2 = dummy_move(white_knight_pos_1, white_knight_pos_2, PieceType::KNIGHT);
    auto white_2_to_1 = dummy_move(white_knight_pos_2, white_knight_pos_1, PieceType::KNIGHT);

    auto black_knight_pos_1 = Position(File::G, Rank::EIGHT);
    auto black_knight_pos_2 = Position(File::H, Rank::SIX);
    auto black_1_to_2 = dummy_move(black_knight_pos_1, black_knight_pos_2, PieceType::KNIGHT);
    auto black_2_to_1 = dummy_move(black_knight_pos_2, black_knight_pos_1, PieceType::KNIGHT);


    const auto nb_turns = 49;
    for (auto half_turn = 0; half_turn < 2 * nb_turns; half_turn++)
    {
        EXPECT_FALSE(board.is_draw());

        if (board.get_white_turn())
        {
            if (board[white_knight_pos_1].has_value())
                board.do_move(white_1_to_2);
            else
                board.do_move(white_2_to_1);
        }
        else
        {
            if (board[black_knight_pos_1].has_value())
                board.do_move(black_1_to_2);
            else
                board.do_move(black_2_to_1);
        }
    }

    EXPECT_TRUE(board.get_white_turn());
    EXPECT_FALSE(board.is_draw());

    // black pawn move
    board.do_move(dummy_move(Position(File::A, Rank::SEVEN),
                             Position(File::A, Rank::SIX),
                             PieceType::PAWN));

    EXPECT_FALSE(board.is_draw());
}

TEST(Draw, Threefold)
{
    Chessboard board;

    auto white_knight_pos_1 = Position(File::B, Rank::ONE);
    auto white_knight_pos_2 = Position(File::A, Rank::THREE);
    auto white_1_to_2 = dummy_move(white_knight_pos_1, white_knight_pos_2, PieceType::KNIGHT);
    auto white_2_to_1 = dummy_move(white_knight_pos_2, white_knight_pos_1, PieceType::KNIGHT);

    auto black_knight_pos_1 = Position(File::G, Rank::EIGHT);
    auto black_knight_pos_2 = Position(File::H, Rank::SIX);
    auto black_1_to_2 = dummy_move(black_knight_pos_1, black_knight_pos_2, PieceType::KNIGHT);
    auto black_2_to_1 = dummy_move(black_knight_pos_2, black_knight_pos_1, PieceType::KNIGHT);

    std::unordered_set<std::string> states;

    states.insert(board.to_fen_string());

    const auto nb_turns = 4;
    for (auto half_turn = 0; half_turn < 2 * nb_turns; half_turn++)
    {
        EXPECT_FALSE(board.is_draw());
        EXPECT_FALSE(board.threefold_repetition());

        if (board.get_white_turn())
        {
            if (board[white_knight_pos_1].has_value())
                board.do_move(white_1_to_2);
            else
                board.do_move(white_2_to_1);
        }
        else
        {
            if (board[black_knight_pos_1].has_value())
                board.do_move(black_1_to_2);
            else
                board.do_move(black_2_to_1);
        }

        states.insert(board.to_fen_string());
    }

    EXPECT_EQ(states.size(), 4);

    EXPECT_TRUE(board.is_draw());
    EXPECT_TRUE(board.threefold_repetition());
}

TEST(Checkmate, True)
{
    // This is the subject example
    Chessboard white_checkmate_board = Chessboard(parse_perft("8/8/8/8/1k6/8/3q2r1/3K4 w - - 0 0 0"));

    EXPECT_TRUE(white_checkmate_board.is_checkmate());
}

TEST(Checkmate, False)
{
    // Slightly modified subject example
    Chessboard white_check_board = Chessboard(parse_perft("8/8/8/8/1k6/8/3q2r1/2K5 w - - 0 0 0"));

    EXPECT_FALSE(white_check_board.is_checkmate());
}

TEST(Check, True)
{
    // Slightly modified subject example
    Chessboard white_check_board = Chessboard(parse_perft("8/8/8/8/1k6/8/3q2r1/2K5 w - - 0 0 0"));

    EXPECT_TRUE(white_check_board.is_check());
}

TEST(Check, False)
{
    Chessboard white_check_board = Chessboard(parse_perft("8/5K2/5P2/8/1k6/5r2/3q4/8 w - - 0 0 0"));

    EXPECT_FALSE(white_check_board.is_check());
}

TEST(GenerateLegalMoves, IsLegalMove)
{
    // a valid board I found on the internet
    Chessboard board = Chessboard(parse_perft("2k4r/1pp1qN2/2nr2pp/pQ6/3P1B2/P3P3/1P3PPP/2R3K1 w - - 0 0 0"));

    auto legal_moves = board.generate_legal_moves();
    for (auto move : legal_moves)
        EXPECT_TRUE(board.is_move_legal(move));
}

TEST(GenerateLegalMoves, Length)
{
    Chessboard board = Chessboard(parse_perft("5k2/8/6K1/p7/8/8/8/R7 w - - 0 0 0"));

    auto nb_legal_king_moves = 6;
    // nb_vertical + nb_horizontal
    auto nb_legal_rook_moves = 4 + 7;
    auto nb_legal_moves = nb_legal_king_moves + nb_legal_rook_moves;

    auto legal_moves = board.generate_legal_moves();

    EXPECT_EQ(legal_moves.size(), nb_legal_moves);
}

TEST(Castling, Illegal)
{
    // Subject example
    // Isn't legal because the black bishop would put the white king in check
    Chessboard board = Chessboard(parse_perft("4k3/8/8/1b6/8/8/8/4K2R w - - 0 0 0"));

    auto legal_moves = board.generate_legal_moves();

    for (auto move : legal_moves)
    {
        EXPECT_FALSE(move.get_king_castling());
        EXPECT_FALSE(move.get_queen_castling());
    }
}

TEST(Castling, WhiteKingSideBlackQueenSide)
{
    Chessboard board = Chessboard(parse_perft("r3k3/8/8/8/8/8/8/4K2R w Kq - 0 0 0"));

    auto black_king_start = Position(File::E, Rank::EIGHT);
    auto black_rook_start = Position(File::A, Rank::EIGHT);
    auto white_king_start = Position(File::E, Rank::ONE);
    auto white_rook_start = Position(File::H, Rank::ONE);

    auto black_king_end = Position(File::C, Rank::EIGHT);
    auto black_rook_end = Position(File::D, Rank::EIGHT);
    auto white_king_end = Position(File::G, Rank::ONE);
    auto white_rook_end = Position(File::F, Rank::ONE);

    board.do_move(dummy_castling_move(white_king_start, white_king_end, true));
    board.do_move(dummy_castling_move(black_king_start, black_king_end, false));

    EXPECT_PIECE(board[black_king_end], PieceType::KING, Color::BLACK);
    EXPECT_PIECE(board[black_rook_end], PieceType::ROOK, Color::BLACK);
    EXPECT_PIECE(board[white_king_end], PieceType::KING, Color::WHITE);
    EXPECT_PIECE(board[white_rook_end], PieceType::ROOK, Color::WHITE);

    for (auto pos : {black_king_start, black_rook_start, white_king_start, white_rook_start})
        EXPECT_NO_PIECE(board[pos]);

    for (auto color : {Color::WHITE, Color::BLACK})
    {
        EXPECT_FALSE(board.get_king_castling(color));
        EXPECT_FALSE(board.get_queen_castling(color));
    }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
