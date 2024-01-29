#include "piece.hpp"

Square::Square() : file{'0'}, rank{'0'} {}
Square::Square(char file, char rank) : file{file}, rank{rank} {}

Square::Square(std::string position) {
  file = position[0];
  rank = position[1];
}

auto Square::operator==(const Square &other) const -> bool {
  return (file == other.file && rank == other.rank);
}

auto Square::mirror() -> void {
  switch (rank) {
  case '1':
    rank = '8';
    break;
  case '2':
    rank = '7';
    break;
  case '3':
    rank = '6';
    break;
  case '4':
    rank = '5';
    break;
  case '5':
    rank = '4';
    break;
  case '6':
    rank = '3';
    break;
  case '7':
    rank = '2';
    break;
  case '8':
    rank = '1';
    break;
  }
}

Piece::Piece(char type, char colour, Square position)
    : m_type(type), m_colour(colour), m_position(position) {
  m_legal_moves = {};
}

auto Piece::get_type() const -> char { return m_type; }

auto Piece::get_colour() const -> char { return m_colour; }

auto Piece::get_file() const -> int { return int(m_position.file - 'a'); }

auto Piece::get_rank() const -> int { return int(m_position.rank - '1'); }

auto Piece::get_square() const -> Square { return m_position; }

auto Piece::move(Square dest) -> void { m_position = dest; }

Pawn::Pawn(char colour, Square position) : Piece('P', colour, position) {}

auto Pawn::generate_legal_moves(Board &board) -> std::vector<std::string> {
  return {"a3", "a4"};
}

Rook::Rook(char colour, Square position) : Piece('R', colour, position) {}
auto Rook::generate_legal_moves(Board &board) -> std::vector<std::string> {
  return {"NONE"};
}

Knight::Knight(char colour, Square position) : Piece('N', colour, position) {}
auto Knight::generate_legal_moves(Board &board) -> std::vector<std::string> {
  return {" "};
}

Bishop::Bishop(char colour, Square position) : Piece('B', colour, position) {}
auto Bishop::generate_legal_moves(Board &board) -> std::vector<std::string> {
  return {" "};
}

Queen::Queen(char colour, Square position) : Piece('Q', colour, position) {}
auto Queen::generate_legal_moves(Board &board) -> std::vector<std::string> {
  return {" "};
}

King::King(char colour, Square position) : Piece('K', colour, position) {}
auto King::generate_legal_moves(Board &board) -> std::vector<std::string> {
  return {" "};
}
