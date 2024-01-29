#pragma once

#include "listener.hh"
#include "chess_engine/board/chessboard-interface.hh"
#include "chess_engine/board/chessboard.hh"
#include "chess_engine/board/entity/position.hh"
#include "parsing/pgn_parser/pgn-move.hh"

namespace listener
{
    class ListenerManager : public board::ChessboardInterface
    {
    public:
        virtual ~ListenerManager()
        {
            for (size_t i = 0; i < dlls_.size(); ++i)
            {
                delete listeners_[i];
                int a = dlclose(dlls_[i]);
                if (a != 0)
                    fprintf(stderr, "%s\n", dlerror());
            }
        }

        virtual opt_piece_t operator[](
                const board::Position& position) const override
        {
            return chessboard_[position];
        }

        void play_pgn_moves(const std::vector<board::PgnMove> moves);

        void add_listener(void* dll, Listener* listener);

        void close_listeners(void);

    private:
        std::vector<Listener*> listeners_;
        board::Chessboard chessboard_;
        std::vector<void*> dlls_;

        void notify_move(board::Move move, opt_piece_t taken_piece) const;
        bool notify_board_state();
        bool do_move_and_notify(board::Move move);
    };

}