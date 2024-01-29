#pragma once

namespace board
{
    /* The Color enum represent a side. */
    enum class Color : bool
    {
        WHITE = false,
        BLACK = true
    };

    inline Color get_opposite_color(const Color& color)
    {
        return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    }

} // namespace board
