%{

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef STDC_HEADERS
#include <string.h>
#endif

#include "ChessPiece.h"
#include "ChessMove.h"

// Our stuff...alg_parse.h is yacc's stuff (which we get for free)
#include "alg_parse_int.h"

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
#define	yymaxdepth alg_maxdepth
#define	yyparse	alg_parse
#define	yylex	alg_lex
#define	yyerror	alg_error
#define	yylval	alg_lval
#define	yychar	alg_char
#define	yydebug	alg_debug
#define	yypact	alg_pact
#define	yyr1	alg_r1
#define	yyr2	alg_r2
#define	yydef	alg_def
#define	yychk	alg_chk
#define	yypgo	alg_pgo
#define	yyact	alg_act
#define	yyexca	alg_exca
#define yyerrflag alg_errflag
#define yynerrs	alg_nerrs
#define	yyps	alg_ps
#define	yypv	alg_pv
#define	yys	alg_s
#define	yy_yys	alg_yys
#define	yystate	alg_state
#define	yytmp	alg_tmp
#define	yyv	alg_v
#define	yy_yyv	alg_yyv
#define	yyval	alg_val
#define	yylloc	alg_lloc
#define yyreds	alg_reds
#define yytoks	alg_toks
#define yylhs	alg_yylhs
#define yylen	alg_yylen
#define yydefred alg_yydefred
#define yydgoto	alg_yydgoto
#define yysindex alg_yysindex
#define yyrindex alg_yyrindex
#define yygindex alg_yygindex
#define yytable	 alg_yytable
#define yycheck	 alg_yycheck
#define yyname   alg_yyname
#define yyrule   alg_yyrule

#define YYERROR_VERBOSE
#define YYDEBUG 1

#define MAX_MOVE_LEN 16
  
  static int move_index;
  static char move_str[MAX_MOVE_LEN + 1] = "";
  static struct Schessmove move_data;
  static char *err_msg;

  static int yylex();
  static int yyerror(char *);


%}

%union {
  char ch;
  int num;
  ChessPiece::Type piece;
  struct Sclarifier clarifier;
  struct Ssquare square;
  struct Schessmove move;
}

%token <num> INT

%type <num> rank file maybe_capture
%type <piece> piece maybe_promote
%type <square> square
%type <move> input init move_check move

%%

   input: init move_check { move_data = $1; move_data = $2; }
	;

   init: /* empty */	{ $$.piece = ChessPiece::Empty;
			  $$.clarifier.rank = 0;
			  $$.clarifier.file = 0;
			  $$.square.rank = 0;
			  $$.square.file = 0;
			  $$.capture = 0;
			  $$.check = 0;
			  $$.promote = ChessPiece::Empty;
			  $$.castling = ChessPosition::None; }
	;

   move_check: move	{ $$ = $1; }
	| move '+'	{ $$ = $1; $$.check = 1; }
	;

   move: piece 'x' square	        { $$.piece = $1;
					  $$.capture = 1;
					  $$.square = $3; }
	| piece square			{ $$.piece = $1;
					  $$.square = $2; }

	| piece file maybe_capture square { $$.piece = $1;
					    $$.clarifier.file =  $2;
					    $$.capture = $3;
					    $$.square = $4; }

	| piece rank maybe_capture square { $$.piece = $1;
					    $$.clarifier.rank = $2;
					    $$.capture = $3;
					    $$.square = $4; }

	| square maybe_promote		{ $$.piece = ChessPiece::Pawn;
					  $$.square = $1;
					  $$.promote = $2; }

        | file 'x' square maybe_promote	{ $$.piece = ChessPiece::Pawn;
					  $$.clarifier.file = $1;
					  $$.capture = 1;
					  $$.square = $3;
					  $$.promote = $4; }

	| 'O' '-' 'O'			{ $$.piece = ChessPiece::King;
					  $$.castling =
						ChessPosition::Kingside; }

	| 'O' '-' 'O' '-' 'O'		{ $$.piece = ChessPiece::King;
					  $$.castling =
						ChessPosition::Queenside; }
					  
	;

   piece: 'N'	{ $$ = ChessPiece::Knight; }
        | 'B'	{ $$ = ChessPiece::Bishop; }
	| 'R'	{ $$ = ChessPiece::Rook; }
	| 'Q'	{ $$ = ChessPiece::Queen; }
	| 'K'	{ $$ = ChessPiece::King; }
	;

   maybe_capture: 'x' { $$ = 1; }
	| /* empty */ { $$ = 0; }

   square: file rank	{ $$.file = $1; $$.rank = $2; }

   maybe_promote: /* empty */ { $$ = ChessPiece::Empty; }
	| '=' piece	      { $$ = $2; };

   rank: '1'		{ $$ = 1; }
	| '2'		{ $$ = 2; }
	| '3'		{ $$ = 3; }
	| '4'		{ $$ = 4; }
	| '5'		{ $$ = 5; }
	| '6'		{ $$ = 6; }
	| '7'		{ $$ = 7; }
	| '8'		{ $$ = 8; }
	;

   file: 'a'		{ $$ = 1; }
	| 'b'		{ $$ = 2; }
	| 'c'		{ $$ = 3; }
	| 'd'		{ $$ = 4; }
	| 'e'		{ $$ = 5; }
	| 'f'		{ $$ = 6; }
	| 'g'		{ $$ = 7; }
	| 'h'		{ $$ = 8; }
	;
       
%%

int yylex() {
  if ( move_index >= MAX_MOVE_LEN ) {
    return 0;
  } else {
    return move_str[ move_index++ ];
  }
}

int yyerror( char *s ) {
  err_msg = s;
  return 1;
}

struct Schessmove *do_alg_parse( const char *_move_str ) {
  strncpy( move_str, _move_str, 16 );
  move_str[ MAX_MOVE_LEN ] = '\0';
  move_index = 0;

  //yydebug = 1;

  if ( yyparse() == 0 ) {
    return &move_data;
  } else {
    return NULL;
  }
}

