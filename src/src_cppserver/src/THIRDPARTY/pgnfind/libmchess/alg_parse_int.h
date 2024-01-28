// -*- C++ -*-
#ifndef _ALG_PARSE_H
#define _ALG_PARSE_H

#include "ChessPiece.h"
#include "ChessPosition.h"

// Structures for passing data within and out of the yacc parser

typedef struct  {
  int rank;
  int file;
} Sclarifier;

typedef struct {
  int rank;
  int file;
} Ssquare;

typedef struct {
  /* Type of piece being moved */
  ChessPiece::Type piece;
  /* rank or file to disambiguate a move */
  struct Sclarifier clarifier;
  /* destination square */
  struct Ssquare square;
  /* was a piece captured? */
  int capture;
  /* was the move with check? */
  int check;
  /* For pawn moves, was it promoted to another piece? */
  ChessPiece::Type promote;
  /* For king moves, was it a castling move? */
  ChessPosition::Castling castling;
} Schessmove;

struct Schessmove *do_alg_parse( const char *_move_str );

#endif /* !_ALG_PARSE_H */
