#include "option-parser.hh"

#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <dlfcn.h>

#include "chess_engine/board/move-initialization.hh"
#include "chess_engine/ai/ai-launcher.hh"
#include "listener/listener.hh"
#include "listener/listener-manager.hh"
#include "parsing/pgn_parser/pgn-parser.hh"
#include "parsing/perft_parser/perft-object.hh"
#include "parsing/perft_parser/perft-parser.hh"

using namespace boost::program_options;

namespace option_parser
{
    using side_piece_t = std::pair<board::PieceType, board::Color>;
    using opt_piece_t = std::optional<side_piece_t>;
    using opt_pos_t = std::optional<board::Position>;

    static void handle_dl_error(void)
    {
        fprintf(stderr, "%s\n", dlerror());
    }

    static void* safe_dl_call(void* result)
    {
        if (result == nullptr)
            handle_dl_error();
        dlerror(); // No error, need to clear the error buffer before next call
        return result;
    }

    static void load_listerners(std::vector<std::string> listeners_path,
                                listener::ListenerManager& manager)
    {
        const char* symbol_name = "listener_create";
        for (const std::string& listener_path : listeners_path)
        {
            void* handle = safe_dl_call(dlopen(listener_path.c_str(),
                                               RTLD_NOW | RTLD_GLOBAL));
            if (handle == nullptr)
                continue;
            void* symbol = safe_dl_call(dlsym(handle, symbol_name));
            if (symbol == nullptr)
                continue;
            listener::Listener* listener =
                    reinterpret_cast<listener::Listener*(*)()>(symbol)();
            manager.add_listener(handle, listener);
        }
    }

    static std::string get_file_content(const std::string& path)
    {
        std::ifstream t(path);
        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    }

    // Would be better in it's own file but fails when I try
    static uint64_t get_perft_value(board::Chessboard& board, int depth)
    {
        if (depth == 0)
            return 1;

        std::vector<board::Move> move_list = board.generate_legal_moves();

        if (depth == 1)
            return move_list.size();

        u_int64_t nodes = 0;

        for (const board::Move& m : move_list)
        {
            board::Chessboard copy = board;
            copy.do_move(m);
            nodes += get_perft_value(copy, depth - 1);
        }

        return nodes;
    }

    static void on_perft(std::string path)
    {
        perft_parser::PerftObject perftobj =
                perft_parser::parse_perft(get_file_content(path));
        board::Chessboard chessboard(perftobj);
        std::cout << get_perft_value(chessboard, perftobj.get_depth())
                  << std::endl;
    }

    void handle_input_option(int argc, const char* argv[])
    {
        listener::ListenerManager manager;
        try
        {
            std::string pgn_path, perft_path;
            std::vector<std::string> listeners_path;

            options_description desc{"Allowed options"};
            desc.add_options()
            ("help,h", "show usage")
            ("pgn", value<std::string>(&pgn_path), "path to the PGN game file")
            ("listeners,l", value<std::vector<std::string>>(&listeners_path),
                "list of paths to listener plugins")
            ("perft", value<std::string>(&perft_path), "path to a perft file");

            variables_map vm;
            store(parse_command_line(argc, argv, desc), vm);
            notify(vm);

            if (vm.count("help"))
                std::cout << desc << '\n'; // print help message
            else
            {
                board::MoveInitialization::get_instance();
                if (vm.count("listeners"))
                    load_listerners(listeners_path, manager);
                if (vm.count("pgn"))
                    manager.play_pgn_moves(pgn_parser::parse_pgn(pgn_path));
                else if (vm.count("perft"))
                    on_perft(perft_path);
                else
                    ai::play_ai();
            }
        }
        catch (const error &ex)
        {
            std::cerr << ex.what() << '\n';
        }
    }
}