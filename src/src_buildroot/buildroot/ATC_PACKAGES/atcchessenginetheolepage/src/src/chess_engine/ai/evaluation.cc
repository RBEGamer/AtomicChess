#include "evaluation.hh"
#include "utils/bits-utils.hh"

using namespace board;

namespace ai
{
    inline bool no_queen_or_unique_minor_piece(const Chessboard& board,
                                               Color color)
    {
        const auto queen_board = board.get_board()(PieceType::QUEEN);
        const auto color_board = board.get_board()(color);

        if (!(queen_board & color_board))
            return true;

        bool minor_piece_encountered = false;

        for (auto piece : minor_piecetype_array)
        {
            const auto piece_board = board.get_board()(piece);
            if (piece_board & color_board)
            {
                if (minor_piece_encountered)
                    return false;

                minor_piece_encountered = true;
            }
        }

        return true;
    }

    bool is_end_game(const Chessboard& board)
    {
        return no_queen_or_unique_minor_piece(board, Color::WHITE) &&
            no_queen_or_unique_minor_piece(board, Color::BLACK);
    }

    int evaluate_material(const Chessboard& board)
    {
        int evaluation = 0;

        for (auto piece : piecetype_array)
        {
            uint64_t white_piece_board = board.get_board()(piece, Color::WHITE);
            uint64_t black_piece_board = board.get_board()(piece, Color::BLACK);

            const auto piece_i = utils::utype(piece);
            const auto piece_value = piecetype_values[piece_i];

            const auto white_piece_count = utils::bits_count(white_piece_board);
            const auto black_piece_count = utils::bits_count(black_piece_board);

            evaluation += (white_piece_count - black_piece_count) * piece_value;
        }

        return evaluation;
    }

    int evaluate_king_squares(const Chessboard& board)
    {
        const uint64_t white_board = board.get_board()(Color::WHITE);
        const uint64_t black_board = board.get_board()(Color::BLACK);

        auto white_king_board = white_board &
            board.get_board()(PieceType::KING);
        auto black_king_board = black_board &
            board.get_board()(PieceType::KING);

        auto white_king_i = utils::pop_lsb(white_king_board);
        auto black_king_i = utils::pop_lsb(black_king_board);

        if (is_end_game(board))
            return white_end_game_piece_square_table_king[white_king_i] -
                black_end_game_piece_square_table_king[black_king_i];


        return white_middle_game_piece_square_table_king[white_king_i] -
            black_middle_game_piece_square_table_king[black_king_i];
    }

    int evaluate_squares(const Chessboard& board)
    {
        int evaluation = 0;

        const uint64_t white_board = board.get_board()(Color::WHITE);
        const uint64_t black_board = board.get_board()(Color::BLACK);

        for (auto piece : piecetype_array_without_king)
        {
            uint64_t white_piece_board = white_board
                    & board.get_board()(piece);
            uint64_t black_piece_board = black_board
                    & board.get_board()(piece);

            const auto piece_i = utils::utype(piece);
            const auto& white_piece_square_table =
                    white_piece_square_tables[piece_i];
            const auto& black_piece_square_table =
                    black_piece_square_tables[piece_i];

            int white_pos = utils::pop_lsb(white_piece_board);
            while (white_pos >= 0)
            {
                evaluation += white_piece_square_table[white_pos];
                white_pos = utils::pop_lsb(white_piece_board);
            }

            int black_pos = utils::pop_lsb(black_piece_board);
            while (black_pos >= 0)
            {
                evaluation -= black_piece_square_table[black_pos];
                black_pos = utils::pop_lsb(black_piece_board);
            }
        }

        return evaluation + evaluate_king_squares(board);
    }

    inline uint64_t pawn_open_files(const Chessboard& board)
    {
        return utils::file_fill(board.get_board()(PieceType::PAWN));
    }

    int evaluate_file_openings(const Chessboard& board)
    {
        const auto open_files = pawn_open_files(board);
        if (!open_files)
            return 0;

        const auto white_board = board.get_board()(Color::WHITE);
        const auto black_board = board.get_board()(Color::BLACK);

        const auto queen_board = board.get_board()(PieceType::QUEEN);
        const auto rook_board = board.get_board()(PieceType::ROOK);

        int evaluation = 0;

        if (open_files & (white_board & queen_board))
            evaluation += queen_on_open_file_bonus;

        if (open_files & (black_board & queen_board))
            evaluation -= queen_on_open_file_bonus;

        if (open_files & (white_board & rook_board))
            evaluation += rook_on_open_file_bonus;

        if (open_files & (black_board & rook_board))
            evaluation -= rook_on_open_file_bonus;

        return evaluation;
    }

    int evaluate_king_safety(const Chessboard& board)
    {
        return evaluate_file_openings(board);
    }

    //  Result:
    // positive -> white advantage
    // negative -> black advantage
    int evaluate(const Chessboard& board)
    {
        return evaluate_material(board) + evaluate_squares(board) +
            evaluate_king_safety(board);
    }
}
