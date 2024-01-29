#pragma once

#include <vector>
#include <optional>

#include "chess_engine/board/entity/color.hh"
#include "chess_engine/board/entity/position.hh"
#include "parsing/perft_parser/fen-rank.hh"

namespace perft_parser
{
    class FenObject
    {
    public:
        using side_piece_t = std::pair<board::PieceType, board::Color>;
        using opt_piece_t = std::optional<side_piece_t>;

        FenObject(std::vector<FenRank>& ranks,
                    board::Color& side_to_move,
                    std::vector<char>& castling,
                    std::optional<board::Position> en_passant) : ranks_(ranks),
                    side_to_move_(side_to_move),
                    castling_(castling),
                    en_passant_target_(en_passant)
                    {}

        FenObject() = default;
        FenObject& operator=(const FenObject&) = default;

        // For test purpose
        bool operator==(const FenObject& rhs) const
        {
            return ranks_ == rhs.ranks_
                    && side_to_move_ == rhs.side_to_move_
                    && castling_ == rhs.castling_
                    && en_passant_target_ == rhs.en_passant_target_;
        }

        opt_piece_t operator[](const board::Position& pos) const
        {
            return ranks_.at(static_cast<int>(pos.get_rank()))[pos.get_file()];
        }

        board::Color side_to_move_to_get(void) const { return side_to_move_; }
        std::vector<char> castling_get(void) const { return castling_; }
        std::optional<board::Position> en_passant_target_get(void) const
        {
            return en_passant_target_;
        }


    private:
        std::vector<FenRank> ranks_;
        board::Color side_to_move_;
        std::vector<char> castling_;
        std::optional<board::Position> en_passant_target_;
    };
}