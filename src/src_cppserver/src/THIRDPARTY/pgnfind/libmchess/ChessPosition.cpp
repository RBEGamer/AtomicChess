#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else
#define assert(x)
#endif

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr (), *strrchr ();
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif /* !STDC_HEADERS */

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <ctype.h>

#ifndef HAVE_SNPRINTF
#include "snprintf.h"
#endif

#include "ChessPosition.h"

// A FEN for the position at the start of a game
static const char *start_FEN =
"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

ChessPosition::ChessPosition() {
  read_FEN( start_FEN );
}

ChessPosition::ChessPosition( const char *FEN ) {
  read_FEN( FEN );
}

const ChessPiece &ChessPosition::get_piece_at( int x, int y ) const {
  assert( x >= 1 && x <= 8);
  assert( y >= 1 && y <= 8);
  return board[x - 1][y - 1];
}

const ChessPiece &ChessPosition::get_piece_at( const char *square ) const {
  return get_piece_at( square_x( square ), square_y( square ) );
}

ChessPosition::Color ChessPosition::get_active_color() const {
  return active_color;
}

ChessPosition::Castling ChessPosition::get_castling( Color color ) const {
  switch ( color ) {
  case White:
    return white_castling;
  case Black:
    return black_castling;
  default:
    assert(0);
  }
}

int ChessPosition::get_en_passant_x() const {
  return en_passant_x;
}

int ChessPosition::get_en_passant_y() const {
  return en_passant_y;
}

int ChessPosition::get_halfmove_clock() const {
  return halfmove_clock;
}

int ChessPosition::get_fullmove_number() const {
  return fullmove_number;
}

void ChessPosition::write_FEN( char *buf ) const {
  int x, y;
  int empty_ctr = 0;
  int any_castling = 0;
  char num[4];

  //
  // Piece placement data
  //
  for ( y = 7 ; y >= 0 ; --y ) {
    for ( x = 0 ; x < 8 ; ++x ) {
      if ( board[x][y].get_type() == ChessPiece::Empty ) {
	++empty_ctr;
      } else {
	if ( empty_ctr != 0) { // End of a series of empty squares
	  snprintf( buf, 2, "%d", empty_ctr );
	  empty_ctr = 0;
	  ++buf;
	}
	*buf = board[x][y].piece_char();
	++buf;
      }
    }

    if ( empty_ctr != 0 ) { // Empty rank
      snprintf( buf, 2, "%d", empty_ctr );
      empty_ctr = 0;
      ++buf;
    }

    if (y > 0)
      *(buf++) = '/';
  }

  *(buf++) = ' '; // Field separator

  //
  // Active color
  //
  if ( active_color == White ) {
    *(buf++) = 'w';
  } else if ( active_color == Black ) {
    *(buf++) = 'b';
  } else { // Should never happen
    *(buf++) = 'x';
  }

  *(buf++) = ' '; // Field separator

  //
  // Castling availability
  //
  if ( white_castling == Both || white_castling == Kingside ) {
    any_castling = 1;
    *(buf++) = 'K';
  }

  if ( white_castling == Both || white_castling == Queenside ) {
    any_castling = 1;
    *(buf++) = 'Q';
  }

  if ( black_castling == Both || black_castling == Kingside ) {
    any_castling = 1;
    *(buf++) = 'k';
  }

  if ( black_castling == Both || black_castling == Queenside ) {
    any_castling = 1;
    *(buf++) = 'q';
  }

  if (!any_castling) {
    *(buf++) = '-';
  }

  *(buf++) = ' '; // Field separator

  //
  // En passant target square
  //
  if ( en_passant_x != 0 
       && en_passant_y != 0 ) {
    *(buf++) = 'a' + en_passant_x - 1;
    *(buf++) = '1' + en_passant_y - 1;
  } else {
    *(buf++) = '-';
  }

  *(buf++) = ' '; // Field separator

  //
  // Halfmove clock
  //
  snprintf( num, 4, "%d", halfmove_clock );
  strncpy( buf, num, strlen(num) );
  buf += strlen( num );

  *(buf++) = ' '; // Field separator

  //
  // Fullmove number
  //
  snprintf( num, 4, "%d", fullmove_number );
  strncpy( buf, num, strlen(num) );
  buf += strlen( num );

  *(buf++) = '\0';
}

void ChessPosition::set_piece_at( int x, int y, const ChessPiece &piece ) {
  assert(x >= 1 && x <= 8);
  assert(y >= 1 && y <= 8);
  board[x - 1][y - 1] = piece;
}

void ChessPosition::set_piece_at( const char *square,
				  const ChessPiece &piece ) {
  set_piece_at( square_x( square ), square_y( square ), piece );
}

void ChessPosition::set_active_color( Color color ) {
  active_color = color;
}

void ChessPosition::set_castling( ChessPosition::Color color,
				  ChessPosition::Castling castling ) {
  assert( color == White || color == Black );
  switch ( color ) {
  case White:
    white_castling = castling;
  case Black:
    black_castling = castling;
  }
}

void ChessPosition::set_en_passant( int x, int y ) {
  en_passant_x = x;
  en_passant_y = y;
}

void ChessPosition::set_halfmove_clock( int value ) {
  halfmove_clock = value;
}

void ChessPosition::set_fullmove_number( int value ) {
  fullmove_number = value;
}

int ChessPosition::read_FEN( const char *_buf ) {
  int x = 0, y = 7;
  char mybuf[1024];
  char *current = mybuf;
  char *ptr;

  strncpy( mybuf, _buf, 1024 );
  mybuf[1023] = '\0';

  while ( y >= 0 ) {
    x = 0;
    while ( x < 8 ) {
      if ( isdigit( *current ) ) {
	int count = *current -  '1' + 1;
	x += count;
	++current;
	//cout << "Skipping " << count << " squares" << endl;
      } else {
	board[x][y].set( *current );
	//cout << "Got piece (" << *current << "): "
	//     << board[x][y].get_type() << ','
	//     << board[x][y].get_color() << endl;
	++current;
	++x;
      }
    }

    if ( y > 0 ) {
      if ( *current != '/' ) { // Bad FEN
	cerr << "ChessPosition::read_FEN: "
	     << "Didn't find slash at end of rank (saw " << *current << ")"
	     << ": bailing" << endl;
	return 0;
      }

      ++current;
    }
    --y;
  }

  if ( *current != ' ' ) { // Check field separator
    cerr << "ChessPosition::read_FEN: "
	 << "Missing field separator (saw " << *current << ')' << endl;
    return 0;
  }
  ++current;

  if ( *current == 'w' ) {
    set_active_color( White );
  } else if ( *current == 'b' ) {
    set_active_color( Black );
  } else {
    cout << "ChessPosition::read_FEN: Couldn't determine whose move it was"
	 << endl;
    return 0;
  }
  ++current;

  if ( *current != ' ' ) { // Check field separator
    cerr << "ChessPosition::read_FEN: "
	 << "Missing field separator (saw " << *current << ')' << endl;
    return 0;
  }
  ++current;

  // Null-terminate castling info for easy scanning
  ptr = strchr( current, ' ' );
  if ( ptr == NULL ) {
    cerr << "ChessPosition::read_FEN: no field sep after castling info"
	 << endl;
    return 0;
  }
  *ptr = '\0';

  set_castling( White, None );
  if ( strchr( current, 'K' ) ) {
    set_castling( White, Kingside );
  }
  if ( strchr( current, 'Q' ) ) {
    set_castling( White, (Castling)( get_castling( White ) | Queenside ) );
  }

  set_castling( Black, None );
  if ( strchr( current, 'k' ) ) {
    set_castling( Black,  Kingside );
  }
  if ( strchr( current, 'q' ) ) {
    set_castling( Black, (Castling)( get_castling( Black ) | Queenside ) );
  }

  // Advance past null-terminated section to start of next field
  current = ptr + 1;

  ptr = strchr( current, ' ' );
  if ( ptr == NULL ) {
    cerr << "ChessPosition::read_FEN: no field sep after en passant"
	 << endl;
    return 0;
  }
  *ptr = '\0';

  if ( !strcmp( current, "-" ) ) { // en passant
    set_en_passant( 0, 0 );
  } else if ( strlen(current) == 2 ) {
    
    set_en_passant( *(current) - 'a' + 1,
		    *(current + 1) - '1' + 1 );
  } else {
    cerr << "ChessPosition::read_FEN: invalid en passant square" << endl;
    return 0;
  }

  // Advance past null-terminated section to start of next field
  current = ptr + 1;

  ptr = strchr( current, ' ' );
  if ( ptr == NULL ) {
    cerr << "ChessPosition::read_FEN: no field sep after halfmove clock"
	 << endl;
    return 0;
  }
  *ptr = '\0';

  set_halfmove_clock( atoi( current ) );

  // Advance past null-terminated section to start of next field
  current = ptr + 1;

  set_fullmove_number( atoi( current ) );

  return 1;
}

int ChessPosition::square_x( const char *square ) {
  char file;
  assert( strlen(square) == 2 );

  file = tolower(square[0]);
  assert( file >= 'a' && file <= 'h');
  return file - 'a' + 1;
}

int ChessPosition::square_y( const char *square ) {
  char rank;
  assert( strlen(square) == 2 );

  rank = tolower(square[1]);
  assert( rank >= '1' && rank <= '8' );
  return rank - '1' + 1;
}

void ChessPosition::increment_move() {
  switch ( active_color ) {
  case White:
    active_color = Black;
    break;
  case Black:
    ++fullmove_number;
    active_color = White;
    break;
  default:
    assert(0);
  }
}

int ChessPosition::find_piece( ChessPiece piece,
			       int start_x, int start_y,
			       int file,
			       int rank,
			       const ChessVector *vectors,
			       unsigned int numvec,
			       int unique,
			       int *found_x,
			       int *found_y ) const {
  int x, y;
  unsigned int vec, len;
  int found = 0;
  for ( vec = 0 ; vec < numvec ; ++vec ) {
    for ( x = start_x + vectors[ vec ].x,
	    y = start_y + vectors[ vec ].y,
	    len = 1 ;

	  x >= 1 && x <= 8 && y >= 1 && y <= 8 // Stay on the board
	    && len <= vectors[ vec ].len ; // check magnitude

	  x += vectors[ vec ].x,           // Move in the direction of
	    y += vectors[ vec ].y,
	    ++len ) {      // the current vector

      // It's rather inefficient to check the rank/file this way, but
      // they're relatively rare in practice (mostly for a clarifier
      // in PGN notation)

      if ( file && file != x )
	continue;
      if ( rank && rank != y )
	continue;

//        cout << "Looking for a piece on (" << x << ',' << y << ')'
//  	   << endl;

      ChessPiece found_piece = get_piece_at( x, y );
      if ( found_piece == piece ) {

	if ( found_x != NULL )
	  *found_x = x;

	if ( found_y != NULL )
	  *found_y = y;

	if ( unique ) {

	  if ( found ) // Ambiguous
	    return 0;
	  else
	    found = 1;

	} else {

	  return 1;
	      
	}
	    
	break;
      }
    }
  }

  return found;
}

int ChessPosition::is_attacked( const char *square, Color color ) const {
  return is_attacked( square_x( square ), square_y( square ), color );
}

int ChessPosition::is_attacked( int x, int y, Color color ) const {
  ChessPiece::Color piece_color = ( color == White ) ?
    ChessPiece::White : ChessPiece::Black;

  // Start with piece attacks
  int attacked = ( find_piece( ChessPiece( ChessPiece::Bishop, piece_color ),
			       x, y, 0, 0,
			       bishop_vectors, num_bishop_vectors,
			       0, NULL, NULL) ||
		   find_piece( ChessPiece( ChessPiece::Knight, piece_color ),
			       x, y, 0, 0,
			       knight_vectors, num_knight_vectors,
			       0, NULL, NULL ) ||
		   find_piece( ChessPiece( ChessPiece::Rook, piece_color ),
			       x, y, 0, 0,
			       rook_vectors, num_rook_vectors,
			       0, NULL, NULL ) ||
		   find_piece( ChessPiece( ChessPiece::Queen, piece_color ),
			       x, y, 0, 0,
			       queen_vectors, num_queen_vectors,
			       0, NULL, NULL ) ||
		   find_piece( ChessPiece( ChessPiece::King, piece_color ),
			       x, y, 9, 9,
			       king_vectors, num_king_vectors,
			       0, NULL, NULL ) );

  if ( !attacked )
    // Try pawn attacks
    if ( color == White )
      attacked = find_piece( ChessPiece( ChessPiece::Pawn, piece_color ),
			     x, y, 0, 0,
			     white_pawn_vectors, num_white_pawn_vectors,
			     0, NULL, NULL );
    else
      attacked = find_piece( ChessPiece( ChessPiece::Pawn, piece_color ),
			     x, y, 0, 0,
			     black_pawn_vectors, num_black_pawn_vectors,
			     0, NULL, NULL );

  return attacked;
}

int ChessPosition::in_check( Color color ) const {
  ChessPiece::Color piece_color = ( color == White ) ?
    ChessPiece::White : ChessPiece::Black;
  Color opponent_color = ( color == Black ) ?
    White : Black;

  //
  // Find the king
  //
  int king_x, king_y;
  int found = 0;

  for( king_x = 1 ; king_x <= 8 && !found ; ++king_x )
    for( king_y = 1 ; king_y <= 8 && !found ; ++king_y )
      if ( get_piece_at( king_x, king_y ) ==
	   ChessPiece( ChessPiece::King, piece_color ) )
	found = 1;

  if ( !found ) // No king!
    throw InvalidPosition;

  return is_attacked( king_x, king_y, opponent_color );
}
