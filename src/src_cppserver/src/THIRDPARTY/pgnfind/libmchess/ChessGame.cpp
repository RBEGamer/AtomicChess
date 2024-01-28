#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else
#define assert(x)
#endif

#include "pgn_parse_int.h"
#include "ChessGame.h"

ChessGame::ChessGame() {
  int i;
  for( i = 0 ; i < CHESSGAME_MAX_MOVES ; ++i ) {
    moves[i] = NULL;
  }
}

ChessGame::ChessGame( const char *pgn ) {
  ChessGame();
  try {
    do_pgn_parse( pgn, this );
  }
  catch ( ChessMove::Exception cme ) {
    throw cme;
  }
}

ChessGame::ChessGame( const ChessPosition &pos ):
  position(pos) {
  ChessGame();
}

const ChessPosition &ChessGame::current_position() const {
  return position;
}

void ChessGame::make_move( const ChessMove &move ) {
  int start_x = move.get_start_x();
  int start_y = move.get_start_y();
  int end_x = move.get_end_x();
  int end_y = move.get_end_y();

  ChessPiece piece( position.get_piece_at( start_x,
					   start_y ) );

  // We check this when we create a new move
  assert( piece.get_type() != ChessPiece::Empty );

  // Handle normal moves
  position.set_piece_at( start_x,
			 start_y,
			 ChessPiece( ChessPiece::Empty ) );
  position.set_piece_at( end_x,
			 end_y,
			 piece );

  // Handle en passant captures (capture the pawn)
  if ( piece.get_type() == ChessPiece::Pawn
       && end_x == position.get_en_passant_x()
       && end_y == position.get_en_passant_y() ) {
    int dir = (position.get_active_color()
	       == ChessPosition::White) ? 1 : -1;
    position.set_piece_at( end_x, end_y - dir,
			   ChessPiece( ChessPiece::Empty ) );
  }
			   
  // Handle castling (move the rook)
  if ( piece.get_type() == ChessPiece::King ) {
    int old_rook_square, new_rook_square;

    switch ( move.get_castling_move() ) {
    case ChessPosition::Kingside:
      old_rook_square = 8;
      new_rook_square = 6;
      break;
    case ChessPosition::Queenside:
      old_rook_square = 1;
      new_rook_square = 4;
      break;
    default:
      old_rook_square = new_rook_square = 0;
    }

    if ( old_rook_square ) {
      position.set_piece_at( new_rook_square, end_y,
			     position.get_piece_at( old_rook_square, end_y ) );
      position.set_piece_at( old_rook_square, end_y,
			     ChessPiece( ChessPiece::Empty ) );
    }
  }

  // Handle pawn promotion (change the recently placed pawn into
  // another piece)
  if ( piece.get_type() == ChessPiece::Pawn
       && move.get_promote() != ChessPiece::Empty )
    position.set_piece_at( end_x, end_y,
			   ChessPiece( move.get_promote(),
				       piece.get_color() ) );

  // Set new en passant, castling info
  position.set_en_passant( move.get_en_passant_x(),
			   move.get_en_passant_y() );
  position.set_castling( position.get_active_color(),
			 move.get_castling() );

  // Set halfmove clock
  position.set_halfmove_clock( move.get_halfmove_clock() );

  // Increment fullmove clock if appropriate
  position.increment_move();
}
