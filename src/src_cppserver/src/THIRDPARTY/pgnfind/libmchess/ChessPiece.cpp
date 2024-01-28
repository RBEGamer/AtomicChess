#include <ctype.h>
#include <iostream.h>

#include "ChessPiece.h"

ChessPiece::ChessPiece(ChessPiece::Type _type,
		       ChessPiece::Color _color):
  type( _type ), color( _color ) {
}

ChessPiece::Type ChessPiece::get_type() const {
  return type;
}

char ChessPiece::piece_char() const {
  char ret;

  switch ( type ) {
  case Empty:
    return ' ';
  case Pawn:
    ret = 'P';break;
  case Knight:
    ret = 'N';break;
  case Bishop:
    ret = 'B';break;
  case Rook:
    ret = 'R';break;
  case Queen:
    ret = 'Q';break;
  case King:
    ret = 'K';break;
  default:
    // Should never happen
    return 'X';
  }

  if ( color == Black )
    ret = tolower(ret);

  return ret;
}

ChessPiece::Color ChessPiece::get_color() const {
  return color;
}

void ChessPiece::set_type(ChessPiece::Type _type) {
  type = _type;
}

void ChessPiece::set(char c) {
  switch ( toupper(c) ) {
  case 'P':
    set_type( Pawn );break;
  case 'N':
    set_type( Knight );break;
  case 'B':
    set_type( Bishop );break;
  case 'R':
    set_type( Rook );break;
  case 'Q':
    set_type( Queen );break;
  case 'K':
    set_type( King );break;
  default:
    set_type( Empty );
  }

  if ( c == tolower(c) )
    set_color( Black );
  else
    set_color( White );
}
  

void ChessPiece::set_color(ChessPiece::Color _color) {
  color = _color;
}

int operator==( const ChessPiece &a, const ChessPiece &b ) {
  // One empty square is the same as another
  if ( a.type == ChessPiece::Empty && b.type == ChessPiece::Empty )
    return 1;

  return ( a.type == b.type && a.color == b.color );
}
