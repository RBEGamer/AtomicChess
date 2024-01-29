#pragma once

#include <optional>

#include "entity/color.hh"
#include "entity/piece-type.hh"
#include "entity/position.hh"

namespace board
{
    /*!
    ** \brief interface of the chessboard.
    **
    ** this class should be the mother class of the adapter.
    ** it contains the member functions needed by the listener
    ** to manipulate the board.
    */
    class ChessboardInterface
    {
    public:
        using side_piece_t = std::pair<PieceType, Color>;
        using opt_piece_t = std::optional<side_piece_t>;

        virtual ~ChessboardInterface() = default;

        /*!
        ** Access operator.
        ** @param position A position on the chessboard.
        ** @return Piece and Color, or nullopt_t if the square is empty
        */
        virtual opt_piece_t operator[](const Position& position) const = 0;
    };

} // namespace board
