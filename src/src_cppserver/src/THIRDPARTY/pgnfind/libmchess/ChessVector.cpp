#include "ChessVector.h"

// For searching...order is important due to the pointers declared
// below
const struct ChessVector piece_vectors[] = {
  // Bishops (4)
  { 1, 1, 8 },
  { -1, 1, 8 },
  { 1, -1, 8 },
  { -1, -1, 8 },
  // Rooks (4)
  { 1, 0, 8 },
  { -1, 0, 8 },
  { 0, 1, 8 },
  { 0, -1, 8 },
  // Knights (8)
  { 1, 2, 1 },
  { -1, 2, 1 },
  { 1, -2, 1 },
  { -1, -2, 1 },
  { 2, 1, 1 },
  { -2, 1, 1 },
  { 2, -1, 1 },
  { -2, -1, 1 },
  // Kings (8)
  { 0, 1, 1 },
  { 0, -1, 1 },
  { 1, 0, 1 },
  { -1, 0, 1 },
  { 1, 1, 1 },
  { -1, 1, 1 },
  { 1, -1, 1 },
  { -1, -1, 1 },
  // White pawns (2)
  { 1, 1, 1 },
  { -1, 1, 1 },
  // Black pawns (2)
  { 1, -1, 1 },
  { -1, -1,1 , } };

const struct ChessVector *queen_vectors = &piece_vectors[0];
const int num_queen_vectors = 8;

const struct ChessVector *bishop_vectors = &piece_vectors[0];
const int num_bishop_vectors = 4;

const struct ChessVector *rook_vectors = &piece_vectors[4];
const int num_rook_vectors = 4;

const struct ChessVector *knight_vectors = &piece_vectors[8];
const int num_knight_vectors = 4;

const struct ChessVector *king_vectors = &piece_vectors[16];
const int num_king_vectors = 8;

const struct ChessVector *white_pawn_vectors = &piece_vectors[24];
const int num_white_pawn_vectors = 2;

const struct ChessVector *black_pawn_vectors = &piece_vectors[26];
const int num_black_pawn_vectors = 2;
