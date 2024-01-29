#ifndef H_PLAYER
#define H_PLAYER

#include <string>

class Player {
private:
  std::string m_name;
  float m_rating;
  char m_colour;

public:
  Player(std::string name, float rating, char colour)
      : m_name(std::move(name)), m_rating(rating), m_colour(colour) {}
  auto get_name() -> std::string;
  auto set_name(std::string name) -> void;
  auto get_rating() -> float;
  auto set_rating(float rating) -> void;
  auto get_colour() -> char;
  auto set_colour(char colour) -> void;
};

#endif
