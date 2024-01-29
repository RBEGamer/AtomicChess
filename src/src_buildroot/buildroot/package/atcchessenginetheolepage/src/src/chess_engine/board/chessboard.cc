#include "chessboard.hh"
#include "parsing/option_parser/option-parser.hh"
#include "entity/piece-type.hh"
#include "entity/color.hh"
#include "entity/position.hh"
#include "entity/move.hh"
#include "move-initialization.hh"
#include "move-generation.hh"

#include <cassert>
#include <optional>
#include <sstream>

namespace board
{
    Chessboard::Chessboard()
    {
        board_ = Board();
        board_.init_default();

        white_turn_ = true;

        white_king_castling_ = true;
        white_queen_castling_ = true;
        black_king_castling_ = true;
        black_queen_castling_ = true;

        en_passant_ = std::nullopt;

        turn_ = 0;
        last_fifty_turn_ = 0;

        register_state();
    }

    Chessboard::Chessboard(const FenObject& fen)
    {
        board_ = Board();

        white_turn_ = fen.side_to_move_to_get() == Color::WHITE;

        const auto castling_chars = fen.castling_get();
        const auto chars_begin = castling_chars.begin();
        const auto chars_end = castling_chars.end();

        white_king_castling_ =
                std::find(chars_begin, chars_end, 'K') != chars_end;
        white_queen_castling_ =
                std::find(chars_begin, chars_end, 'Q') != chars_end;
        black_king_castling_ =
                std::find(chars_begin, chars_end, 'k') != chars_end;
        black_queen_castling_ =
                std::find(chars_begin, chars_end, 'q') != chars_end;

        en_passant_ = fen.en_passant_target_get();

        // FIXME it should be an available data in the fen string
        turn_ = 0;
        last_fifty_turn_ = 0;

        for (size_t rank_i = 0; rank_i < width; rank_i++)
        {
            const auto rank = static_cast<Rank>(rank_i);

            for (size_t file_i = 0; file_i < width; file_i++)
            {
                const auto file = static_cast<File>(file_i);

                const auto pos = Position(file, rank);
                const auto opt_side_piece = fen[pos];
                if (opt_side_piece.has_value())
                {
                    const auto side_piece = opt_side_piece.value();
                    board_.set_piece(pos, side_piece.first, side_piece.second);
                }
            }
        }

        register_state();
    }

    Chessboard::Chessboard(const std::string& str, const Color& color)
            : Chessboard(parse_perft(str + ((color == Color::WHITE)
                                           ? std::string(" w - - 0 0 0")
                                           : std::string(" b - - 0 0 0"))))
    {}

    Chessboard::Chessboard(const PerftObject& perft)
            : Chessboard(perft.get_fen())
    {}

    Chessboard::Chessboard(const std::string& fen_string)
            : Chessboard(parse_perft(fen_string + std::string(" w - - 0 0 0")))
    {}

    char Chessboard::sidepiece_to_char(const PieceType& piece,
                                       const Color& color)
    {
        const char piece_char = piece_to_char(piece);

        return color == Color::WHITE ? piece_char : tolower(piece_char);
    }

    char Chessboard::sidepiece_to_char(const side_piece_t& sidepiece)
    {
        const char piece_char = piece_to_char(sidepiece.first);

        return sidepiece.second == Color::WHITE
                                    ? piece_char
                                    : tolower(piece_char);
    }

    std::ostream& Chessboard::write_fen_rank(std::ostream& os,
                                             const Rank rank) const
    {
        unsigned short empty_cells_count = 0;

        for (size_t file_i = 0; file_i < width; file_i++)
        {
            const auto file = static_cast<File>(file_i);

            const opt_piece_t opt_piece = (*this)[Position(file, rank)];

            if (opt_piece.has_value())
            {
                if (empty_cells_count != 0)
                {
                    os << empty_cells_count;
                    empty_cells_count = 0;
                }

                os << sidepiece_to_char(opt_piece.value());
            }
            else
            {
                empty_cells_count++;
            }
        }

        if (empty_cells_count != 0)
            os << empty_cells_count;

        return os;
    }

    std::ostream& Chessboard::write_fen_board(std::ostream& os) const
    {
        constexpr auto last_rank = Rank::EIGHT;
        constexpr auto last_rank_i = utils::utype(last_rank);

        write_fen_rank(os, last_rank);

        for (int rank_i = last_rank_i - 1; rank_i >= 0; rank_i--)
            write_fen_rank(os << '/', static_cast<Rank>(rank_i));

        return os;
    }

    std::string Chessboard::to_fen_string(void) const
    {
        std::stringstream ss;

        write_fen_board(ss);

        return ss.str();
    }

    void Chessboard::register_state()
    {
        states_history_.push_back(board_);
    }

    std::vector<Move> Chessboard::generate_legal_moves(void)
    {
        std::vector<Move> legal_moves;

        const std::vector<Move> possible_moves =
                move_generation::generate_all_moves(*this);

        for (const Move& move : possible_moves)
        {
            if (is_possible_move_legal(move))
            {
                legal_moves.push_back(move);
            }
        }

        return legal_moves;
    }

    bool Chessboard::has_legal_moves()
    {
        return !generate_legal_moves().empty();
    }

    void Chessboard::register_double_pawn_push(const Move& move,
                                               const Color color)
    {
        const Position& start = move.get_start();
        const Position& end = move.get_end();

        const auto start_rank_i = utils::utype(start.get_rank());
        const auto en_passant_rank_i = start_rank_i +
                                       (color == Color::WHITE ? 1 : -1);
        const auto en_passant_rank = static_cast<Rank>(en_passant_rank_i);

        en_passant_ = opt_pos_t(Position(end.get_file(), en_passant_rank));
    }

    void Chessboard::forget_en_passant(void)
    {
        if (en_passant_.has_value())
            en_passant_ = std::nullopt;
    }

    void Chessboard::update_draw_data(const Move& move)
    {
        if (move.get_capture() || move.get_piece() == PieceType::PAWN)
        {
            last_fifty_turn_ = 0;
            // If a piece is captured (ie if there will never be again as much
            // pieces as before on the board) or if a pawn is moved,
            // we know that no precedent board state will ever appear again
            states_history_.clear();
        }
        else
            if (!white_turn_)
                last_fifty_turn_++;
    }

    void Chessboard::eat_en_passant(const Move& move, const Color color)
    {
        const Position& end = move.get_end();

        const auto en_passant_rank_i = utils::utype(end.get_rank());
        const auto eaten_pawn_rank_i = en_passant_rank_i +
            (color == Color::WHITE ? -1 : 1);
        const auto eaten_pawn_rank = static_cast<Rank>(eaten_pawn_rank_i);

        const auto eaten_pawn_color = color == Color::WHITE ?
            Color::BLACK : Color::WHITE;

        board_.unset_piece(Position(end.get_file(), eaten_pawn_rank),
                       PieceType::PAWN, eaten_pawn_color);
    }

    void Chessboard::move_castling_rook(const Move& move, const Color color)
    {
        const Position& end = move.get_end();

        const auto king_file_i = utils::utype(end.get_file());
        const auto king_rank = end.get_rank();

        const auto rook_start_file = move.get_king_castling() ?
                                     File::H : File::A;
        const auto rook_end_file_i = king_file_i +
                                     (move.get_king_castling() ? -1 : 1);
        const auto rook_end_file = static_cast<File>(rook_end_file_i);

        const auto rook_start = Position(rook_start_file, king_rank);
        const auto rook_end = Position(rook_end_file, king_rank);

        board_.move_piece(rook_start, rook_end, PieceType::ROOK, color);
    }

    void Chessboard::update_white_castling_bools(const Move& move)
    {
        const Position queenside_rook_pos = Position(File::A, Rank::ONE);
        const Position kingside_rook_pos = Position(File::H, Rank::ONE);

        if (move.get_castling())
        {
            white_king_castling_ = false;
            white_queen_castling_ = false;
        }
        else
        {
            const auto move_piecetype = move.get_piece();

            if (move_piecetype == PieceType::KING)
            {
                //If the king is moved no castling will ever be possible again
                white_king_castling_ = false;
                white_queen_castling_ = false;
            }
            else if (move_piecetype == PieceType::ROOK)
            {
                const auto rook_start = move.get_start();

                // If a rook is moved,
                // it can only cancels one castling out of two
                if (rook_start == kingside_rook_pos)
                    white_king_castling_ = false;
                else if (rook_start == queenside_rook_pos)
                    white_queen_castling_ = false;
            }
        }
    }

    void Chessboard::update_black_castling_bools(const Move& move)
    {
        const Position queenside_rook_pos = Position(File::A, Rank::EIGHT);
        const Position kingside_rook_pos = Position(File::H, Rank::EIGHT);

        if (move.get_castling())
        {
            black_king_castling_ = false;
            black_queen_castling_ = false;
        }
        else
        {
            const auto move_piecetype = move.get_piece();

            if (move_piecetype == PieceType::KING)
            {
                //If the king is moved, no castling will ever be possible again
                black_king_castling_ = false;
                black_queen_castling_ = false;
            }
            else if (move_piecetype == PieceType::ROOK)
            {
                const auto rook_start = move.get_start();

                // If a rook is moved,
                // it can only cancels one castling out of two
                if (rook_start == kingside_rook_pos)
                    black_king_castling_ = false;
                else if (rook_start == queenside_rook_pos)
                    black_queen_castling_ = false;
            }
        }
    }

    void Chessboard::update_castling_bools(const Move& move, const Color color)
    {
        if (color == Color::WHITE)
            update_white_castling_bools(move);
        else
            update_black_castling_bools(move);
    }

    void Chessboard::do_move(const Move& move)
    {
        const Position& start = move.get_start();
        const Position& end = move.get_end();
        const Color color = get_playing_color();
        const PieceType piecetype = move.get_piece();

        // The piece that will be eaten if move is a capture
        const opt_piece_t opt_end_piece = (*this)[end];

        if (!move.get_capture())
            board_.move_piece(start, end, piecetype, color);

        // NOTE if a move is a double pawn push, then it cannot be a capture
        if (move.get_double_pawn_push())
            register_double_pawn_push(move, color);
        else
        {
            forget_en_passant();
            update_draw_data(move);

            if (move.get_en_passant())
            {
                eat_en_passant(move, color);
                board_.move_piece(start, end, piecetype, color);
            }
            else if (move.get_castling())
                move_castling_rook(move, color);
            else if (move.get_capture())
            {
                assert(opt_end_piece.has_value());
                side_piece_t eaten_piece = opt_end_piece.value();
                auto eaten_piece_type = eaten_piece.first;
                auto eaten_piece_color = eaten_piece.second;

                assert(eaten_piece_color == (color == Color::WHITE ?
                                             Color::BLACK : Color::WHITE));

                board_.unset_piece(end, eaten_piece_type, eaten_piece_color);
                board_.move_piece(start, end, piecetype, color);
            }

            if (move.get_promotion().has_value())
            {
                const auto new_piecetype = move.get_promotion().value();
                board_.change_piece_type(end,
                                         PieceType::PAWN,
                                         new_piecetype,
                                         color);
            }
        }
        update_castling_bools(move, color);

        // If black played, then a turned passed
        if (!white_turn_)
            turn_++;

        white_turn_ = !white_turn_;

        register_state();
    }

    bool Chessboard::is_move_possible(const Move& move)
    {
        // Move is invalid if in start the piece is not there or bad color
        // Scope is soooo perf, swiftly
        {
            const auto opt_piece = (*this)[move.get_start()];
            if (!opt_piece.has_value())
            {
                return false;
            }
            else if (opt_piece.value().first != move.get_piece()
                    || opt_piece.value().second != (white_turn_ ? Color::WHITE
                                                               : Color::BLACK))
            {
                return false;
            }
        }

        std::vector<Move> possible_moves;

        switch (move.get_piece())
        {
            case PieceType::QUEEN:
                move_generation::generate_queen_moves(*this, possible_moves);
                break;
            case PieceType::ROOK:
                move_generation::generate_rook_moves(*this, possible_moves);
                break;
            case PieceType::BISHOP:
                move_generation::generate_bishop_moves(*this, possible_moves);
                break;
            case PieceType::KNIGHT:
                move_generation::generate_knight_moves(*this, possible_moves);
                break;
            case PieceType::PAWN:
                move_generation::generate_pawn_moves(*this, possible_moves);
                break;
            case PieceType::KING:
                move_generation::generate_king_moves(*this, possible_moves);
                break;
        };

        const auto start = possible_moves.begin();
        const auto end = possible_moves.end();

        return std::find(start, end, move) != end;
    }

    bool Chessboard::is_possible_move_legal(const Move& move) const
    {
        Chessboard board_copy = *this;
        board_copy.do_move(move);
        board_copy.white_turn_ = !board_copy.white_turn_;
        return !board_copy.is_check();
    }

    bool Chessboard::is_move_legal(const Move& move)
    {
        return is_move_possible(move) && is_possible_move_legal(move);
    }

    bool Chessboard::pos_threatened(const Position& pos) const
    {
        const Color color = white_turn_ ? Color::WHITE : Color::BLACK;
        const Color opponent_color = white_turn_ ? Color::BLACK : Color::WHITE;
        const int index = pos.get_index();
        const MoveInitialization& m = MoveInitialization::get_instance();

        // Rook / Queens
        const uint64_t r = m.get_targets(PieceType::ROOK, index, board_());
        if (r & (board_(PieceType::ROOK, opponent_color)
            | board_(PieceType::QUEEN, opponent_color)))
            return true;

        // Bishop / Queens
        const uint64_t b = m.get_targets(PieceType::BISHOP, index, board_());
        if (b & (board_(PieceType::BISHOP, opponent_color)
            | board_(PieceType::QUEEN, opponent_color)))
            return true;

        // Knight
        const uint64_t n = m.get_targets(PieceType::KNIGHT, index, board_());
        if (n & board_(PieceType::KNIGHT, opponent_color))
            return true;

        // Pawn
        const uint64_t p = m.get_pawn_targets(index, color);
        if (p & board_(PieceType::PAWN, opponent_color))
            return true;

        // King
        const uint64_t k = m.get_targets(PieceType::KING, index, board_());
        if (k & board_(PieceType::KING, opponent_color))
            return true;

        return false;
    }

    bool Chessboard::is_check(void)
    {
        uint64_t kings = board_(PieceType::KING,
                                white_turn_ ? Color::WHITE : Color::BLACK);
        const int king_pos = utils::pop_lsb(kings);
        if (king_pos == -1)
            return false;
        return pos_threatened(Position(king_pos));
    }

    bool Chessboard::is_pat(void)
    {
        return !is_check() && !has_legal_moves();
    }

    bool Chessboard::is_pat(const std::vector<board::Move>& legal_moves,
                            const bool is_check)
    {
        return !is_check && legal_moves.empty();
    }

    bool Chessboard::is_checkmate(void)
    {
        return is_check() && !has_legal_moves();
    }

    bool Chessboard::is_checkmate(const std::vector<board::Move>& legal_moves,
                                  const bool is_check)
    {
        return is_check && legal_moves.empty();
    }

    bool Chessboard::threefold_repetition()
    {
        const auto& current_state = states_history_.back();

        unsigned current_state_count = 0;
        for (const auto& state : states_history_)
            if (state == current_state && ++current_state_count == 3)
                return true;

        return false;
    }

    bool Chessboard::is_draw(void)
    {
        return last_fifty_turn_ >= 50 || is_pat() || threefold_repetition();
    }

    bool Chessboard::is_draw(const std::vector<board::Move>& legal_moves,
                             const bool is_check)
    {
        return last_fifty_turn_ >= 50 || is_pat(legal_moves, is_check)
                || threefold_repetition();
    }

    Color Chessboard::get_playing_color() const
    {
        return white_turn_ ? Color::WHITE : Color::BLACK;
    }

    Board& Chessboard::get_board(void)
    {
        return board_;
    }

    const Board& Chessboard::get_board(void) const
    {
        return board_;
    }

    bool Chessboard::get_white_turn(void) const
    {
        return white_turn_;
    }

    void Chessboard::set_white_turn(const bool state)
    {
        white_turn_ = state;
    }

    Chessboard::opt_pos_t Chessboard::get_en_passant(void) const
    {
        return en_passant_;
    }

    bool Chessboard::get_king_castling(const Color& color) const
    {
        return color == Color::WHITE ?
            white_king_castling_:
            black_king_castling_;
    }

    bool Chessboard::get_queen_castling(const Color& color) const
    {
        return color == Color::WHITE ?
            white_queen_castling_:
            black_queen_castling_;
    }

    void Chessboard::set_king_castling(const Color& color, const bool state)
    {
        if (color == Color::WHITE)
            white_king_castling_ = state;
        else if (color == Color::BLACK)
            black_king_castling_ = state;
    }

    void Chessboard::set_queen_castling(const Color& color, const bool state)
    {
        if (color == Color::WHITE)
            white_queen_castling_ = state;
        else if (color == Color::BLACK)
            black_queen_castling_ = state;
    }

    Chessboard::opt_piece_t Chessboard::operator[](const Position& pos) const
    {
        return board_[pos];
    }

    bool Chessboard::operator==(const Chessboard& rhs) const
    {
        return white_king_castling_ == rhs.white_king_castling_
            && white_queen_castling_ == rhs.white_queen_castling_
            && black_king_castling_ == rhs.black_king_castling_
            && black_queen_castling_ == rhs.black_queen_castling_
            && board_ == rhs.board_
            && white_turn_ == rhs.white_turn_
            && en_passant_ == rhs.en_passant_
            && turn_ == rhs.turn_
            && last_fifty_turn_ == rhs.last_fifty_turn_;
    }

    std::ostream& operator<<(std::ostream& os, const Chessboard& board)
    {
        constexpr std::string_view sep = " ";

        for (int rank_i = Chessboard::width - 1; rank_i >= 0; rank_i--)
        {
            os << rank_i + 1;
            for (size_t file_i = 0; file_i < Chessboard::width; file_i++)
            {
                const auto file = static_cast<File>(file_i);
                const auto rank = static_cast<Rank>(rank_i);
                const auto curr_piece = board[Position(file, rank)];

                os << sep;

                if (curr_piece.has_value())
                {
                    const PieceType piecetype = curr_piece.value().first;
                    const Color piece_color = curr_piece.value().second;

                    os << Chessboard::sidepiece_to_char(piecetype,
                                                        piece_color);
                }
                else
                    os << empty_cell_char;
            }

            os << std::endl;
        }

        os << sep;
        for (size_t file_i = 0; file_i < Chessboard::width; file_i++)
            os << sep << char('A' + file_i);

        return os;
    }

}
