#pragma once

#include <vector>

#include "chess_engine/board/chessboard.hh"
#include "chess_engine/board/entity/move.hh"
#include "chess_engine/board/entity/piece-type.hh"
#include "chess_engine/board/entity/color.hh"
#include "chess_engine/board/entity/position.hh"

using namespace board;

namespace move_generation
{
    void generate_bishop_moves(const Chessboard& board,
                               std::vector<Move>& moves);
    void generate_rook_moves(const Chessboard& board,
                             std::vector<Move>& moves);
    void generate_queen_moves(const Chessboard& board,
                              std::vector<Move>& moves);
    void generate_knight_moves(const Chessboard& board,
                               std::vector<Move>& moves);
    void generate_king_moves(const Chessboard& board,
                             std::vector<Move>& moves);
    void generate_pawn_moves(const Chessboard& board,
                             std::vector<Move>& moves);

    std::vector<Move> generate_all_moves(const Chessboard& board);
} // namespace move_generation