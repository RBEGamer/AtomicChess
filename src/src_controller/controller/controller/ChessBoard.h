#pragma once
#ifndef __CHESSBOARD_H__
#define __CHESSBOARD_H__

#include <string>
#include <locale>         // std::locale, std::tolower
#include <limits> // int a = std::numeric_limits<int>::max();
#define INT_INF (unsigned)!((int)0)
#include <list>
#include <vector>
#include <cmath> // sqrtl
#include <queue>
#include <regex> //FOR StringToMovePair => to parse a1b2 into fields

#include <chrono>
#include <stdlib.h>
#include <map>
#define USE_STD_LOG

#ifdef USE_STD_LOG
#include <iostream>
#endif

///CHESS LOGIC RELTATED
#include "ChessPiece.h"
#include "ChessField.h"
//HARDWARE LOGIC RELATED
#include "HardwareInterface.h"

//MISC RELATED
#include "ConfigParser.h"

//MISK 3rd PARTY
#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/magic_enum-master/include/magic_enum.hpp"

///CHESS BOARD DIMENSIONS
///FOR WIDTH 8 normal + 4 for parkposition
/// 2 PARK WHITE + 8 CHESS BOARD + 2 PARK BLACK
#define  BOARD_WIDTH (8+4)
#define BOARD_HEIGHT 8

#define DEFAULT_BOARD_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define WAITITME_FOR_MOTORS_TO_ARRIVE 100 //[ms] to check if motor target position is reached
#define WAITIME_MULTIPLIER_AXIS_ARRIVAL 20 //check x times if target postion is reached
#define STRING_MOVE_REGEX "[abcdefgh][12345678][abcdefgh][12345678]" //FOR CHECKING a1b3 h1e3
class ChessBoard
{
	
public:
	///WAYPOINT STRUCT - USED BY THE MoveAlongWaypoints Funktion - Generates by makeMoveSync Function
	struct MV_POSITION
	{
		int x;
		int y;
		bool coil_a_state;
		bool coil_b_state;
		MV_POSITION(int _x, int _y, bool _cas,bool _cbs)
		{
			x = _x;
			y = _y;
            coil_a_state = _cas;
			coil_b_state = _cbs;
				
		}
	};
	
	enum class BOARD_TPYE
	{
	         REAL_BOARD = 0,
			TARGET_BOARD = 1,
			TEMP_BOARD = 2
	};
	
	enum class BOARD_ERROR {
		NO_ERROR = 0,
		MOVE_FAILED_FIGURE_MISSING = 1,
		MOVE_FAILED = 2,
		MOVE_FAILED_TARGET_OCCUPIED = 3,
		MOVE_SUCCESS= 4,
		INIT_NULLPTR_EXECPTION = 5,
		AXIS_TRAGET_ARRIVAL_FAILED = 6,
		BAD_FILED_INDEX = 7,
		INIT_CHESS_FIGURES_NOT_COMPLETE = 8,
		INIT_COMPLETE = 9
	};
	
	enum class BOARD_STATUS {
		BOARD_INIT_FAILED,
		BOARD_INIT_OK
		
	};
	
	enum class BOARD_PRESET {
		BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION = 0,
		BOARD_PRESET_ALL_FIGURES_IN_PARK_POSITION = 1,
		BOARD_PRESET_NO_FIGURES_PLACED = 2
	};
	
	struct FigureField
	{
		ChessField::CHESS_FILEDS field;
		ChessPiece::FIGURE figure;
		FigureField()
		{
		}
		FigureField(ChessField::CHESS_FILEDS _field, ChessPiece::FIGURE _figure)
		{
			field = _field;
			figure = _figure;
		}
	};
	
	struct FigureFieldPair
	{
		FigureField field_curr;
		FigureField field_target;
		bool processed; //IS TRUE IF THIS FIELS IS ALREADY PROCESSED BY THE SYNC BOARD ALGORITHM
	};
	
	struct MovePiar
	{
		ChessField::CHESS_FILEDS from_field;
		ChessField::CHESS_FILEDS to_field;
		bool is_valid;
		
		MovePiar(ChessField::CHESS_FILEDS _from, ChessField::CHESS_FILEDS _to)
		{
			from_field = _from;
			to_field = _to;
			is_valid = true;
		}
		MovePiar()
		{
			is_valid = false;
		}
	};
		
	bool test_make_move_func(std::string& _descr, int& _test_no);
	void test_make_move_static();
	void home_board();
	ChessBoard() ;
	~ChessBoard();
	MovePiar StringToMovePair(std::string _mv);
	std::string board2FEN(ChessBoard::BOARD_TPYE _type); //RETURNS A FEN REPRESENTATION OF THE BOARD
	bool boardFromFen(std::string _fen, ChessBoard::BOARD_TPYE _target_board); //LOADS A BOARD BY FEN
	bool syncRealWithTargetBoard(); ///SNYC THE RealBoard with the Target board and move the 
	bool syncRealWithTargetBoard(MovePiar _ecevute_move);   ///SNYC THE RealBoard with the Target board and move the figures
	void printBoard(ChessBoard::BOARD_TPYE _target_board);   ///PRINT BOARD TO CONSOLE CURRENT AND TARGET
	ChessBoard::BOARD_ERROR scanBoard(bool _include_park_postion);     ///SCANS THE BOARD WITH THE NFC READER AND STORE THE RESULT IN THE GIVEN REFERENCE BOARD
	std::vector<FigureFieldPair> compareBoards(ChessPiece::FIGURE* _board_a, ChessPiece::FIGURE* _board_b, bool _include_park_pos);     ///COMPARE THE REAL AND TARGET BOARD AND GET THE DIFFERENCES
	ChessBoard::BOARD_ERROR initBoard(bool _with_scan);   ///INIT THE MECHANICS AND SCANS THE BOARD
	void loadBoardPreset(ChessBoard::BOARD_TPYE _target_board, ChessBoard::BOARD_PRESET _preset);
	ChessBoard::BOARD_ERROR makeMoveSync(MovePiar _move, bool _with_scan, bool _directly, bool _occupy_check); ///MOVES A FIGURE FROM TO AN FIELD TO AN OTHER _with_scan_scans the figure on start field first; _directly moves figure on direct way, occupy_check ches if target field is alreadey occupied
	ChessBoard::BOARD_ERROR travelToField(ChessField::CHESS_FILEDS _field, IOController::COIL _coil,bool _to_field_center); ///TRAVEL HEAD TO A FIELD DIRECTLY DIAGONAL
	ChessBoard::BOARD_ERROR calibrate_home_pos(); ///MOVE THE HEAD TO FIELD H1
	int get_next_free_park_position(ChessBoard::BOARD_TPYE _target_board, ChessPiece::FIGURE _fig); ///RETURNS THE INDEX OF THE NEXT FREE PARK POSITION FO RTHE GIVEN COLOR
	std::vector<ChessField::CHESS_FILEDS> get_free_fields_on_the_board(ChessPiece::FIGURE* _board_pointer);
	std::vector<ChessField::CHESS_FILEDS> get_parking_fileld_occupied_by_figure_type(ChessPiece::FIGURE* _board_pointer, ChessPiece::FIGURE _fig);
	
	int get_figure_type_count(ChessBoard::BOARD_TPYE _target_board, char _type_char , bool _board_only); ///RETURNS THE COUNT OF FIGURES PLACED ON THE BOARD
	int get_figure_type_count(ChessPiece::FIGURE* _board_pointer, char _type_char , bool _board_only); ///RETURNS THE COUNT OF FIGURES PLACED ON THE BOARD
	
	
private:
	

	IOController::COIL current_selected_coil = IOController::COIL::COIL_A;
	ChessField::CHESS_FILEDS current_field;
	///REPRESENTS THE CHESS BOARD
	ChessPiece::FIGURE board_current[BOARD_WIDTH*BOARD_HEIGHT];  ///REPRESENTS THE CURRENT CHESS BOARD (=> THE MECHANICAL/REAL WORLD)
	ChessPiece::FIGURE board_target[BOARD_WIDTH*BOARD_HEIGHT]; ///REPRESENTS THE TARGETBOARD WHICH SHOULD BE ARCHVIED
	ChessPiece::FIGURE board_temp[BOARD_WIDTH*BOARD_HEIGHT];   ///USED FOR FEN PARSING
	void log_error(std::string _err);
	
	void getFieldCoordinates(ChessField::CHESS_FILEDS _index, int& _x, int& _y, IOController::COIL _coil, bool _get_only_array_index, bool _get_field_center);
	void getParkPositionCoordinates(ChessField::CHESS_FILEDS _index, int& _x, int& _y, IOController::COIL _coil, bool before_parkpostion_entry);
	bool isFieldParkPosition(ChessField::CHESS_FILEDS _field);
	ChessBoard::BOARD_ERROR switch_coil(IOController::COIL _coil, bool _activate_swtiched_coil);
	ChessBoard::BOARD_ERROR get_coil_offset(IOController::COIL _coil, int& _x, int& _y);
	
	
	
	std::list<ChessPiece::FIGURE> checkBoardForFullFigureSet(ChessPiece::FIGURE(&board)[BOARD_WIDTH][BOARD_HEIGHT]);
	IOController::COIL getValidCoilTypeParkPosition(ChessField::CHESS_FILEDS _field, IOController::COIL _target); //GET THE RIGHT COIL FOR THE PARK POS
	std::vector<ChessField::CHESS_FILEDS> get_chess_fields_occupied_from_figure(ChessPiece::FIGURE* _board_pointer, ChessPiece::FIGURE _fig, bool _board_only); //RETURNS FIELD LIST WITH ALL SELECTED TYPES OF FIGURE _fig OCCUPIEDS FILES
	
	ChessPiece::FIGURE* get_board_pointer(ChessBoard::BOARD_TPYE _target_board);  //RETURNS THE POINTER TO A SPCIFIED BOARD ARRAY
	
	bool MoveWaypointsAlong(std::queue<MV_POSITION>& _mv);  //MOVES THE HEAD ALONG A LIST OF WAYPOINTS
	bool makeMoveFromParkPositionToBoard(ChessField::CHESS_FILEDS _park_pos, ChessField::CHESS_FILEDS _dest_pos, std::queue<MV_POSITION>& _generated_waypoint_list, int& _dest_pos_x, int& _dest_pos_y, bool _EN_BOARD_SIZE_WORKAROUND);    //MOVES THE FIGURES FROM THE PARKPOSTION THE THE H4,5 OR A4,5 FIELD => GENERATED THE MOVES LIST
	ChessField::CHESS_FILEDS getNextHalfFieldFromParkPos(ChessField::CHESS_FILEDS _board_field);
	bool makeMoveFromBoardToParkPosition(ChessField::CHESS_FILEDS _park_pos, std::queue<MV_POSITION>& _generated_waypoint_list, int _current_x, int _current_y);
	
	void initBoardArray(ChessPiece::FIGURE* _board); //INITS THE GIVEN BOARD ARRAY TO A COMPLETE EMPTY FIELD
	
	bool isBoardFieldOccupied(ChessPiece::FIGURE* _board,ChessField::CHESS_FILEDS _field);
	ChessPiece::FIGURE getFigureOnField(ChessPiece::FIGURE* _board, ChessField::CHESS_FILEDS _field);
	
	std::vector<ChessBoard::FigureFieldPair>::iterator getFigureInDiffListPlacedOnEmptpy(std::vector<ChessBoard::FigureFieldPair>& _figlist, ChessPiece::FIGURE _fig);
	
	bool syncRealWithTargetBoard_add_remove_empty(); //CALCULCATES ONLY ADD/REMOVE FIGURE FROM/TO FIELS MOVES => RETURNS TRUE IF A CHANGE/MOVE WAS MADE
	bool syncRealWithTargetBoard_not_empty();
	
	ChessPiece::TYPE getSpecificFigureTypeForParkpostion(ChessField::CHESS_FILEDS _field); //THIS FUNCTIONS DETERM THE SPECIFIC TYPE THE PARKS POSITION IS DESIGNED FOR => PARKPOS 1 TO 8 ARE THE PAWNS
	
	bool copyBoard(ChessPiece::FIGURE* _from, ChessPiece::FIGURE* _to);
};

#endif //__CHESSBOARD_H__

