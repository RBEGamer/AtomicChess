typedef union {
  int num;
  ChessMove *move;
} YYSTYPE;
#define	NUM	258
#define	MOVE	259


extern YYSTYPE yylval;
