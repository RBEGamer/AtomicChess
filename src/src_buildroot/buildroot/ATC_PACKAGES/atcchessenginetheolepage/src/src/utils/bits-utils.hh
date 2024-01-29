#pragma once

#include <cstdint>
#include <iostream>
#include <ostream>
#include <optional>

#include "chess_engine/board/entity/position.hh"

namespace utils
{
    inline int is_bit_set(const uint64_t bit, const int index)
    {
        return (bit >> index) & 1ULL;
    }

    inline uint64_t bitboard_from_pos(const std::optional<board::Position>& pos)
    {
        if (!pos.has_value())
            return 0ULL;
        return 1ULL << pos->get_index();
    }

    inline void print_bit(const uint64_t bit)
    {
        for (int i = 56; i < 64; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
        for (int i = 48; i < 56; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
        for (int i = 40; i < 48; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
        for (int i = 32; i < 40; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
        for (int i = 24; i < 32; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
        for (int i = 16; i < 24; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
        for (int i = 8; i < 16; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
        for (int i = 0; i < 8; ++i)
        {
            if (is_bit_set(bit, i))
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
    }

    inline uint64_t north_fill(uint64_t bitboard)
    {
        bitboard |= (bitboard << 8);
        bitboard |= (bitboard << 16);
        bitboard |= (bitboard << 32);

        return bitboard;
    }

    inline uint64_t south_fill(uint64_t bitboard)
    {
        bitboard |= (bitboard >> 8);
        bitboard |= (bitboard >> 16);
        bitboard |= (bitboard >> 32);

        return bitboard;
    }

    inline uint64_t file_fill(const uint64_t bitboard)
    {
        return north_fill(bitboard) | south_fill(bitboard);
    }

    // index = 0 means set first bit
    inline void set_bit(uint64_t& bit, const int index)
    {
        bit |= 1ULL << index;
    }

    inline void unset_bit(uint64_t& bit, const int index)
    {
        bit &= ~(1ULL << index);
    }

    // Get the least significant bit position and remove it from n
    inline int pop_lsb(uint64_t& n)
    {
        const int result = __builtin_ffsll(n) - 1;
        n &= n - 1;
        return result;
    }

    // Returns the number of bits in the number
    inline int bits_count(const uint64_t n)
    {
        return __builtin_popcountll(n);
    }

    // Returns the position of the least significant bit
    // Was forward
    inline int bit_scan_lowest(const uint64_t n)
    {
        if (n == 0ULL)
            return -1;
        return __builtin_ffsll(n) - 1;
    }

    // Returns the position of the most significant bit
    // Was reverse
    inline int bit_scan_highest(const uint64_t n)
    {
        if (n == 0ULL)
            return -1;
        return 63 - __builtin_clzll(n);
    }
}
