#ifndef H_BOARD
#define H_BOARD

#include <algorithm>
#include <iostream>
#include <memory>
#include <random>

#include "piece.hpp"

class Piece;

class Board {
private:
  std::string m_current_fen;
  // Vector of pieces on board
  std::vector<std::unique_ptr<Piece>> m_pieces;
  char m_turn;

public:
  Board();
  Board(int variant);
  auto get_fen() -> std::string;
  auto set_fen(const std::string &fen_string) -> bool;
  auto static check_fen_is_valid(const std::string &fen) -> bool;
  auto print() -> void;
  auto print_fen() -> void;
  auto static print_position(std::string &fen) -> void;
  auto is_legal_move(const std::string &notation) -> bool;
  auto move(const std::string &notation) -> bool;
  auto parse_move(const std::string &move) -> void;
  auto get_pieces() -> std::vector<std::unique_ptr<Piece>> &;
};

#endif