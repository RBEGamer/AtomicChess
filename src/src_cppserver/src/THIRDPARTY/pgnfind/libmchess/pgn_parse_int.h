#ifndef _PGN_PARSE_H
#define _PGN_PARSE_H

#include "ChessGame.h"

extern ChessGame *pgn_game; // For pgn_lex

int pgn_error( char * );
int do_pgn_parse( const char *_pgn_data, ChessGame *_game );

#endif /* !_PGN_PARSE_H */
