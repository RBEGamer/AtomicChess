#include <optional>

#include "ai-launcher.hh"
#include "chess_engine/ai/ai-mini.hh"
#include "chess_engine/ai/uci.hh"
#include "parsing/pgn_parser/ebnf-parser.hh"

namespace ai
{
    void play_ai(void)
    {
        uci::init("bLiPbLoP");
        AiMini ai = AiMini();
        board::Chessboard chessboard = board::Chessboard();
        while (true)
        {
            std::string board_str = uci::get_board();
            int16_t depth = pgn_parser::get_depth(uci::get_go()).value_or(5);
            pgn_parser::add_move_to_board(chessboard, board_str);
            std::optional<board::Move> move = ai.search(chessboard, depth);
            if (!move.has_value())
                break;
            uci::play_move(pgn_parser::move_to_string(move.value()));
            chessboard.do_move(move.value());
        }
    }
}