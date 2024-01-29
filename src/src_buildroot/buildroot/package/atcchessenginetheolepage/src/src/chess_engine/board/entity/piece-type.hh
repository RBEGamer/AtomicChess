#pragma once

#include <array>
#include <optional>
#include <cstdint>
#include <stdexcept>

#include "utils/utype.hh"

namespace board
{
    /* PieceType is an enum representing every possible
     * piece type present on the board. The char associated
     * with each value is the ascii char representing the
     * piece on the board */
    enum class PieceType
    {
        QUEEN = 0,
        ROOK = 1,
        BISHOP = 2,
        KNIGHT = 3,
        PAWN = 4,
        KING = 5 // Last position allows better optimization (order-move)
    };

    using opt_piecetype_t = std::optional<PieceType>;

    constexpr size_t nb_pieces = 6;

    /* Can be useful to iterate over PieceTypes */
    constexpr std::array<PieceType, nb_pieces> piecetype_array
    {
        PieceType::QUEEN,  PieceType::ROOK, PieceType::BISHOP,
        PieceType::KNIGHT, PieceType::PAWN, PieceType::KING
    };

    constexpr std::array<PieceType, nb_pieces - 1> piecetype_array_without_king
    {
        PieceType::QUEEN,  PieceType::ROOK, PieceType::BISHOP,
        PieceType::KNIGHT, PieceType::PAWN
    };

    constexpr std::array<PieceType, nb_pieces - 2> minor_piecetype_array
    {
        PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT, PieceType::PAWN
    };

    inline PieceType char_to_piece(const char c)
    {
        switch (c)
        {
        case 'P':
            return PieceType::PAWN;
        case 'N':
            return PieceType::KNIGHT;
        case 'B':
            return PieceType::BISHOP;
        case 'R':
            return PieceType::ROOK;
        case 'Q':
            return PieceType::QUEEN;
        case 'K':
            return PieceType::KING;
        default:
            throw std::invalid_argument("Unknown piece");
        }
    }

    constexpr char empty_cell_char = ' ';
    constexpr std::array<char, nb_pieces> piece_chars =
    {
        'Q', 'R', 'B', 'N', 'P', 'K'
    };

    inline char piece_to_char(const PieceType piece)
    {
        return piece_chars[utils::utype(piece)];
    }

} // namespace board
