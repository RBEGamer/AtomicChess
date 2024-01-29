#ifndef H_PIECE
#define H_PIECE

#include <string>
#include <vector>

#include "board.hpp"

class Board;

// Struct for the squares of the board
// Parameters:
// `file` - horizontal column index as a character
// `rank` - vertical row index
struct Square {
  char file{}; // a b c d e f g h
  char rank{}; // 1 2 3 4 5 6 7 8
  Square();
  Square(char file, char rank);
  Square(std::string position);
  struct HashFunction {
    auto operator()(const Square &position) const -> int {
      int rowHash = std::hash<char>()(position.file);
      int colHash = std::hash<char>()(position.rank) << 1;
      return rowHash ^ colHash;
    }
  };
  auto operator==(const Square &other) const -> bool;
  auto mirror() -> void;
};

// Base class for the pieces. Each piece derives from this class and implements
// its own `generate_legal_moves` Pieces are initialised and contained within
// the Board constructor as it takes ownership. Use the constructor for the
// specific piece you need eg `std::make_unique<Pawn>('w', Square{'a', '2'})`
class Piece {
private:
  char m_type;
  const char m_colour;
  Square m_position;
  std::vector<std::string> m_legal_moves;

public:
  Piece(char type, char colour, Square position);
  auto get_type() const -> char;
  auto get_colour() const -> char;
  auto get_file() const -> int;
  auto get_rank() const -> int;
  auto get_square() const -> Square;
  auto move(Square dest) -> void;
  auto virtual generate_legal_moves(Board &board)
      -> std::vector<std::string> = 0;
  virtual ~Piece() {}
};

class Pawn : public Piece {
public:
  Pawn(char colour, Square position);
  auto generate_legal_moves(Board &board) -> std::vector<std::string> override;
};

class Rook : public Piece {
public:
  Rook(char colour, Square position);
  auto generate_legal_moves(Board &board) -> std::vector<std::string> override;
};

class Knight : public Piece {
public:
  Knight(char colour, Square position);
  auto generate_legal_moves(Board &board) -> std::vector<std::string> override;
};

class Bishop : public Piece {
public:
  Bishop(char colour, Square position);
  auto generate_legal_moves(Board &board) -> std::vector<std::string> override;
};

class Queen : public Piece {
public:
  Queen(char colour, Square position);
  auto generate_legal_moves(Board &board) -> std::vector<std::string> override;
};

class King : public Piece {
public:
  King(char colour, Square position);
  auto generate_legal_moves(Board &board) -> std::vector<std::string> override;
};

#endif