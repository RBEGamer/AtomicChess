#pragma once

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <cassert>
#include <string>
#include <optional>
#include <string_view>

#include "chess_engine/board/chessboard.hh"
#include "chess_engine/board/entity/move.hh"
#include "chess_engine/board/entity/piece-type.hh"
#include "parsing/perft_parser/perft-parser.hh"
#include "utils/utype.hh"

using namespace board;

namespace pgn_parser
{
    using opt_piece_t = std::optional<PieceType>;

    std::string move_to_string(const board::Move& move)
    {
        std::string result;
        result.push_back((char)(utils::utype(move.get_start().get_file())
                                + 'a'));
        result.push_back((char)(utils::utype(move.get_start().get_rank())
                                + '1'));
        result.push_back((char)(utils::utype(move.get_end().get_file())
                                + 'a'));
        result.push_back((char)(utils::utype(move.get_end().get_rank())
                                + '1'));
        if (move.get_promotion().has_value())
        {
            switch(move.get_promotion().value())
            {
                case board::PieceType::QUEEN:
                    result.append("q");
                    break;
                case board::PieceType::ROOK:
                    result.append("r");
                    break;
                case board::PieceType::BISHOP:
                    result.append("b");
                    break;
                case board::PieceType::KNIGHT:
                    result.append("n");
                    break;
                case board::PieceType::PAWN:
                    assert(false);
                case board::PieceType::KING:
                    assert(false);
            }
        }
        return result;
    }

    static PieceType get_piece(char c)
    {
        switch (c)
        {
            case 'q':
                return PieceType::QUEEN;
            case 'n':
                return PieceType::KNIGHT;
            case 'b':
                return PieceType::BISHOP;
            case 'r':
                return PieceType::ROOK;
        }
        assert(false);
        return PieceType::KING;
    }

    board::Move string_to_move(const board::Chessboard& chessboard,
                               const std::string& str_move)
    {
        assert(str_move.size() == 4 || str_move.size() == 5);
        const board::Position start(static_cast<uint8_t>(str_move.at(0) - 'a'),
                (static_cast<uint8_t>(str_move.at(1) - '1')));
        const board::Position end(static_cast<uint8_t>(str_move.at(2) - 'a'),
                (static_cast<uint8_t>(str_move.at(3) - '1')));
        opt_piece_t promotion = (str_move.size() == 5)
                                ? std::optional<PieceType>(
                                        get_piece(str_move.at(4)))
                                : std::optional<PieceType>(std::nullopt);

        auto op_piece = chessboard[start];
        assert(op_piece.has_value());
        const PieceType piece = op_piece.value().first;
        const bool double_pawn_push = (piece == board::PieceType::PAWN)
                && (std::abs(utils::utype(start.get_rank())
                        - utils::utype(end.get_rank())) == 2);
        bool queen_castling = false;
        bool king_castling = false;

        static const Position wking_pos{File::E, Rank::ONE};
        static const Position bking_pos{File::E, Rank::EIGHT};
        static const Position w_king_side{File::G, Rank::ONE};
        static const Position w_queen_side{File::C, Rank::ONE};
        static const Position b_king_castling{File::G, Rank::EIGHT};
        static const Position b_queen_castling{File::C, Rank::EIGHT};
        if (piece == PieceType::KING)
        {
            if (start == wking_pos)
            {
                if (end == w_king_side)
                    king_castling = true;
                else if (end == w_queen_side)
                    queen_castling = true;
            }
            else if (start == bking_pos)
            {
                if (end == b_king_castling)
                    king_castling = true;
                else if (end == b_queen_castling)
                    queen_castling = true;
            }
        }

        // The pawn moved diagonally He moved diagonally yet there is no pawn
        // where he moved (tiens tiens tiens)
        const bool en_passant = piece == board::PieceType::PAWN
                                && start.get_file() != end.get_file()
                                && !chessboard[end].has_value();

        const bool capture = en_passant || chessboard[end].has_value();
        return Move(start, end, piece, capture, double_pawn_push,
                    queen_castling, king_castling, en_passant, promotion);
    }

    void add_move_to_board(board::Chessboard& chessboard,
                           const std::string& string_board)
    {
        static bool first = true;
        static constexpr std::string_view startpos = "startpos";
        static constexpr std::string_view move_str = "moves";
        std::vector<std::string> tokens;
        boost::split(tokens, string_board, boost::is_any_of(" "));
        assert(tokens.size() >= 2);

        if (first)
        {
            if (tokens.at(1) != startpos) // Init with a fenstring
            {
                constexpr int fen_size = 6;
                std::vector<std::string> fenstring;
                for (int i = 2; i < 2 + fen_size; ++i)
                    fenstring.emplace_back(tokens.at(i));
                chessboard = Chessboard(perft_parser::parse_fen(fenstring));
            }
        }

        if ((tokens.size() > 2 && tokens.at(1) == startpos
             && tokens.at(2) == move_str) // Case position start_pos moves
            || (tokens.size() > 8 && tokens.at(8) == move_str))
                // Case position fen_string param param param moves
        {
            const std::string last_move = tokens.back();
            chessboard.do_move(string_to_move(chessboard, last_move));
        }
    }

    std::optional<int16_t> get_depth(const std::string& go_str)
    {
        static constexpr std::string_view depth_str = "depth";
        std::vector<std::string> tokens;
        boost::split(tokens, go_str, boost::is_any_of(" "));

        if (tokens.at(1) == depth_str)
        {
            return std::stoi(tokens.at(2));
        }
        return std::nullopt;
    }
}