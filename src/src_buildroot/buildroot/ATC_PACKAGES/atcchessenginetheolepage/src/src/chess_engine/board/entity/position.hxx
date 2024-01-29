namespace board
{
    inline Position::Position(File file, Rank rank)
        : file_(file)
        , rank_(rank)
    {}

    inline Position::Position(int x, int y)
    {
        file_ = static_cast<File>(x);
        rank_ =  static_cast<Rank>(y);
    }

    inline Position::Position(int i)
    {
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 01000000

        assert(i >= 0 && i < 64);
        rank_ = static_cast<Rank>(i / 8);
        file_ = static_cast<File>(i % 8);
    }

    inline Position::Position(char file, char rank)
    {
        assert(islower(file) && isdigit(rank));
        file_ = static_cast<File>(file - 'a');
        rank_ =  static_cast<Rank>(rank - '0' - 1);
        // + 1 because ONE corresponds to 0
    }

    inline bool Position::operator==(const Position& pos) const
    {
        return get_file() == pos.get_file() && get_rank() == pos.get_rank();
    }

    inline bool Position::operator!=(const Position& pos) const
    {
        return !(*this == pos);
    }

    inline File Position::get_file() const
    {
        return file_;
    }

    inline Rank Position::get_rank() const
    {
        return rank_;
    }

    inline std::optional<Position> Position::translate(int f, int r) const
    {
        int new_file = static_cast<int>(file_) + f;
        int new_rank = static_cast<int>(rank_) + r;

        if (new_file < 0 || new_rank < 0 || new_file >= 8 || new_rank >= 8)
            return std::nullopt;

        return Position(static_cast<File>(new_file),
                        static_cast<Rank>(new_rank));
    }

    inline std::ostream& operator<<(std::ostream& os, const Position& pos)
    {
        os << "(" << (char)(static_cast<char>(pos.get_file()) + 'A');
        os << ";" << static_cast<int>(pos.get_rank()) + 1 << ")";
        return os;
    }

    inline int Position::get_index(void) const
    {
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 00000000
        // 10000000
        // rank_ = 1(0)     file_ = A(0)
        // expected = 0 (cause we set the bit at indice 0 aka 1th bit)
        return static_cast<int>(rank_) * 8 + static_cast<int>(file_);
    }
} // namespace board
