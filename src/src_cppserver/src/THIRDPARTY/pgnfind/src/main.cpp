#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include "ChessMove.h"
#include "pgn_parse_int.h"

int main(int argc, char *argv[]) {
  char buf[1024] = "";
  char *ptr = buf;

  int len = 0;
  while ( read(fileno(stdin), ptr, 1024 - len ) > 0 ) {
    len = strlen( buf );
    ptr = buf + len;
  }

  try {
    ChessGame game( buf );
    
    game.current_position().write_FEN( buf );
    cout << buf << endl;
  }
  
  catch (ChessMove::Exception cme) {
    if ( cme == ChessMove::InvalidMove ) {
      cout << "Invalid move" << endl;
    } else if ( cme == ChessMove::IllegalMove ) {
      cout << "Illegal move" << endl;
    } else {
      cout << "Unrecognized exception" << endl;
    }

    return 1;
  }

  return 0;
}
