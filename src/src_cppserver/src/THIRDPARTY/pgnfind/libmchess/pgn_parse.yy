%{

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef STDC_HEADERS
#include <string.h>
#endif

#include "ChessGame.h"
#include "alg_parse_int.h"
#include "pgn_lex_int.h"

// Maximum length of a single game in PGN
#define PGN_MAX_LENGTH 10240

// Localify yacc stuff

#undef yymaxdepth
#undef yyparse
#undef yylex
#undef yyerror
#undef yylval
#undef yychar
#undef yydebug
#undef yypact
#undef yyr1
#undef yyr2
#undef yydef
#undef yychk
#undef yypgo
#undef yyact
#undef yyexca
#undef yyerrflag
#undef yynerrs
#undef yyps
#undef yypv
#undef yys
#undef yy_yys
#undef yystate
#undef yytmp
#undef yyv
#undef yy_yyv
#undef yyval
#undef yylloc
#undef yyreds
#undef yytoks
#undef yylhs
#undef yylen
#undef yydefred
#undef yydgoto
#undef yysindex
#undef yyrindex
#undef yygindex
#undef yytable
#undef yycheck
#undef yyname
#undef yyrule
#define	yymaxdepth pgn_maxdepth
#define	yyparse	pgn_parse
#define	yylex	pgn_lex
#define	yyerror	pgn_error
#define	yylval	pgn_lval
#define	yychar	pgn_char
#define	yydebug	pgn_debug
#define	yypact	pgn_pact
#define	yyr1	pgn_r1
#define	yyr2	pgn_r2
#define	yydef	pgn_def
#define	yychk	pgn_chk
#define	yypgo	pgn_pgo
#define	yyact	pgn_act
#define	yyexca	pgn_exca
#define yyerrflag pgn_errflag
#define yynerrs	pgn_nerrs
#define	yyps	pgn_ps
#define	yypv	pgn_pv
#define	yys	pgn_s
#define	yy_yys	pgn_yys
#define	yystate	pgn_state
#define	yytmp	pgn_tmp
#define	yyv	pgn_v
#define	yy_yyv	pgn_yyv
#define	yyval	pgn_val
#define	yylloc	pgn_lloc
#define yyreds	pgn_reds
#define yytoks	pgn_toks
#define yylhs	pgn_yylhs
#define yylen	pgn_yylen
#define yydefred pgn_yydefred
#define yydgoto	pgn_yydgoto
#define yysindex pgn_yysindex
#define yyrindex pgn_yyrindex
#define yygindex pgn_yygindex
#define yytable	 pgn_yytable
#define yycheck	 pgn_yycheck
#define yyname   pgn_yyname
#define yyrule   pgn_yyrule

#define YYERROR_VERBOSE
#define YYDEBUG 1

  static char *err_msg;
  ChessGame *pgn_game; // for pgn_lex

  int yyerror(char *);


%}

%union {
  int num;
  ChessMove *move;
}

%token <num> NUM
%token <move> MOVE

%type <game> moveseq
%type <move> move

%%

   input: dangling_halfmove			{ }
	| moveseq				{ }
	| moveseq dangling_halfmove		{ }
	;

   moveseq: NUM '.' move move			{ }
	| moveseq NUM '.' move move		{ }
	;

   dangling_halfmove: NUM '.' move		{ }
	;

   move: MOVE				{ pgn_game->make_move( *$1 ); }
	;

%%

int yyerror( char *s ) {
  err_msg = s;
  return 1;
}

int do_pgn_parse( const char *pgn_data, ChessGame *_game ) {
  pgn_game = _game;
  //yydebug = 1;

  // Feed the pgn string into the lexer
  init_pgn_lex( pgn_data );

  return yyparse();
}

