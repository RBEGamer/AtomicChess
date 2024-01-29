#include <cassert>

#include "utils/bits-utils.hh"
#include "board.hh"
#include "entity/position.hh"
#include "entity/color.hh"

using namespace utils;

namespace board
{
    Board::Board()
    {
        whites_ = 0ULL;
        blacks_ = 0ULL;
        for (int i = 0; i < 6; ++i)
            pieces_[i] = 0ULL;
    }

    void Board::init_default()
    {
        for (size_t file_i = 0; file_i < 8; file_i++)
        {
            const auto file = static_cast<File>(file_i);
            set_piece(Position(file, Rank::TWO),
                      PieceType::PAWN, Color::WHITE);
            set_piece(Position(file, Rank::SEVEN),
                      PieceType::PAWN, Color::BLACK);
        }
        symetric_init_end_ranks(PieceType::ROOK, File::A);
        symetric_init_end_ranks(PieceType::KNIGHT, File::B);
        symetric_init_end_ranks(PieceType::BISHOP, File::C);
        init_end_ranks(PieceType::QUEEN, File::D);
        init_end_ranks(PieceType::KING, File::E);
    }

    Board::opt_piece_t Board::operator[](const Position& pos) const
    {
        const int index = pos.get_index();
        const bool white = is_bit_set(whites_, index);
        const bool black = is_bit_set(blacks_, index);
        if (!white && !black)
            return std::nullopt;

        const Color piece_color = ((white) ? Color::WHITE : Color::BLACK);
        if (is_bit_set((*this)(PieceType::PAWN), index))
            return Board::side_piece_t(PieceType::PAWN, piece_color);
        if (is_bit_set((*this)(PieceType::ROOK), index))
            return Board::side_piece_t(PieceType::ROOK, piece_color);
        if (is_bit_set((*this)(PieceType::KNIGHT), index))
            return Board::side_piece_t(PieceType::KNIGHT, piece_color);
        if (is_bit_set((*this)(PieceType::BISHOP), index))
            return Board::side_piece_t(PieceType::BISHOP, piece_color);
        if (is_bit_set((*this)(PieceType::QUEEN), index))
            return Board::side_piece_t(PieceType::QUEEN, piece_color);
        if (is_bit_set((*this)(PieceType::KING), index))
            return Board::side_piece_t(PieceType::KING, piece_color);

        assert(false);
        return std::nullopt;
    }

    bool Board::operator==(const Board& rhs) const
    {
        if (whites_ != rhs.whites_ || blacks_ != rhs.blacks_)
            return false;
        for (int i = 0; i < 6; ++i)
            if (pieces_[i] != rhs.pieces_[i])
                return false;
        return true;
    }

    void Board::set_piece(const Position& pos,
                          const PieceType piecetype,
                          const Color color)
    {
        const int index = pos.get_index();
        set_bit(pieces_[static_cast<uint8_t>(piecetype)], index);
        if (color == Color::WHITE)
            set_bit(whites_, index);
        else
            set_bit(blacks_, index);
    }

    void Board::unset_piece(const Position& pos,
                            const PieceType piecetype,
                            const Color color)
    {
        const int index = pos.get_index();
        if (color == Color::WHITE)
        {
            if (!is_bit_set(blacks_
                & pieces_[static_cast<uint8_t>(piecetype)], index))
                unset_bit(pieces_[static_cast<uint8_t>(piecetype)], index);
            unset_bit(whites_, index);
        }
        else
        {
            if (!is_bit_set(whites_
                & pieces_[static_cast<uint8_t>(piecetype)], index))
                unset_bit(pieces_[static_cast<uint8_t>(piecetype)], index);
            unset_bit(blacks_, index);
        }
    }

    void Board::move_piece(const Position& start,
                           const Position& end,
                           const PieceType piecetype,
                           const Color color)
    {
        unset_piece(start, piecetype, color);
        set_piece(end, piecetype, color);
    }

    void Board::change_piece_type(const Position& pos,
                                  const PieceType old_type,
                                  const PieceType new_type,
                                  const Color color)
    {
        unset_piece(pos, old_type, color);
        set_piece(pos, new_type, color);
    }

    void Board::init_end_ranks(const PieceType piecetype, const File file)
    {
        constexpr Rank white_end_rank = Rank::ONE;
        constexpr Rank black_end_rank = Rank::EIGHT;

        set_piece(Position(file, white_end_rank), piecetype, Color::WHITE);
        set_piece(Position(file, black_end_rank), piecetype, Color::BLACK);
    }

    File symetric_file(const File file)
    {
        switch (file)
        {
        case File::A:
            return File::H;
        case File::B:
            return File::G;
        case File::C:
            return File::F;
        case File::D:
            return File::E;
        case File::E:
            return File::D;
        case File::F:
            return File::C;
        case File::G:
            return File::B;
        case File::H:
            return File::A;
        default:
            throw std::invalid_argument("Unknown file");
        }
    }

    void Board::symetric_init_end_ranks(const PieceType piecetype,
                                        const File file)
    {
        init_end_ranks(piecetype, file);
        init_end_ranks(piecetype, symetric_file(file));
    }

    uint64_t Board::operator()() const
    {
        return whites_ | blacks_;
    }

    uint64_t Board::operator()(const PieceType& piece) const
    {
        return pieces_[static_cast<uint8_t>(piece)];
    }

    uint64_t Board::operator()(const Color& color) const
    {
        return color == Color::WHITE ? whites_ : blacks_;
    }

    uint64_t Board::operator()(const PieceType& piece, const Color& color) const
    {
        return (*this)(piece) & (*this)(color);
    }
}