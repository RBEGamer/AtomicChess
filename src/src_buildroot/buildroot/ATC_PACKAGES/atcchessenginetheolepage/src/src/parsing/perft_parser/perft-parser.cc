#include "perft-parser.hh"

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include <optional>

#include "perft-object.hh"
#include "fen-rank.hh"

namespace perft_parser
{
    PerftObject parse_perft(const std::string& input)
    {
        std::vector<std::string> tokens;
        boost::split(tokens, input, boost::is_any_of(" "));
        assert(tokens.size() == 7);

        std::string depth_s = tokens.at(tokens.size() - 1);
        int depth = std::stoi(depth_s);
        tokens.pop_back(); // Remove depth

        FenObject fen = parse_fen(tokens);
        PerftObject result(fen, depth);

        return result;
    }


    //Fen example : rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 1

    FenObject parse_fen(std::vector<std::string>& splited_input)
    {
        assert(splited_input.size() == 6);

        // Split ranks
        std::vector<std::string> ranks;
        boost::split(ranks, splited_input.at(0), boost::is_any_of("/"));

        // TODO find more opti than that
        // Need to push front since ranks are given from 8 to 1
        std::vector<FenRank> franks;
        for (const std::string& rank : ranks)
        {
            franks.insert(franks.begin(), FenRank(rank));
        }

        // Color
        board::Color side_to_move = ((splited_input.at(1)[0] == 'w')
                                    ? board::Color::WHITE
                                    : board::Color::BLACK);

        // Castling (switching with the rook)
        std::vector<char> castling;
        if (splited_input.at(2)[0] != '-')
        {
            for (char c : splited_input.at(2))
                castling.emplace_back(c);
        }

        // En passant (swiftly)
        std::optional<board::Position> en_passant;
        if (splited_input.at(3)[0] == '-')
            en_passant = std::nullopt;
        else
            en_passant = board::Position(splited_input.at(3)[0],
                                            splited_input.at(3)[1]);

        // We don't care about the 2 last params

        return FenObject(franks, side_to_move, castling, en_passant);
    }
}