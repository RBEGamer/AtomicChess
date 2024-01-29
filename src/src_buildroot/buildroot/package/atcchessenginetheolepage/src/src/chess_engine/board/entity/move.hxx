namespace board
{
    inline Move::Move(const Position& start,
               const Position& end,
               const PieceType& piece,
               const bool capture,
               const bool double_pawn_push,
               const bool queen_castling,
               const bool king_castling,
               const bool en_passant,
               const opt_piece_t& promotion)
        : start_(start)
        , end_(end)
        , piece_(piece)
        , promotion_(promotion)
        , capture_(capture)
        , double_pawn_push_(double_pawn_push)
        , queen_castling_(queen_castling)
        , king_castling_(king_castling)
        , en_passant_(en_passant)
    {}

    inline Move::Move(const Position& start,
               const Position& end,
               const PieceType& piece)
        : start_(start)
        , end_(end)
        , piece_(piece)
        , capture_(false)
        , double_pawn_push_(false)
        , queen_castling_(false)
        , king_castling_(false)
        , en_passant_(false)
    {}

    inline Move::Move(const Position& start,
               const Position& end,
               const PieceType& piece,
               const bool capture)
        : start_(start)
        , end_(end)
        , piece_(piece)
        , capture_(capture)
        , double_pawn_push_(false)
        , queen_castling_(false)
        , king_castling_(false)
        , en_passant_(false)
    {}

    inline bool Move::operator==(const Move& move) const
    {
        if (promotion_.has_value() != move.promotion_.has_value())
            return false;
        bool almost_equal = start_ == move.start_ &&
            end_ == move.end_ &&
            piece_ == move.piece_ &&
            capture_ == move.capture_ &&
            double_pawn_push_ == move.double_pawn_push_ &&
            queen_castling_ == move.queen_castling_ &&
            king_castling_ == move.king_castling_ &&
            en_passant_ == move.en_passant_;
        if (promotion_.has_value())
            return almost_equal
                    && promotion_.value() == move.promotion_.value();
        return almost_equal;
    }

    inline bool Move::operator!=(const Move& move) const
    {
        return !(*this == move);
    }

    inline const Position& Move::get_start() const
    {
        return start_;
    }

    inline const Position& Move::get_end() const
    {
        return end_;
    }

    inline const PieceType& Move::get_piece() const
    {
        return piece_;
    }

    inline bool Move::get_capture() const
    {
        return capture_;
    }

    inline bool Move::get_double_pawn_push() const
    {
        return double_pawn_push_;
    }

    inline bool Move::get_queen_castling() const
    {
        return queen_castling_;
    }

    inline bool Move::get_king_castling() const
    {
        return king_castling_;
    }

    inline bool Move::get_castling() const
    {
        return get_queen_castling() || get_king_castling();
    }

    inline bool Move::get_en_passant() const
    {
        return en_passant_;
    }

    inline const Move::opt_piece_t& Move::get_promotion() const
    {
        return promotion_;
    }

    inline void Move::set_start(const Position& start)
    {
        start_ = start;
    }

    inline void Move::set_end(const Position& end)
    {
        end_ = end;
    }

    inline void Move::set_piece(const PieceType& piece)
    {
        piece_ = piece;
    }

    inline void Move::set_capture(bool capture)
    {
        capture_ = capture;
    }

    inline void Move::set_double_pawn_push(bool double_pawn_push)
    {
        double_pawn_push_ = double_pawn_push;
    }

    inline void Move::set_queen_castling(bool queen_castling)
    {
        queen_castling_ = queen_castling;
    }

    inline void Move::set_king_castling(bool king_castling)
    {
        king_castling_ = king_castling;
    }

    inline void Move::set_en_passant(bool en_passant)
    {
        en_passant_ = en_passant;
    }

    inline void Move::set_promotion(const Move::opt_piece_t& promotion)
    {
        promotion_ = promotion;
    }

    inline std::ostream& operator<<(std::ostream& os, const Move& move)
    {
        os << "Move - " << piece_to_char(move.get_piece()) << " ";
        os << move.get_start() << " -> " << move.get_end() << " (";
        if (move.get_capture()) os << "capture";
        if (move.get_en_passant()) os << ", en passant";
        os << ")";
        return os;
    }
}
