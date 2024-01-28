#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else
#define assert(x)
#endif

#include <iostream.h>

#include "ChessMove.h"
#include "ChessVector.h"

ChessMove::ChessMove( const char *data, ChessMove::MoveFormat format,
		      const ChessGame *game ) {
  assert( format == Algebraic || format == Descriptive );

  assert( game != NULL );

  ChessPosition::Color pos_color =
    game->current_position().get_active_color();

  // Initialize the new state variables
  en_passant_x = 0;
  en_passant_y = 0;
  castling = game->current_position().get_castling( pos_color );
  halfmove_clock = game->current_position().get_halfmove_clock() + 1;

  promote = ChessPiece::Empty;
						  
  //
  // Parse a move based on the specified format
  //

  if ( format == Algebraic ) {

    //
    // Algebraic notation
    //

    // Inexpensive sanity check before calling the parser
    if ( strlen( data ) < 2 )
      throw InvalidMove;
      
    //
    // Call the algebraic notation parser
    //

    struct Schessmove *move = do_alg_parse( data );

    if (!move)
      throw InvalidMove;

//      cout << "Parsed move:" << endl
//  	 << "piece: " << (int)move->piece << endl
//  	 << "clarifier.rank: " << move->clarifier.rank << endl
//  	 << "clarifier.file: " << move->clarifier.file << endl
//  	 << "square.rank: " << move->square.rank << endl
//  	 << "square.file: " << move->square.file << endl
//  	 << "capture: " << move->capture << endl
//  	 << "check: " << move->check << endl
//  	 << "promote: " << (int)move->promote << endl
//  	 << "castling: " << (int)move->castling << endl
//  	 << endl
//  	 << "Deduced: " << endl;
      
    ChessPiece::Type piece_type = move->piece;
    ChessPiece piece;

    // Make sure we got a valid piece

    switch ( piece_type ) {
    case ChessPiece::Pawn:
    case ChessPiece::Bishop:
    case ChessPiece::Knight:
    case ChessPiece::Rook:
    case ChessPiece::Queen:
    case ChessPiece::King:
      break;
    default:
      throw InvalidMove;
    }
    
    //
    // Sanity checks...trust no one...not even the parser.
    //

    // Check coordinate ranges
    if ( !move->castling )
      if ( move->square.file >= 1 && move->square.file <= 8  )
	end_x = move->square.file;
      else 
	throw InvalidMove;

    if ( !move->castling )
      if ( move->square.rank >= 1 && move->square.rank <= 8 )
	end_y = move->square.rank;
      else 
	throw InvalidMove;

    // Check the clarifier...a problem here would be indicative of a
    // programmatic error in the parser
    assert( !( move->clarifier.rank && move->clarifier.file ) );

    // Check active color
    assert( pos_color == ChessPosition::White
	    || pos_color == ChessPosition::Black );

    //
    // Determine the color of the piece being moved
    //

    ChessPiece::Color piece_color =
      ( pos_color == ChessPosition::White )
      ? ChessPiece::White
      : ChessPiece::Black;

    // Make sure we're not trying to capture our own piece, or make a
    // null move

    if ( !move->castling ) {
      piece = game->current_position().get_piece_at( end_x, end_y );
      if ( piece.get_type() != ChessPiece::Empty &&
	   piece.get_color() == piece_color )
	throw IllegalMove;
    }

    // Reset the halfmove clock for captures
    if ( move->capture )
      halfmove_clock = 0;

    //
    // Determine start square
    //

    int found = 0;
    switch ( piece_type ) {
      //
      // Pawn moves
      //
    case ChessPiece::Pawn:
      int dir, eighth_rank, second_rank;

      // Reset the halfmove clock
      halfmove_clock = 0;

      if ( pos_color == ChessPosition::White ) {
	dir = 1;
	second_rank = 2;
      } else {
	dir = -1;
	second_rank = 7;
      }

      eighth_rank = second_rank + ( 6 * dir );

      if ( end_y == ( second_rank ) || end_y == (second_rank - dir) )
	throw InvalidMove;

      if ( move->capture ) {
	//
	// Pawn captures
	//

	start_x = move->clarifier.file;
	start_y = end_y - dir;

	if ( !( start_x >= 1 && start_x <= 8 ) ) {
	  throw InvalidMove;
	}

      } else {
	//
	// Pawn moves without capture
	//

	start_x = end_x;
	
	if ( ( end_y == second_rank + ( dir * 2 ) )
	     && ( game->current_position().get_piece_at( end_x,
							 second_rank + dir )
		  .get_type() == ChessPiece::Empty )
	     && ( game->current_position().get_piece_at( end_x, second_rank )
		  == ChessPiece( ChessPiece::Pawn, piece_color ) ) ) {
	  //
	  // Initial two-square advance
	  //

	  start_y = second_rank;
	  en_passant_x = start_x;
	  en_passant_y = second_rank + dir;

	} else if ( game->current_position().
		    get_piece_at( end_x, end_y - dir )
		    == ChessPiece( ChessPiece::Pawn, piece_color ) ) {
	  //
	  // Normal pawn move
	  //

	  start_y = end_y - dir;

	} else {
	  throw InvalidMove;
	}
      }
      
      if ( game->current_position().get_piece_at( start_x,
						   start_y ).get_type()
	     != ChessPiece::Pawn ) {
	throw InvalidMove;
      }

      if ( end_y == eighth_rank )
	if ( move->promote == ChessPiece::Empty )
	  throw InvalidMove;
	else
	  promote = move->promote;

      break;

    case ChessPiece::Knight:
      if ( ! game->current_position().
	   find_piece( ChessPiece( ChessPiece::Knight, piece_color ),
		       end_x,
		       end_y,
		       move->clarifier.file,
		       move->clarifier.rank,
		       knight_vectors, num_knight_vectors,
		       1,
		       &start_x,
		       &start_y ) ) {

	  throw InvalidMove;
      }
					    
      break;

    case ChessPiece::Bishop:
      if ( ! game->current_position().
	   find_piece( ChessPiece( ChessPiece::Bishop, piece_color ),
		       end_x,
		       end_y,
		       move->clarifier.file,
		       move->clarifier.rank,
		       bishop_vectors, num_bishop_vectors,
		       1,
		       &start_x,
		       &start_y ) ) {
	
	throw InvalidMove;
      }
      
      break;

    case ChessPiece::Rook:
      if ( ! game->current_position().
	   find_piece( ChessPiece( ChessPiece::Rook, piece_color ),
		       end_x,
		       end_y,
		       move->clarifier.file,
		       move->clarifier.rank,
		       rook_vectors, num_rook_vectors,
		       1,
		       &start_x,
		       &start_y ) ) {

	throw InvalidMove;

      }
      
      // Disallow castling, where appropriate
      if ( start_x == 1 ) {
	castling = (ChessPosition::Castling)
	  ( castling & ~(ChessPosition::Queenside) );
      } else if ( start_x == 8 ) {
	castling = (ChessPosition::Castling)
	  ( castling & ~(ChessPosition::Kingside) );
      }
	
      break;

    case ChessPiece::Queen:
      if ( ! game->current_position().
	   find_piece( ChessPiece( ChessPiece::Queen, piece_color ),
		       end_x,
		       end_y,
		       move->clarifier.file,
		       move->clarifier.rank,
		       queen_vectors, num_queen_vectors,
		       1,
		       &start_x,
		       &start_y ) ) {
	
	throw InvalidMove;

      }
      
      break;

    case ChessPiece::King:
      // Assume we're not castling (until we find otherwise)
      castling_move = ChessPosition::None;

      // She definitely won't be able to castle after this move
      castling = ChessPosition::None;

      if ( move->castling ) {
	int back_rank = ( pos_color == ChessPosition::White ? 1 : 8 );
	struct {
	  int x;
	  int y;
	} king_square = { 5, back_rank },
		  rook_square = { 0, back_rank },
		  int_squares[2] = { { 0, back_rank },
				     { 0, back_rank } };

        // int_squares[1] is where the king will go

	switch ( move->castling ) {

	case ChessPosition::Kingside:
	  int_squares[0].x = 6;
	  int_squares[1].x = 7;
	  rook_square.x = 8;break;

	case ChessPosition::Queenside:
	  int_squares[0].x = 4;
	  int_squares[1].x = 3;
	  rook_square.x = 1;break;

	default:
	  assert(0);
	}

//  	cout << "Castling: " << endl
//  	     << "king_square.x: " << king_square.x << endl
//  	     << "king_square.y: " << king_square.y << endl
//  	     << "rook_square.x: " << rook_square.x << endl
//  	     << "rook_square.y: " << rook_square.y << endl
//  	     << "int_squares[0].x" << int_squares[0].x << endl
//  	     << "int_squares[0].y" << int_squares[0].y << endl
//  	     << "int_squares[1].x" << int_squares[1].x << endl
//  	     << "int_squares[1].y" << int_squares[1].y << endl
//  	     << endl;
	  
	
	if ( game->current_position().get_piece_at( king_square.x,
						  king_square.y )
	     .get_type()
	     == ChessPiece::King
	     && game->current_position().get_piece_at( rook_square.x,
						     rook_square.y )
	     .get_type()
	     == ChessPiece::Rook
	     && game->current_position().get_piece_at( int_squares[0].x,
						     int_squares[0].y )
	     .get_type()
	     == ChessPiece::Empty
	     && game->current_position().get_piece_at( int_squares[1].x,
						     int_squares[1].y )
	     .get_type()
	     == ChessPiece::Empty ) {
	  start_x = king_square.x;
	  start_y = king_square.y;
	  end_x = int_squares[1].x;
	  end_y = int_squares[1].y;
	  
	  // This is a castling move
	  castling_move = move->castling;

	} else {
	  throw IllegalMove;
	}

      } else {
	struct {
	  int x;
	  int y;
	} starts[8] = { { end_x, end_y - 1 },
			{ end_x, end_y + 1 },
			{ end_x + 1, end_y },
			{ end_x + 1, end_y + 1 },
			{ end_x + 1, end_y - 1 },
			{ end_x - 1, end_y + 1 },
			{ end_x - 1, end_y - 1 },
			{ end_x - 1, end_y } };

	// Look for a king on each of them
	for ( int i = 0 ; i < 8 ; ++i ) {
	  if (starts[i].x >= 1 && starts[i].x <= 8 && // is on the board
	      starts[i].y >= 1 && starts[i].y <= 8 &&
	      game->current_position().get_piece_at( starts[i].x,
						     starts[i].y )
	      == ChessPiece( ChessPiece::King, piece_color ) ) {

	    start_x = starts[i].x;
	    start_y = starts[i].y;
	    found = 1;
	    break;
	  }
	}

	if (!found)
	  throw InvalidMove;
      }
      
      break;
    default:
      // Should never happen, since we checked this case above
      assert(0);
    }
    
//      cout << "start_x: " << start_x << endl;
//      cout << "start_y: " << start_y << endl;

  } else { // Descriptive notation
    // This isn't here yet
    throw InvalidMove;
  }
}
