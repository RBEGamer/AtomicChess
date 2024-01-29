#include "pgn-move.hh"

namespace board
{
    PgnMove::PgnMove(const Position& start, const Position& end,
                     PieceType piece, bool capture, ReportType report,
                     const PgnMove::opt_piece_t& promotion)
        : start_(start)
        , end_(end)
        , piece_(piece)
        , promotion_(promotion)
        , capture_(capture)
        , report_(report)
        , queen_castling_(false)
        , king_castling_(false)
    {
        double_pawn_push_ = (piece_ == board::PieceType::PAWN) &&
                            (std::abs(utils::utype(start_.get_rank()) -
                                 utils::utype(end_.get_rank())) ==
                             2);
    }

    PgnMove::PgnMove(const Position& start, const Position& end,
                     PieceType piece, bool capture, ReportType report,
                     const PgnMove::opt_piece_t& promotion,
                     bool queen_castling, bool king_castling)
        : start_(start)
        , end_(end)
        , piece_(piece)
        , promotion_(promotion)
        , capture_(capture)
        , report_(report)
        , queen_castling_(queen_castling)
        , king_castling_(king_castling)
    {
        double_pawn_push_ = (piece_ == board::PieceType::PAWN) &&
                            (std::abs(utils::utype(start_.get_rank()) -
                                 utils::utype(end_.get_rank())) ==
                             2);
    }

    PgnMove PgnMove::generate_castling(bool queen_side, Color color)
    {
        static const Position wking_pos{File::E, Rank::ONE};
        static const Position bking_pos{File::E, Rank::EIGHT};
        static const PgnMove w_small{wking_pos,
                                     Position(File::G, Rank::ONE),
                                     PieceType::KING,
                                     false,
                                     ReportType::NONE,
                                     std::nullopt, false, true};
        static const PgnMove w_big{wking_pos,
                                   Position(File::C, Rank::ONE),
                                   PieceType::KING,
                                   false,
                                   ReportType::NONE,
                                   std::nullopt, true, false};
        static const PgnMove b_small{bking_pos,
                                     Position(File::G, Rank::EIGHT),
                                     PieceType::KING,
                                     false,
                                     ReportType::NONE,
                                     std::nullopt, false, true};
        static const PgnMove b_big{bking_pos,
                                   Position(File::C, Rank::EIGHT),
                                   PieceType::KING,
                                   false,
                                   ReportType::NONE,
                                   std::nullopt, true, false};

        if (color == Color::WHITE)
            return queen_side ? w_big : w_small;

        return queen_side ? b_big : b_small;
    }

    void PgnMove::report_set(ReportType report)
    {
        report_ = report;
    }

    board::Move PgnMove::to_Move() const
    {
        return Move(start_, end_, piece_, capture_, double_pawn_push_,
                    queen_castling_, king_castling_, false);
    }

    void PgnMove::to_Move(board::Move &previous_move) const
    {
        // Check if move is an en passant move
        const board::Position prev_end = previous_move.get_end();
        bool en_passant = previous_move.get_double_pawn_push() &&
                          piece_ == board::PieceType::PAWN &&
                          capture_ &&
                          end_.get_file() == prev_end.get_file() &&
                          (utils::utype(end_.get_rank()) == 5 ||
                          utils::utype(end_.get_rank()) == 2);

        previous_move = Move(start_, end_, piece_, capture_, double_pawn_push_,
                        queen_castling_, king_castling_,
                        en_passant, promotion_);
    }

} // namespace board
