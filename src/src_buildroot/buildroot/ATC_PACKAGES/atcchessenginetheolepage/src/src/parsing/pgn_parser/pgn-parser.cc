#include "pgn-parser.hh"

#include <fstream>
#include <regex>
#include <iostream>

#include "pgn-exception.hh"
#include "chess_engine/board/entity/piece-type.hh"

namespace pgn_parser
{
    board::File to_file(char symbol)
    {
        return static_cast<board::File>(symbol - 'a');
    }

    board::Rank to_rank(char symbol)
    {
        return static_cast<board::Rank>(symbol - '1');
    }

    /*
    ** parse end of game. return false if it's not
    ** a valid end
    */
    bool parse_end(const std::string& word)
    {
        return word == "1/2-1/2" || word == "1-0" || word == "0-1"
            || word == "*";
    }

    /*
    ** Parse move-text.
    ** An exception is thrown if there are syntax errors
    */
    void parse_action(const std::string& word)
    {
        std::regex san_regex{
            "[KQBNR]?[a-h]?[1-8]?[x-]?[a-h][1-8](=[QBNR])?[+#]?"};
        if (!std::regex_match(word, san_regex) /* not a standard move */
            && word[0] != 'O' /* not a castling */)
            throw pgn_parser::PgnParsingException("syntax error", word);
    }

    board::PieceType parse_piecetype(char symbol)
    {
        switch (symbol)
        {
        case 'K':
            return board::PieceType::KING;
        case 'Q':
            return board::PieceType::QUEEN;
        case 'R':
            return board::PieceType::ROOK;
        case 'B':
            return board::PieceType::BISHOP;
        case 'N':
            return board::PieceType::KNIGHT;
        default:
            return board::PieceType::PAWN;
        }
    }

    /* simply parse turn number token */
    void parse_turn_number(const std::string& word)
    {
        std::regex number_regex{"[[:digit:]]+"};
        if (!std::regex_match(word, number_regex))
            throw pgn_parser::PgnParsingException("wrong turn number", word);
    }

    board::PgnMove parse_castling(const std::string& word, board::Color side)
    {
        std::regex castling_regex{"O-O(-O)?[+#]{0,1}"};
        if (!std::regex_match(word, castling_regex))
            throw pgn_parser::PgnParsingException("bad castling", word);

        /*
        ** distinction between small castling (2 x O)
        ** or big castling (3 x O)
        */
        unsigned counter = 0;
        for (char letter : word)
            if (letter == 'O')
                counter++;
        bool queen_side = counter == 3;
        return board::PgnMove::generate_castling(queen_side, side);
    }

    ReportType parse_report(const std::string& word)
    {
        if (word.back() == '+')
            return ReportType::CHECK;
        else if (word.back() == '#')
            return ReportType::CHECKMATE;
        return ReportType::NONE;
    }

    // parse the header of a pgn file as a string
    const std::string parse_header(std::ifstream& pgn)
    {
        std::string header_part;
        std::regex header_regex{"\\[.*\\]"};

        while (std::getline(pgn, header_part, '\n'))
        {
            if (!header_part.empty()
                && !std::regex_match(header_part, header_regex))
            {
                pgn.seekg(-header_part.length() - 1, pgn.cur);
                break;
            }
        }
        return header_part;
    }

    // parse the body of a pgn file as a list of string
    const std::vector<std::string> parse_body(std::ifstream& pgn)
    {
        /* tokenize everything */
        std::vector<std::string> tokens;
        std::string token;

        while (pgn >> token)
        {
            std::stringstream ss{token};
            std::string sub_token;

            while (std::getline(ss, sub_token, '.'))
                if (sub_token.length())
                    tokens.push_back(sub_token);
        }

        /* separating tokens in number | move | end */
        std::vector<std::string> moves_txt;
        unsigned idx = 0;

        try
        {
            for (const auto& tok : tokens)
            {
                if (parse_end(tok))
                    break;
                else if (idx++ % 3 == 0)
                    parse_turn_number(tok);
                else
                {
                    /* it should be an action */
                    parse_action(tok);
                    moves_txt.push_back(tok);
                }
            }
        } catch (const pgn_parser::PgnParsingException& parse_error)
        {
            std::cerr << parse_error.what() << '\n';
            std::exit(1);
        }

        return moves_txt;
    }

    // convert a list of string into a list of move
    const std::vector<board::PgnMove>
    string_to_move(const std::vector<std::string>& body)
    {
        auto moves =
            std::for_each(body.cbegin(), body.cend(), MoveTextParser{});
        return moves.moves_get();
    }

    board::PgnMove::opt_piece_t MoveTextParser::parse_promotion(char promotion)
    {
        return {board::char_to_piece(promotion)};
    }

    void MoveTextParser::operator()(const std::string& word)
    {
        using PieceType = board::PieceType;
        using Position = board::Position;
        using Color = board::Color;

        if (word.front() == 'O')
        {
            try
            {
                auto move = parse_castling(word, side_);
                move.report_set(parse_report(word));
                moves_.push_back(move);
            } catch (const pgn_parser::PgnParsingException& parse_error)
            {
                std::cerr << parse_error.what() << '\n';
                std::exit(1);
            }
        }
        else
        {
            auto iter = word.cbegin();
            /* get the moving piece type and incrementing pointer */
            auto moving_piece = parse_piecetype(*iter);

            if (moving_piece != PieceType::PAWN)
                iter++; // a piece was indicated => incr iterator

            Position start{to_file(*iter++), to_rank(*iter++)};

            bool piece_taken = (*iter++ == 'x');

            Position end{to_file(*iter++), to_rank(*iter++)};

            /*
            ** parse promotion if there is one + move iter
            ** after the '=X' where X is Q,R,B or N
            */
            board::PgnMove::opt_piece_t promotion;

            if (*iter == '=')
                promotion = parse_promotion(*++iter);
            auto move = board::PgnMove(start, end, moving_piece, piece_taken,
                                       parse_report(word), promotion);
            moves_.emplace_back(move);
        }
        side_ = side_ == Color::WHITE ? Color::BLACK : Color::WHITE;
    }

    const std::vector<board::PgnMove> MoveTextParser::moves_get()
    {
        return moves_;
    }

    const std::vector<board::PgnMove> parse_pgn(const std::string& file)
    {
        std::ifstream pgn(file);
        parse_header(pgn);

        auto body = parse_body(pgn);

        auto moves = string_to_move(body);
        return moves;
    }
} // namespace pgn_parser
