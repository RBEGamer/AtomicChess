#pragma once

#include <string>
#include <vector>

#include "pgn-move.hh"
#include "report-type.hh"

namespace pgn_parser
{
    // parse a PGN file into a list of moves
    const std::vector<board::PgnMove> parse_pgn(const std::string& file);

    /*
    ** Functor class used to parse the pgn part associated with
    ** moves. It's used by the main parse function 'parse_pgn' to
    ** translate standard algebraic notation to Move objects.
    */
    class MoveTextParser
    {
    public:
        // Constructor of the functor
        MoveTextParser() = default;

        // Parse a movetext token into a word and store it in moves_
        void operator()(const std::string& word);

        // return moves_ which contains all parsed moves
        const std::vector<board::PgnMove> moves_get();

    private:
        board::Color side_ = board::Color::WHITE;
        std::vector<board::PgnMove> moves_;

        /* Auxiliary functions */
        board::PgnMove::opt_piece_t parse_promotion(char promotion);
    };

    // Convert a character into a File
    board::File to_file(char symbol);

    // Convert a character into a rank
    board::Rank to_rank(char symbol);

    /* Parse a string, deduce if a move induce a check,
    ** a checkmate or nothing
    */
    bool parse_end(const std::string& word);

    /*
    ** Check is a string is a move and throw a exception
    ** if it is not
    */
    void parse_action(const std::string& word);

    // Convert a charater into a PieceType
    board::PieceType parse_piecetype(char symbol);

    /*
    ** Check is a string is a turn number and throw
    ** a exception if it is not
    */
    void parse_turn_number(const std::string& word);

    /*
    ** Parse a castling into a move and throw a exception
    ** if it is not
    */
    board::PgnMove parse_castling(const std::string& word, board::Color side);

    /*
    ** Parse a string into a report
    */
    ReportType parse_report(const std::string& word);

    /*
    ** Parse the header of a pgn. It is only to move
    ** forward our reader
    */
    const std::string parse_header(std::ifstream& pgn);

    /*
    ** Parse the body of a pgn file and extract each move
    */
    const std::vector<std::string> parse_body(std::ifstream& pgn);

    /*
    ** Convert a vector of string that represent the moves of
    ** the pgn into a vector of PgnMove
    */
    const std::vector<board::PgnMove>
    string_to_move(const std::vector<std::string>& body);
} // namespace pgn_parser
