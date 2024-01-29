#pragma once

#include <iostream>
#include <array>
#include <bitset>
#include <vector>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "board.hh"
#include "chessboard-interface.hh"
#include "parsing/perft_parser/perft-parser.hh"
#include "parsing/perft_parser/fen-object.hh"
#include "parsing/perft_parser/perft-object.hh"
#include "parsing/option_parser/option-parser.hh"
#include "entity/move.hh"
#include "entity/position.hh"
#include "entity/piece-type.hh"
#include "entity/color.hh"

using namespace perft_parser;

namespace board
{
    class Chessboard : public ChessboardInterface
    {
    public:
        constexpr static size_t width = 8;

        using side_piece_t = std::pair<PieceType, Color>;
        using opt_piece_t = std::optional<side_piece_t>;
        using opt_pos_t = std::optional<Position>;

        Chessboard();
        Chessboard(const FenObject&);
        Chessboard(const std::string& str, const Color& color);
        Chessboard(const PerftObject& perft);
        Chessboard(const std::string& fen_string);

        static char sidepiece_to_char(const side_piece_t& sidepiece);
        static char sidepiece_to_char(const PieceType& piece,
                                      const Color& color);
        std::string to_fen_string(void) const;

        bool pos_threatened(const Position& pos) const;
        std::vector<Move> generate_legal_moves(void);
        bool has_legal_moves(void);
        void do_move(const Move& move);
        bool is_move_legal(const Move& move);
        bool is_possible_move_legal(const Move& move) const;
        bool is_move_possible(const Move& move);
        bool is_check(void);
        bool is_checkmate(void);
        bool is_checkmate(const std::vector<board::Move>& legal_moves,
                          const bool is_check);
        bool is_pat(void);
        bool is_pat(const std::vector<board::Move>& legal_moves,
                    const bool is_check);
        bool threefold_repetition(void);
        bool is_draw(void);
        bool is_draw(const std::vector<board::Move>& legal_moves,
                     const bool is_check);

        Color get_playing_color() const;
        Board& get_board(void);
        const Board& get_board(void) const;
        opt_pos_t get_en_passant() const;
        bool get_white_turn() const;
        void set_white_turn(bool state);
        bool get_king_castling(const Color& color) const;
        bool get_queen_castling(const Color& color) const;
        void set_king_castling(const Color& color, bool state);
        void set_queen_castling(const Color& color, bool state);

        opt_piece_t operator[](const Position& pos) const;
        bool operator==(const Chessboard& rhs) const;
        friend std::ostream& operator<<(std::ostream& os,
                                        const Chessboard& board);

    private:
        Board board_;

        // used by threefold_repetition
        std::vector<Board> states_history_;

        bool white_turn_;
        bool white_king_castling_;
        bool white_queen_castling_;
        bool black_king_castling_;
        bool black_queen_castling_;
        opt_pos_t en_passant_;
        unsigned turn_;
        unsigned last_fifty_turn_;

        std::ostream& write_fen_rank(std::ostream& os, const Rank rank) const;
        std::ostream& write_fen_board(std::ostream& os) const;
        void register_state();
        void init_end_ranks(const PieceType piecetype, const File file);
        void symetric_init_end_ranks(const PieceType piecetype,
                                     const File file);
        void register_double_pawn_push(const Move& move, const Color color);
        void forget_en_passant(void);
        void update_draw_data(const Move& move);
        void eat_en_passant(const Move& move, const Color color);
        void move_castling_rook(const Move& move, const Color color);
        void update_white_castling_bools(const Move& move);
        void update_black_castling_bools(const Move& move);
        void update_castling_bools(const Move& move, const Color color);
    };

}
