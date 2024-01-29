#include "uci.hh"

#include <fnmatch.h>
#include <iostream>

namespace uci
{
    namespace
    {
        std::string get_input(const std::string& expected = "*")
        {
            // Get a command following the expected globbing
            // Ignoring all unrecognized commands
            std::string buffer;
            do
            {
                std::getline(std::cin, buffer);
                if ("quit" == buffer || "stopuci" == buffer)
                    exit(0);
                #ifdef LICHESS
                if ("isready" == buffer && "isready" != expected)
                    std::cout << "readyok" << std::endl;
                #endif /* LICHESS */
            } while (fnmatch(expected.c_str(), buffer.c_str(), 0));
            return buffer;
        }
    } // namespace

    void init(const std::string& name)
    {
        get_input("uci");
        std::cout << "id name " << name << '\n';
        std::cout << "id author " << name << '\n';
        std::cout << "uciok" << std::endl;
        get_input("isready");
        std::cout << "readyok" << std::endl;
        #ifdef LICHESS
        get_input("position *");
        #endif /* LICHESS */
    }

    void play_move(const std::string& move)
    {
        // Send the computed move
        std::cout << "bestmove " << move << std::endl;
    }

    void info(const int depth, const int evaluation_score)
    {
        // Send the computed move
        std::cout << "info "
                  << "depth " << depth << " "
                  << "score cp " << evaluation_score << std::endl;
    }

    std::string get_board()
    {
        auto board = get_input("position *"); // Get the board
        return board;
    }

    std::string get_go()
    {
        auto go_str = get_input("go *"); // Wait for a go from GUI
        return go_str;
    }
} // namespace ai
