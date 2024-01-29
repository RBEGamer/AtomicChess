#pragma once

#include <type_traits>

namespace utils
{
    /* this function is very useful when you want to get the
     * type a scoped enum. For example, if a scoped enum PieceType
     * is a char, and you want to print to associated ascii, just
     * do std::cout << utils::utype(enum); */
    template <typename E>
    constexpr auto utype(E enumerator) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(enumerator);
    }
} // namespace utils
