#pragma once

#include <string>

#include "listener/listener.hh"
#include "chess_engine/board/chessboard-interface.hh"
#include "chess_engine/board/chessboard.hh"
#include "chess_engine/board/entity/color.hh"
#include "chess_engine/board/entity/piece-type.hh"
#include "chess_engine/board/entity/position.hh"

using namespace std;

namespace listener
{
    class ChessboardInterface;

    class AlanListener : public Listener
    {
    public:
        AlanListener()
        {
        }

        virtual ~AlanListener() = default;

        virtual void
        register_board(const board::ChessboardInterface& board_interface)
        {
            board_ = dynamic_cast<const board::Chessboard*>(&board_interface);
        }

        /**
        ** \brief Event when the game finishes.
        ** It is always the last method called.
        */
        virtual void on_game_finished()
        {
            cout << "[GAME_FINISHED]" << endl;
            print_board();
        }

        const char* piece_name(const board::PieceType piece)
        {
            switch (piece)
            {
                case board::PieceType::KING:
                    return "KING";
                case board::PieceType::BISHOP:
                    return "BISHOP";
                case board::PieceType::KNIGHT:
                    return "KNIGHT";
                case board::PieceType::PAWN:
                    return "PAWN";
                case board::PieceType::ROOK:
                    return "ROOK";
                case board::PieceType::QUEEN:
                    return "QUEEN";
            }
            return "olo";
        }

        /**
        ** \brief Must be called when a piece is moved.
        **
        ** @param piece: The moving piece.
        ** @param from:  The initial position of the piece.
        ** @param to:    The final position of the piece.
        */
        virtual void on_piece_moved(const board::PieceType piece,
                                    const board::Position& from,
                                    const board::Position& to)
        {
            cout << "[PIECE_MOVE] piece moved : " << piece_name(piece)
                 << " from " << from << " to " << to << endl;
            print_board();
        }

        /**
        ** \brief Must be called every time a piece is taken.
        ** It is always called *after* the move that causes the attack.
        **
        ** @param piece: The taken piece.
        ** @param at:    The position where the piece is taken.
        */
        virtual void on_piece_taken(const board::PieceType piece,
                                    const board::Position& at)
        {
            cout << "[PIECE_TAKEN] piece taken : " << static_cast<int>(piece)
                 << " at " << at << endl;
            print_board();
        }

        /**
        ** \brief Must be called when a pawn is promoted.
        ** It always happens *after* the pawn was moved
        **
        ** @param piece: The chosen type of piece of the promotion.
        ** @param at:    The position of the piece promoted.
        */
        virtual void on_piece_promoted(const board::PieceType piece,
                                       const board::Position& at)
        {
            cout << "[PROMOTION] of type " << static_cast<int>(piece)
                 << " at " << at << endl;
            print_board();
        }


        virtual void on_kingside_castling(const board::Color color)
        {
            cout << "[KING_CASTLING] player :" << static_cast<int>(color)
                 << endl;
            print_board();
        }


        virtual void on_queenside_castling(const board::Color color)
        {
            cout << "[QUEEN_CASTLING] player :" << static_cast<int>(color)
                 << endl;
            print_board();
        }

        virtual void on_player_check(const board::Color color)
        {
            cout << "[CHECK] player :" << static_cast<int>(color) << endl;
            print_board();
        }


        virtual void on_player_mat(const board::Color color)
        {
            cout << "[MAT] player :" << static_cast<int>(color) << endl;
            print_board();
        }


        virtual void on_player_pat(const board::Color color)
        {
            cout << "[PAT] player :" << static_cast<int>(color) << endl;
            print_board();
        }

        virtual void on_player_disqualified(const board::Color color)
        {
            std::cout << "[DISQUALIFIED] player :" << static_cast<int>(color)
                      << endl;
            print_board();
        }

        virtual void on_draw()
        {
            std::cout << "[DRAW] ntm c'est fini" << std::endl;
            print_board();
        };

    private:
        void print_board(void) const
        {
            std::cout << (*board_) << std::endl;
        }
        const board::Chessboard* board_;
    };

} // namespace listener