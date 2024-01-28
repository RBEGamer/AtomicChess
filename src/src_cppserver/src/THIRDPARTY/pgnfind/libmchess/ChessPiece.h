// -*- C++ -*-
#ifndef _CHESSPIECE_H
#define _CHESSPIECE_H

class ChessPiece {
  //
  // Public data types
  //
 public:
  enum Type { Empty, Pawn, Knight, Bishop, Rook, Queen, King };
  enum Color { White, Black };

  //
  // Constructors and destructors
  //
 public:
  // For type = Empty, color is ignored
  ChessPiece(Type = Empty, Color = White);

  //
  // Accessors
  //
 public:
  Type get_type() const;
  Color get_color() const;
  // Return the character symbol for a piece (SAN notation)
  // lowercase p, n, b, etc. for black, uppercase for white
  char piece_char() const;

  //
  // Mutators
  //
 public:
  void set_type(Type);
  void set(char);
  void set_color(Color);

  //
  // Operators
  //
public:
  friend int operator==( const ChessPiece &, const ChessPiece & );

  //
  // Private data
  //
 private:
  Type type;
  Color color;
};

#endif /* !_CHESSPIECE_H */
