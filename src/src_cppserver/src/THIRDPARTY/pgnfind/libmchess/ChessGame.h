// -*- C++ -*-
#ifndef _CHESSGAME_H
#define _CHESSGAME_H

#include "ChessPosition.h"
#include "ChessMove.h"

#define CHESSGAME_MAX_MOVES 200

class ChessGame {
  //
  // Constructors and destructors
  //
public:
  // Default constructor (initial game setup, move 1)
  ChessGame();

  // Read PGN data
  ChessGame( const char *pgn );

  // Start from a position
  ChessGame( const ChessPosition & );

  //
  // Accessors
  //
  const ChessPosition &current_position() const;

  //
  // Mutators
  //
  void make_move( const class ChessMove & );

  //
  // Protected data
  //
protected:
  ChessPosition position;
  class ChessMove *moves[ CHESSGAME_MAX_MOVES ];
};

#endif /* !_CHESSGAME_H */
