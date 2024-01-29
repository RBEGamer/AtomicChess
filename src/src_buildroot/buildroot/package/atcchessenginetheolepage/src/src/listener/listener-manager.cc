#include <optional>
#include <vector>
#include <dlfcn.h>

#include "listener-manager.hh"
#include "chess_engine/board/entity/color.hh"
#include "chess_engine/board/entity/piece-type.hh"

#define NOTIFY(FUNC) for (auto listener : listeners_) listener->FUNC
#define GET_COLOR() chessboard_.get_white_turn()\
                    ? board::Color::WHITE\
                    : board::Color::BLACK
#define GET_OTHER_COLOR() chessboard_.get_white_turn()\
                    ? board::Color::BLACK\
                    : board::Color::WHITE

namespace listener
{
    void ListenerManager::notify_move(board::Move move,
                                      opt_piece_t taken_piece) const
    {
        NOTIFY(on_piece_moved(move.get_piece(), move.get_start(),
                                    move.get_end()));
        if (move.get_king_castling())
            NOTIFY(on_kingside_castling(GET_OTHER_COLOR()));
            // move already done so color changed
        else if (move.get_queen_castling())
            NOTIFY(on_queenside_castling(GET_OTHER_COLOR()));
        else
        {
            if (move.get_capture()) //FIXME better way ?
            {
                NOTIFY(on_piece_taken(taken_piece.value().first,
                                      move.get_end()));
            }
            if (move.get_promotion().has_value())
                NOTIFY(on_piece_promoted(move.get_promotion().value(),
                                         move.get_end()));
        }
    }

    bool ListenerManager::notify_board_state()
    {
        if (chessboard_.is_checkmate())
        {
            NOTIFY(on_player_mat(GET_COLOR()));
            return false;
        }

        if (chessboard_.is_check())
            NOTIFY(on_player_check(GET_COLOR()));

        if (chessboard_.is_pat())
        {
            NOTIFY(on_player_pat(GET_COLOR()));
            NOTIFY(on_draw());
            return false;
        }
        else if (chessboard_.is_draw())
        {
            NOTIFY(on_draw());
            return false;
        }
        return true;
    }

    bool ListenerManager::do_move_and_notify(board::Move move)
    {
        if (!chessboard_.is_move_legal(move))
        {
            NOTIFY(on_player_disqualified(GET_COLOR()));
            return false;
        }
        opt_piece_t possibly_taken_piece = std::nullopt;

        if (move.get_en_passant())
        {
            board::Position target = chessboard_.get_white_turn()
                ? move.get_end().translate(0, -1).value()
                : move.get_end().translate(0,  1).value();
            possibly_taken_piece.emplace(chessboard_[target].value());
        }
        else if (move.get_capture()) //FIXME better way ?
            possibly_taken_piece.emplace(chessboard_[move.get_end()].value());

        chessboard_.do_move(move);
        notify_move(move, possibly_taken_piece);

        return notify_board_state();
    }

    void ListenerManager::play_pgn_moves(
            const std::vector<board::PgnMove> moves)
    {
        NOTIFY(register_board(chessboard_));

        // quit if there is no moves
        if (moves.size() == 0)
            return;

        // first move
        board::Move move = moves[0].to_Move();
        if (!do_move_and_notify(move))
        {
            NOTIFY(on_game_finished());
            return;
        }

        // all other moves
        for (size_t i = 1; i < moves.size(); i++)
        {
            moves[i].to_Move(move);
            if (!do_move_and_notify(move))
            {
                NOTIFY(on_game_finished());
                break;
            }
        }
    }

    void ListenerManager::add_listener(void* dll, Listener* listener)
    {
        dlls_.emplace_back(dll);
        listeners_.emplace_back(listener);
    }

}
