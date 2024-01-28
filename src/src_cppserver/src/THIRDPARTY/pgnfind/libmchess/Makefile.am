noinst_LIBRARIES = libmchess.a

libmchess_a_SOURCES = ChessPiece.cpp ChessPiece.h \
			ChessPosition.cpp ChessPosition.h \
			alg_parse.yy alg_parse_int.h \
			ChessMove.cpp ChessMove.h \
			ChessGame.cpp ChessGame.h \
			pgn_parse.yy pgn_parse_int.h \
			pgn_lex.ll pgn_lex_int.h \
			ChessVector.cpp ChessVector.h
libmchess_a_LIBADD = @LIBOBJS@

EXTRA_DIST = pgn_parse.h alg_parse.h
