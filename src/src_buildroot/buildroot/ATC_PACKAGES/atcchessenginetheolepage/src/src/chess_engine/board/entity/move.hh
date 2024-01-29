#pragma once

#include <optional>

#include "color.hh"
#include "piece-type.hh"
#include "position.hh"

namespace board
{
    class Move final
    {
    public:
        using opt_piece_t = std::optional<PieceType>;

        Move() = delete;

        explicit Move(const Position& start,
             const Position& end,
             const PieceType& piece,
             const bool capture,
             const bool double_pawn_push,
             const bool queen_castling,
             const bool king_castling,
             const bool en_passant,
             const opt_piece_t& promotion = std::nullopt);

        explicit Move(const Position& start,
             const Position& end,
             const PieceType& piece);

        explicit Move(const Position& start,
             const Position& end,
             const PieceType& piece,
             bool capture);

        bool operator==(const Move& move) const;
        bool operator!=(const Move& move) const;

        const Position& get_start(void) const;
        const Position& get_end(void) const;
        const PieceType& get_piece(void) const;
        bool get_capture(void) const;
        bool get_double_pawn_push(void) const;
        bool get_queen_castling() const;
        bool get_king_castling() const;
        bool get_castling() const;
        bool get_en_passant() const;
        const opt_piece_t& get_promotion() const;

        void set_start(const Position& start);
        void set_end(const Position& end);
        void set_piece(const PieceType& piece);
        void set_capture(bool capture);
        void set_double_pawn_push(bool double_pawn_push);
        void set_queen_castling(bool queen_castling);
        void set_king_castling(bool king_castling);
        void set_en_passant(bool en_passant);
        void set_promotion(const opt_piece_t& promotion);

        friend std::ostream& operator<<(std::ostream& os, const Position& pos);

    private:
        Position start_;
        Position end_;
        PieceType piece_;
        opt_piece_t promotion_;
        bool capture_;
        bool double_pawn_push_;
        bool queen_castling_;
        bool king_castling_;
        bool en_passant_;
    };
} // namespace board

#include "move.hxx"