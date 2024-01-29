#include "player.hpp"

auto Player::get_name() -> std::string { return m_name; }

auto Player::set_name(std::string name) -> void { m_name = name; }

auto Player::get_rating() -> float { return m_rating; }

auto Player::set_rating(float rating) -> void { m_rating = rating; }

auto Player::get_colour() -> char { return m_colour; }

auto Player::set_colour(char colour) -> void { m_colour = colour; }
