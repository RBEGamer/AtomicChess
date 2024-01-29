#pragma once

#include <cstdint>

#include "entity/position.hh"
#include "entity/piece-type.hh"
#include "entity/color.hh"

namespace board
{
    class Board
    {
    public:
        using side_piece_t = std::pair<PieceType, Color>;
        using opt_piece_t = std::optional<side_piece_t>;
        using opt_pos_t = std::optional<Position>;

        Board();
        void init_default();

        opt_piece_t operator[](const Position& pos) const;
        bool operator==(const Board& rhs) const;

        uint64_t operator()() const;
        uint64_t operator()(const PieceType& piece) const;
        uint64_t operator()(const Color& color) const;
        uint64_t operator()(const PieceType& piece, const Color& color) const;

        void set_piece(const Position& pos,
                    const PieceType piecetype,
                    const Color color);

        void unset_piece(const Position& pos,
                        const PieceType piecetype,
                        const Color color);

        void move_piece(const Position& start,
                        const Position& end,
                        const PieceType piecetype,
                        const Color color);

        void change_piece_type(const Position& pos,
                            const PieceType old_type,
                            const PieceType new_type,
                            const Color color);

        void init_end_ranks(const PieceType piecetype, const File file);
        void symetric_init_end_ranks(const PieceType piecetype,
                                     const File file);

    private:
        uint64_t whites_;
        uint64_t blacks_;

        // 0 : Queen, 1 : Rook, 2 : Bishop, 3 : Knight, 4 : Pawn, 5 : King
        uint64_t pieces_[6];

        uint64_t get_whites(void) const;
        uint64_t get_blacks(void) const;
        uint64_t get_pawns(void) const;
        uint64_t get_queens(void) const;
        uint64_t get_kings(void) const;
        uint64_t get_rooks(void) const;
        uint64_t get_bishops(void) const;
        uint64_t get_knights(void) const;
    };
}