#include "move-generation.hh"

#include <vector>

#include "chessboard.hh"
#include "entity/move.hh"
#include "utils/utype.hh"
#include "utils/bits-utils.hh"
#include "move-initialization.hh"

using namespace board;

namespace move_generation
{
    static void generate_moves_aux(std::vector<Move>& moves,
                                   const Position& from,
                                   const PieceType& piece,
                                   uint64_t targets,
                                   const bool capture)
    {
        int to = utils::pop_lsb(targets);
        while (to >= 0)
        {
            moves.emplace_back(
                Position(from),
                Position(to),
                piece,
                capture);
            to = utils::pop_lsb(targets);
        }
    }

    static void generate_moves(
            const PieceType& piece,
            const Chessboard& board,
            std::vector<Move>& moves)
    {
        const Color color = board.get_playing_color();
        const Color opponent_color = get_opposite_color(color);
        uint64_t pieces = board.get_board()(piece, color);

        // Iterate over all piece of color currently playing
        int pos = utils::pop_lsb(pieces);
        while (pos >= 0)
        {
            Position from(pos);
            uint64_t targets = MoveInitialization::get_instance().get_targets(
                piece,
                pos,
                board.get_board()());

            // Remove pieces from player currently playing
            targets &= ~board.get_board()(color);

            // Extract non captures moves and captures moves
            uint64_t captures = targets & board.get_board()(opponent_color);
            uint64_t simples = targets & ~board.get_board()(opponent_color);

            generate_moves_aux(moves, from, piece, captures, true);
            generate_moves_aux(moves, from, piece, simples, false);

            pos = utils::pop_lsb(pieces);
        }
    }

    static void generate_king_castling(std::vector<Move>& moves,
                                       const Chessboard& board)
    {
        const Color color = board.get_playing_color();
        const Rank rank = (color == Color::WHITE) ? Rank::ONE : Rank::EIGHT;

        const uint64_t obstacles =
            utils::bitboard_from_pos(Position(File::F, rank))
            | utils::bitboard_from_pos(Position(File::G, rank));
        const uint64_t have_pieces_between = board.get_board()() & obstacles;

        if (have_pieces_between == 0ULL)
        {
            const bool attacked = board.pos_threatened(Position(File::E, rank))
                || board.pos_threatened(Position(File::F, rank))
                || board.pos_threatened(Position(File::G, rank));
            if (!attacked)
            {
                Move king_castling(Position(File::E, rank),
                                Position(File::G, rank),
                                PieceType::KING);
                king_castling.set_king_castling(true);
                moves.push_back(king_castling);
            }
        }
    }

    static void generate_queen_castling(std::vector<Move>& moves,
                                       const Chessboard& board)
    {
        const Color color = board.get_playing_color();
        const Rank rank = (color == Color::WHITE) ? Rank::ONE : Rank::EIGHT;

        const uint64_t obstacles =
                utils::bitboard_from_pos(Position(File::B, rank))
                | utils::bitboard_from_pos(Position(File::C, rank))
                | utils::bitboard_from_pos(Position(File::D, rank));
        const uint64_t have_pieces_between = board.get_board()() & obstacles;
        if (have_pieces_between == 0ULL)
        {
            const bool attacked = board.pos_threatened(Position(File::E, rank))
                || board.pos_threatened(Position(File::D, rank))
                || board.pos_threatened(Position(File::C, rank));
            if (!attacked)
            {
                Move queen_castling(Position(File::E, rank),
                                Position(File::C, rank),
                                PieceType::KING);
                queen_castling.set_queen_castling(true);
                moves.push_back(queen_castling);
            }
        }
    }

    void generate_bishop_moves(const Chessboard& board,
                               std::vector<Move>& moves)
    {
        return generate_moves(PieceType::BISHOP, board, moves);
    }

    void generate_rook_moves(const Chessboard& board,
                             std::vector<Move>& moves)
    {
        return generate_moves(PieceType::ROOK, board, moves);
    }

    void generate_queen_moves(const Chessboard& board,
                              std::vector<Move>& moves)
    {
        return generate_moves(PieceType::QUEEN, board, moves);
    }

    void generate_knight_moves(const Chessboard& board,
                               std::vector<Move>& moves)
    {
        return generate_moves(PieceType::KNIGHT, board, moves);
    }

    void generate_king_moves(const Chessboard& board,
                             std::vector<Move>& moves)
    {
        generate_moves(PieceType::KING, board, moves);

        if (board.get_king_castling(board.get_playing_color()))
            generate_king_castling(moves, board);
        if (board.get_queen_castling(board.get_playing_color()))
            generate_queen_castling(moves, board);
    }

    static void generate_pawns_promotions(std::vector<Move>& moves,
                                          const Position& from,
                                          const Position& to,
                                          const bool capture)
    {
        Move move(from, to, PieceType::PAWN, capture);

        Move queen_promotion = move;
        queen_promotion.set_promotion(PieceType::QUEEN);
        moves.push_back(queen_promotion);

        Move rook_promotion = move;
        rook_promotion.set_promotion(PieceType::ROOK);
        moves.push_back(rook_promotion);

        Move bishop_promotion = move;
        bishop_promotion.set_promotion(PieceType::BISHOP);
        moves.push_back(bishop_promotion);

        Move knight_promotion = move;
        knight_promotion.set_promotion(PieceType::KNIGHT);
        moves.push_back(knight_promotion);
    }

    static void generate_pawn_forward(std::vector<Move>& moves,
                                      const Chessboard& board,
                                      const uint64_t pawns,
                                      const Color& color)
    {
        uint64_t pawns_one_forward = ~board.get_board()()
            & (color == Color::WHITE
                ? (pawns << 8)
                : (pawns >> 8));

        int dest_one_forward = utils::pop_lsb(pawns_one_forward);
        while (dest_one_forward >= 0)
        {
            Position from(color == Color::WHITE
                    ? (dest_one_forward - 8)
                    : (dest_one_forward + 8));
            Position to(dest_one_forward);

            if ((color == Color::WHITE && dest_one_forward >= 56)
                || (color == Color::BLACK && dest_one_forward <= 7))
                generate_pawns_promotions(moves, from, to, false);
            else
                moves.emplace_back(from, to, PieceType::PAWN);

            dest_one_forward = utils::pop_lsb(pawns_one_forward);
        }
    }

    static void generate_pawn_double(std::vector<Move>& moves,
                                     const Chessboard& board,
                                     const uint64_t pawns,
                                     const Color& color)
    {
        uint64_t pawns_one_forward = ~board.get_board()()
            & (color == Color::WHITE
                ? ((defs::RANK_TWO & pawns) << 8)
                : ((defs::RANK_SEVEN & pawns) >> 8));
        uint64_t pawns_two_forward = ~board.get_board()()
            & (color == Color::WHITE
                ? (pawns_one_forward << 8)
                : (pawns_one_forward >> 8));

        int dest_two_forward = utils::pop_lsb(pawns_two_forward);
        while (dest_two_forward >= 0)
        {
            Position from(color == Color::WHITE
                    ? (dest_two_forward - 16)
                    : (dest_two_forward + 16));
            Position to(dest_two_forward);

            Move move(from, to, PieceType::PAWN);
            move.set_double_pawn_push(true);
            moves.push_back(move);

            dest_two_forward = utils::pop_lsb(pawns_two_forward);
        }
    }

    static void generate_pawn_attacks_left(std::vector<Move>& moves,
                                          const Chessboard& board,
                                          const uint64_t pawns,
                                          const Color& color)
    {
        const Color opponent_color = get_opposite_color(color);
        const uint64_t enemies = board.get_board()(opponent_color);

        uint64_t left_attacks = enemies
            & (color == Color::WHITE
                ? (pawns << 7) & ~defs::FILE_H
                : (pawns >> 9) & ~defs::FILE_H);

        int dest_left_attack = utils::pop_lsb(left_attacks);
        while (dest_left_attack >= 0)
        {
            Position from(color == Color::WHITE
                    ? (dest_left_attack - 7)
                    : (dest_left_attack + 9));
            Position to(dest_left_attack);

            if ((color == Color::WHITE && dest_left_attack >= 56)
                || (color == Color::BLACK && dest_left_attack <= 7))
                generate_pawns_promotions(moves, from, to, true);
            else
                moves.emplace_back(from, to, PieceType::PAWN, true);

            dest_left_attack = utils::pop_lsb(left_attacks);
        }
        if (board.get_en_passant().has_value())
        {
            uint64_t left_en_passant = utils::bitboard_from_pos(
                                                board.get_en_passant().value())
                & (color == Color::WHITE
                    ? (pawns << 7) & ~defs::FILE_H
                    : (pawns >> 9) & ~defs::FILE_H);

            int dest_left_en_passant = utils::pop_lsb(left_en_passant);
            if (dest_left_en_passant >= 0)
            {
                Position from(color == Color::WHITE
                    ? (dest_left_en_passant - 7)
                    : (dest_left_en_passant + 9));
                Position to(dest_left_en_passant);

                Move move(from, to, PieceType::PAWN, true);
                move.set_en_passant(true);
                moves.push_back(move);
            }
        }
    }

    static void generate_pawn_attacks_right(std::vector<Move>& moves,
                                          const Chessboard& board,
                                          const uint64_t pawns,
                                          const Color& color)
    {
        const Color opponent_color = get_opposite_color(color);
        const uint64_t enemies = board.get_board()(opponent_color);

        uint64_t right_attacks = enemies
            & (color == Color::WHITE
                ? (pawns << 9) & ~defs::FILE_A
                : (pawns >> 7) & ~defs::FILE_A);

        int dest_right_attack = utils::pop_lsb(right_attacks);
        while (dest_right_attack >= 0)
        {
            Position from(color == Color::WHITE
                    ? (dest_right_attack - 9)
                    : (dest_right_attack + 7));
            Position to(dest_right_attack);

            if ((color == Color::WHITE && dest_right_attack >= 56)
                || (color == Color::BLACK && dest_right_attack <= 7))
                generate_pawns_promotions(moves, from, to, true);
            else
                moves.emplace_back(from, to, PieceType::PAWN, true);

            dest_right_attack = utils::pop_lsb(right_attacks);
        }
        if (board.get_en_passant().has_value())
        {
            uint64_t right_en_passant = utils::bitboard_from_pos(
                                                board.get_en_passant().value())
                & (color == Color::WHITE
                    ? (pawns << 9) & ~defs::FILE_A
                    : (pawns >> 7) & ~defs::FILE_A);

            int dest_right_en_passant = utils::pop_lsb(right_en_passant);
            if (dest_right_en_passant >= 0)
            {
                Position from(color == Color::WHITE
                    ? (dest_right_en_passant - 9)
                    : (dest_right_en_passant + 7));
                Position to(dest_right_en_passant);

                Move move(from, to, PieceType::PAWN, true);
                move.set_en_passant(true);
                moves.push_back(move);
            }
        }
    }

    void generate_pawn_moves(const Chessboard& board,
                             std::vector<Move>& moves)
    {
        const Color color = board.get_playing_color();
        const uint64_t pawns = board.get_board()(PieceType::PAWN, color);

        generate_pawn_attacks_left(moves, board, pawns, color);
        generate_pawn_attacks_right(moves, board, pawns, color);
        generate_pawn_forward(moves, board, pawns, color);
        generate_pawn_double(moves, board, pawns, color);
    }

    std::vector<Move> generate_all_moves(const Chessboard& board)
    {
        std::vector<Move> res;
        generate_pawn_moves(board, res);
        generate_king_moves(board, res);
        generate_queen_moves(board, res);
        generate_knight_moves(board, res);
        generate_rook_moves(board, res);
        generate_bishop_moves(board, res);
        return res;
    }
} // namespace move_generation