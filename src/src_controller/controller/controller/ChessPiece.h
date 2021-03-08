#pragma once
#ifndef __CHESSPIECE_H__
#define __CHESSPIECE_H__

#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1


#define USE_STD_LOG

#ifdef USE_STD_LOG
#include <iostream>
#endif


class ChessPiece
{
public:
	///THE FIGURE TYPE OF A CHESS FIGURE
	enum TYPE
	{
		TYPE_INVALID = 0,
		TYPE_KING = 1,
		TYPE_QUEEN = 2,
		TYPE_ROOK = 3,
		TYPE_BISHOP = 4,
		TYPE_KNIGHT = 5,
		TYPE_PAWN = 6,
	};
	///COLOR
	enum  COLOR
	{	
		COLOR_WHITE = 0,
		COLOR_BLACK = 1,	
		COLOR_UNKNOWN
	};
	/// COLOR, TYPE COMBINED WITH THE FIGURE ID
	struct FIGURE
	{
		ChessPiece::COLOR color; //BLACK OR WHITE ONLY ONE BIT
		ChessPiece::TYPE type;
		///THE UPCOUNTING NUMBER OF THE FIGURE TO DETERMN THE PARK POSITION, THE NUMBER IS ONLY UNIQUE COMBINED WITH THE OTHER
		///START COUNTING FROM 0
		int figure_number;
		///THIS IS THE UNIQUE ID OF THE FIGURE HWICH IS ALSO WRITTEN ON THE NFC TAG
		unsigned char  unique_id;
		bool is_empty;
	};
	
	
	static FIGURE getFigureByCharakter(char _figure_rep); //RETURNS A FIGURE BY (WHITE)R,N,B,Q,K,P or (BLACK)r,n,b,q,k,p or invalid
	
	static bool compareFigures(FIGURE _a, FIGURE _b); //A EQUAL FIGURE IS AFIGURE WITH THE SAME TYPE AND THE SAME COLOR NOT THE SAME ID BEACUSE THE HUMAN CAN SWITCH THE FIGURES THEN THE ID CHAGES
	static int getFigureCountByChrakter(char _figure_rep); //returns the normal count if figures of the tsype in a game
	///GENERATES THE UNIQUE ID (WRITTEN ON NFC TAG) FROM A GIVEN FIGURE
	///USING A REFERENCE THEN CALCULATED unique_id WILL BE OVERWRITTEN
	static unsigned char figure2NDEF(ChessPiece::FIGURE& _figure);
	///POPULATES A FIGURE struct FROM AN unique_id 
	///THE FUNCTION IS USED TO PARSE THE NFC TAG DATA TO GET THE SCANNED FIGURE
	static ChessPiece::FIGURE NDEF2Figure(unsigned char _ndef_id);
	
	///PRINTS SOME INFORMATION ABOUT A GIVEN FIGURE
	static void FigureDebugPrint(ChessPiece::FIGURE _figure);
	
	static char Figure2Charakter(ChessPiece::FIGURE _figure);
	
	static bool IsFigureValid(ChessPiece::FIGURE _figure);
	
	
	
};
#endif

