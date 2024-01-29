#pragma once

#include <utility>
#include <cassert>
#include <iostream>
#include <optional>

#include "utils/utype.hh"

namespace board
{
    /* The file enum represent the colomns
     * on the board */
    enum class File
    {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H
    };

    /* The rank enum represent the lines
     * on the board */
    enum class Rank
    {
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT
    };

    /* Position represent a coordinate on the board */
    class Position final
    {
    public:
        Position() = delete;
        explicit Position(File file, Rank rank);
        explicit Position(int x, int y);
        explicit Position(int i);
        explicit Position(char file, char rank);

        Position& operator=(const Position& position) = default;

        File get_file() const;
        Rank get_rank() const;
        int get_index(void) const;

        bool operator==(const Position& pos) const;
        bool operator!=(const Position& pos) const;

        std::optional<Position> translate(int file, int rank) const;

        friend std::ostream& operator<<(std::ostream& os, const Position& pos);

    private:
        File file_; // col / x
        Rank rank_; // line / y
    };

} // namespace board

#include "position.hxx"