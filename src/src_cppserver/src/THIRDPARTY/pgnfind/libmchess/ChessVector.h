#ifndef _CHESSVECTOR_H
#define _CHESSVECTOR_H

struct ChessVector {
  int x;
  int y;
  unsigned int len;
};

extern const struct ChessVector piece_vectors[];
extern const struct ChessVector *queen_vectors;
extern const int num_queen_vectors;

extern const struct ChessVector *bishop_vectors;
extern const int num_bishop_vectors;

extern const struct ChessVector *rook_vectors;
extern const int num_rook_vectors;

extern const struct ChessVector *knight_vectors;
extern const int num_knight_vectors;

extern const struct ChessVector *king_vectors;
extern const int num_king_vectors;

extern const struct ChessVector *white_pawn_vectors;
extern const int num_white_pawn_vectors;

extern const struct ChessVector *black_pawn_vectors;
extern const int num_black_pawn_vectors;

#endif /* !_CHESSVECTOR_H */
