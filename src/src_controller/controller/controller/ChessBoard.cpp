#include "ChessBoard.h"


	

ChessBoard::ChessBoard() {
	
	if (!ConfigParser::getInstance()->configLoaded()) {
		LOG_F(ERROR, "CONFIG NOT LOADED");
	}
	
	if (!HardwareInterface::getInstance()->check_hw_init_complete())
	{
		LOG_F(ERROR, "ChessBoard::ChessBoard HARDWARE INIT NOT COMPLETE LOADED");
	}
	//INIT BOARD ARRAYS => TO EMPY FIELDS
	initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD));
	initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD));
	initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TEMP_BOARD));
}
	
ChessBoard::~ChessBoard() {
}

void ChessBoard::test_make_move_static()
{
	MovePiar tmp_pair;
	tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_G2;
	tmp_pair.to_field =  ChessField::CHESS_FILEDS::CHESS_FIELD_A2;
	makeMoveSync(tmp_pair, false, false, false);
}
	
bool ChessBoard::test_make_move_func(std::string& _descr, int& _test_no)
{
	
	home_board();
	MovePiar tmp_pair;
	
	switch (_test_no)
	{
	case 2: {
		_descr = "H1 -> PARK_WHITE_1";
			tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_H1;
			tmp_pair.to_field =  ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1;
			makeMoveSync(tmp_pair, false, false, false); 
			break;
		}
	case 3: {
			_descr = "H2 -> PARK_WHITE_16";
			tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_H2;
			tmp_pair.to_field =  ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_16;
			makeMoveSync(tmp_pair, false, false, false);
			break;
		}
	case 0: {
		_descr = "H3 -> PARK_BLACK_1";
			tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_H3;
			tmp_pair.to_field =  ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1;
			makeMoveSync(tmp_pair, false, false, false);
			break;
		}
	case 1: {
		_descr = "H4 -> PARK_BLACK_16";
			tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_H4;
			tmp_pair.to_field =  ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_16;
			makeMoveSync(tmp_pair, false, false, false); 
			break;
		}
	default: _test_no = -1; break;
	}
	
	
	if (_test_no >= 0) {
		_test_no++;
	}
	return true;
}
	
	

bool ChessBoard::MoveWaypointsAlong(std::queue<MV_POSITION>& _mv)
{
	
	//LOAD CONFIG FOR MOVEMENT //TODO MOVE TO CONSTRUCTOR
	bool EN_COIL_UPDATE_ALWAYS = ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_MAKE_MOVE);
	bool EN_COILE_ALWAYS_WRITE_OFF = ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_WRITE_OFF);
	
	
	
	
	//SET MOTORS TO CHESS FIGURE MOVE SPEED
	HardwareInterface::getInstance()->set_speed_preset(HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);

	//DISABLE COILS
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
	//ENABLE MOTOR
	HardwareInterface::getInstance()->enable_motors();
	
	int wait_counter = 0;
	bool ca_changed = false;
	bool cb_changed = false;
	while (!_mv.empty())
	{
		if (EN_COILE_ALWAYS_WRITE_OFF)
		{
			HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
			HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
		}
			
			
		
		const MV_POSITION tmp = _mv.front();
		
		//SOME IMPROVEMENTS => SPI WRITING IS EXPENSIVE
		if(EN_COIL_UPDATE_ALWAYS || ca_changed != tmp.coil_a_state)
		{
			ca_changed = tmp.coil_a_state;
			HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, tmp.coil_a_state);
		}
		if (EN_COIL_UPDATE_ALWAYS || cb_changed != tmp.coil_b_state)
		{
			cb_changed = tmp.coil_b_state;
			HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, tmp.coil_b_state);
		}
			
	
		HardwareInterface::getInstance()->move_to_postion_mm_absolute(tmp.x, tmp.y, false);           //MOVE TO X AND NOT WAIT
		//WAIT FO BOTH MOTORS
		while(!(HardwareInterface::getInstance()->is_target_position_reached())) {
			std::this_thread::sleep_for(std::chrono::milliseconds(WAITITME_FOR_MOTORS_TO_ARRIVE));
		}
		_mv.pop();
	}	
	//DISBALE MOTOR
	HardwareInterface::getInstance()->disable_motors();
	//DISABLE COILS
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
}

//GET NEXT FIELD TO PARK POS
ChessField::CHESS_FILEDS ChessBoard::getNextHalfFieldFromParkPos(ChessField::CHESS_FILEDS _board_field)
{	
	
	IOController::COIL parkpos_needed_coil = getValidCoilTypeParkPosition(_board_field, IOController::COIL::COIL_A);
	//IF NOT PARK POS RETURN THE BOARD FIELD COODIANTES
	if(!isFieldParkPosition(_board_field))
	{
		return _board_field;
	}
	
	//GET POSITION OF THE PARK POS
	int pp_x = -1;
	int pp_y = -1;
	getParkPositionCoordinates(_board_field, pp_x, pp_y, parkpos_needed_coil, true);

	//NOW FIND THE NEXT BOARD FIELD NEXT TO THIS POSITION
	int bf_x = std::numeric_limits<int>::max();
	int bf_y = std::numeric_limits<int>::max();	
	int tmp_x = std::numeric_limits<int>::max();
	int tmp_y = std::numeric_limits<int>::max();
	float distance = 0.0f;
	float smallest_distance = std::numeric_limits<float>::max();
	ChessField::CHESS_FILEDS nearest_field = _board_field;
	///EDIT START STOP FIELDS HERE
	const int start_index = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_A1);
	const int stop_index = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1);
	
	//FOR EACH BOARD FIELD
	for(int i = start_index ; i < stop_index ; i++)
	{
		//GET FIELD COODIANTES
		getFieldCoordinates(ChessField::Index2Field(i), tmp_x, tmp_y, parkpos_needed_coil, false, true);
		//USE PYTHAGORAS TO GET THE DISTANCE BEWEEN THE POINTS
		distance = std::hypotl(tmp_x - pp_x, tmp_y - pp_y);
		if (distance < smallest_distance)
		{
			smallest_distance = distance;
			nearest_field = ChessField::Index2Field(i);
		}
	}
		

	return nearest_field;
}


bool ChessBoard::makeMoveFromBoardToParkPosition(ChessField::CHESS_FILEDS _park_pos, std::queue<MV_POSITION>& _generated_waypoint_list, int _current_x, int _current_y)
{	
	//IF NOT PARK POS RETURN ADD THE FINAL TARGET FIELD POSITION TO THE QUEUE
	if(!isFieldParkPosition(_park_pos))
	{
		IOController::COIL parkpos_needed_coil = getValidCoilTypeParkPosition(_park_pos, IOController::COIL::COIL_A);
		getFieldCoordinates(_park_pos, _current_x, _current_y, parkpos_needed_coil, false, true);
		_generated_waypoint_list.push(MV_POSITION(_current_x, _current_y, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
		return false;
	}
	
	int pp_pos_inside_x = 0;
	int pp_pos_inside_y = 0;  
	
	int pp_pos_before_x = 0;
	int pp_pos_before_y = 0;  
	IOController::COIL parkpos_needed_coil = getValidCoilTypeParkPosition(_park_pos, IOController::COIL::COIL_A);

	
	//GET PARK POS COORIDNATES IN THE PARK POS
	getParkPositionCoordinates(_park_pos, pp_pos_inside_x, pp_pos_inside_y, parkpos_needed_coil, false);
	//GET PARK POS COORIDNATES IN THE BEFORE THE PARK POS
	getParkPositionCoordinates(_park_pos, pp_pos_before_x, pp_pos_before_y, parkpos_needed_coil, true);
	
	
	//MOVE OUT THE PARK POS COILS
	///DONT CHANGE Y AXIS 
	_generated_waypoint_list.push(MV_POSITION(pp_pos_before_x, _current_y, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
	//	MoveWaypointsAlong(_generated_waypoint_list);
		//MOVE TO PARK POS BEFORE ENTRY
		_generated_waypoint_list.push(MV_POSITION(pp_pos_before_x, pp_pos_before_y, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
	//	MoveWaypointsAlong(_generated_waypoint_list);
	
		//FINALLY MOVE IN
		_generated_waypoint_list.push(MV_POSITION(pp_pos_inside_x, pp_pos_inside_y, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
	//	MoveWaypointsAlong(_generated_waypoint_list);
		//TURN OFF COILS
		_generated_waypoint_list.push(MV_POSITION(pp_pos_inside_x, pp_pos_inside_y, false, false));
	//	MoveWaypointsAlong(_generated_waypoint_list);
		return true;
}
	
	
bool ChessBoard::makeMoveFromParkPositionToBoard(ChessField::CHESS_FILEDS _park_pos, ChessField::CHESS_FILEDS _dest_pos, std::queue<MV_POSITION>& _generated_waypoint_list, int& _dest_pos_x, int& _dest_pos_y, bool _EN_BOARD_SIZE_WORKAROUND)
{
	if (!isFieldParkPosition(_park_pos))
	{
		return false;
	}
	//GET FIELD WITDTH /2
	int field_width = 50;
	if (!ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH, field_width))
	{
		field_width = 50;        //SET TO DEFAULT WIDTH
	}
	field_width /= 2;        //WANT TO TRAVEL BETWEEN TWO FIELDS
	//MOVE TO PARK START POSTION
	int pp_pos_inside_x = 0;
	int pp_pos_inside_y = 0;  
	
	int pp_pos_before_x = 0;
	int pp_pos_before_y = 0;  
	IOController::COIL parkpos_needed_coil = getValidCoilTypeParkPosition(_park_pos, IOController::COIL::COIL_A);
	IOController::COIL target_coil = getValidCoilTypeParkPosition(_dest_pos, IOController::COIL::COIL_A);
	
	//GET PARK POS COORIDNATES IN THE PARK POS
	getParkPositionCoordinates(_park_pos, pp_pos_inside_x, pp_pos_inside_y, parkpos_needed_coil, false);
	//GET PARK POS COORIDNATES IN THE BEFORE THE PARK POS
	getParkPositionCoordinates(_park_pos, pp_pos_before_x, pp_pos_before_y, parkpos_needed_coil, true);
	
	//TRAVEL TO PARK POS
	_generated_waypoint_list.push(MV_POSITION(pp_pos_inside_x, pp_pos_inside_y, false, false));
	//MoveWaypointsAlong(_generated_waypoint_list);
	//ACTIVATE COILS
	_generated_waypoint_list.push(MV_POSITION(pp_pos_inside_x, pp_pos_inside_y, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
	//MoveWaypointsAlong(_generated_waypoint_list);
	//MOVE OUT THE PARK POS COILS
	_generated_waypoint_list.push(MV_POSITION(pp_pos_before_x, pp_pos_before_y, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
	//MoveWaypointsAlong(_generated_waypoint_list);
	
	//MOVE THE FIGURE TO THE _target_field y position
	int target_pos_x = 0;
	int target_pos_y = 0;
	getFieldCoordinates(_dest_pos, target_pos_x, target_pos_y, target_coil, false, true);
	
	//TARGET POS IS EXACLTY BETWEEN TWO FIELDS
	if(_EN_BOARD_SIZE_WORKAROUND) {
		_generated_waypoint_list.push(MV_POSITION(pp_pos_before_x, target_pos_y - field_width, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
	}else {
		_generated_waypoint_list.push(MV_POSITION(pp_pos_before_x, target_pos_y + field_width, parkpos_needed_coil == IOController::COIL::COIL_A, parkpos_needed_coil == IOController::COIL::COIL_B));
		
	}
	//MoveWaypointsAlong(_generated_waypoint_list);
	_dest_pos_x = pp_pos_before_x;
	_dest_pos_y = target_pos_y;
	
	return true;
}


ChessBoard::BOARD_ERROR ChessBoard::makeMoveSync(ChessBoard::MovePiar _move, bool _with_scan, bool _directly, bool _occupy_check)
{
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
	//IF FIELDS EQUAL NOTHING TODO
	if(_move.to_field == _move.from_field) {
		return ChessBoard::BOARD_ERROR::NO_ERROR;
	}
	
	int coil_switch_pos = 200;
	if (!ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_COIL_SWITCH_POSTION_TRIGGER, coil_switch_pos))
	{
		coil_switch_pos = 200;
	}
	
	
	//------------------- BOARD SIZE LIMIT WORKAROUND ---------------------- //
	bool EN_BSIZE_WORKAROUND = false;
	if (!ConfigParser::getInstance()->getBool(ConfigParser::CFG_ENTRY::MECHANIC_BOARD_SIZE_550MM_WORKAROUND, EN_BSIZE_WORKAROUND))
	{
		EN_BSIZE_WORKAROUND = true;
	}
	//GET FIELD WITDTH /2
	int field_width = 50;
	if (!ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH, field_width))
	{
		field_width = 50;       //SET TO DEFAULT WIDTH
	}
	field_width /= 2;       //WANT TO TRAVEL BETWEEN TWO FIELDS
	//GET COIL OFFSET FOR THE LATER COIL SWITCH
	int coil_distance = 0;
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM, coil_distance);
	//COLLECTED THE MOVEMENT STEPS WITH COIL INFORMATION
	std::queue<MV_POSITION> position_queue;

	//DISABLE COILS

	//GET THE NEDDED COILS TO REACH THE POSITONS
	IOController::COIL start_coil = getValidCoilTypeParkPosition(_move.from_field, IOController::COIL::COIL_A);
	IOController::COIL end_coil = getValidCoilTypeParkPosition(_move.to_field, IOController::COIL::COIL_B);
	//GET THE POSITIONS FROM START / END
	//WITHIN THE BOARD !!!
	int x_start = -1;
	int y_start = -1;
	int x_end = -1;
	int y_end = -1;
	bool is_start_park_pos = false;
	bool is_end_park_pos = false;
	
	
	//WORKAROUND FOR TOO SMALL BOARD
	bool EN_START_WORKAROUND = false;
	bool EN_END_WORKAROUND = false;
	int INVERT_FIELD_OFFSET = 1;
	int INVERT_FIELD_OFFSET_END = 1;
	//------------------- BOARD SIZE LIMIT WORKAROUND ---------------------- //
	if(EN_BSIZE_WORKAROUND && (_move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_A8 || _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_B8 || _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_C8 || _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_D8 || _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_E8 || _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_F8 || _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_G8 || _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_H8))
	{
		EN_START_WORKAROUND = true;
		INVERT_FIELD_OFFSET = -1;
	}
	if (EN_BSIZE_WORKAROUND && (_move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_A8 || _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_B8 || _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_C8 || _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_D8 || _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_E8 || _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_F8 || _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_G8 || _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_H8))
	{
		EN_END_WORKAROUND = true;
		INVERT_FIELD_OFFSET_END = -1;
	}
	//
	if(ChessField::is_row_over_row(_move.from_field, _move.to_field))
	{
		EN_END_WORKAROUND = true;
		INVERT_FIELD_OFFSET_END = -1;
	}
		
	//FIST GET THE MOTOR COORINATES
	if(isFieldParkPosition(_move.from_field))
	{
		makeMoveFromParkPositionToBoard(_move.from_field, _move.to_field, position_queue, x_start, y_start, EN_END_WORKAROUND);
		is_start_park_pos = true;
		//------------------- BOARD SIZE LIMIT WORKAROUND ---------------------- //
		if(EN_END_WORKAROUND)
		{
			EN_START_WORKAROUND = true;
			INVERT_FIELD_OFFSET = -1;
		}
			
	}else {
		getFieldCoordinates(_move.from_field, x_start, y_start, start_coil, false, true);
		start_coil = getValidCoilTypeParkPosition(_move.from_field, IOController::COIL::COIL_B);
		
	}
	
	if (isFieldParkPosition(_move.to_field))
	{
		ChessField::CHESS_FILEDS end_field_tmp = getNextHalfFieldFromParkPos(_move.to_field);     // GET NEAREST FIELD NEXT TO PARK POS
		getFieldCoordinates(end_field_tmp, x_end, y_end, end_coil, false, true);     //CALCULATE NEW COORDINATES FOR THE CHANGED FIELD
		end_coil = getValidCoilTypeParkPosition(end_field_tmp, IOController::COIL::COIL_B);
		is_end_park_pos = true;
	}
	else {
		getFieldCoordinates(_move.to_field, x_end, y_end, end_coil, false, true);
	}
	
	//FIRST TRAVEL TO START POSITON
	if(!is_start_park_pos) {
		position_queue.push(MV_POSITION(x_start, y_start, false, false));
		position_queue.push(MV_POSITION(x_start, y_start, start_coil == IOController::COIL::COIL_A, start_coil == IOController::COIL::COIL_B));
	}
	
	//MoveWaypointsAlong(position_queue);
	//SECOND TRAVEL 1/2 FIELD UP OR DOWN IF START WORKAROUND
	position_queue.push(MV_POSITION(x_start, y_start + field_width*INVERT_FIELD_OFFSET, start_coil == IOController::COIL::COIL_A, start_coil == IOController::COIL::COIL_B));
	
	
	//	MoveWaypointsAlong(position_queue);

		//CHECK IF COIL_SWITCH NEEDED
		//INERT A BREAKPOINT BEWEEN FIELD D AND E ON THE SAME Y LINE
	
		if(start_coil != end_coil) {
		int coil_offset = 0;
		
		//	MoveWaypointsAlong(position_queue);
			//MOVE TO THE COIL SWITCH POSITION WITH THE START COIL
			if(start_coil == IOController::COIL::COIL_A) {
			coil_offset = coil_switch_pos;
		}
		else if(start_coil == IOController::COIL::COIL_B) {
			coil_offset = coil_switch_pos - coil_switch_pos / 2;	
		}
		
		
		position_queue.push(MV_POSITION(coil_offset, y_start + field_width*INVERT_FIELD_OFFSET, start_coil == IOController::COIL::COIL_A, start_coil == IOController::COIL::COIL_B));
	
		
		
		//MoveWaypointsAlong(position_queue);
		//TURN COILS OFF
		position_queue.push(MV_POSITION(coil_offset, y_start + field_width*INVERT_FIELD_OFFSET, false, false));
		//MoveWaypointsAlong(position_queue);
		//GET COORDINATES FOR THE COIL SWITCH
		//=> SWITCH TO B COIL => -coil_offset
		if(start_coil == IOController::COIL::COIL_A && end_coil == IOController::COIL::COIL_B)
		{
			position_queue.push(MV_POSITION(coil_offset - coil_switch_pos / 2, y_start + field_width*INVERT_FIELD_OFFSET, false, false));
			//MoveWaypointsAlong(position_queue);
			position_queue.push(MV_POSITION(coil_offset - coil_switch_pos / 2, y_start + field_width*INVERT_FIELD_OFFSET, end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));
		}else if(start_coil == IOController::COIL::COIL_B && end_coil == IOController::COIL::COIL_A)
		{
			position_queue.push(MV_POSITION(coil_offset + coil_switch_pos / 2, y_start + field_width*INVERT_FIELD_OFFSET, false, false));
			//MoveWaypointsAlong(position_queue);
			position_queue.push(MV_POSITION(coil_offset + coil_switch_pos / 2, y_start + field_width*INVERT_FIELD_OFFSET, end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));
			
		}
		//MoveWaypointsAlong(position_queue);
		
	}
	
	//MOVE TO TARGET X COORDINATES WITH 1/2 FIELD OFFSET
	position_queue.push(MV_POSITION(x_end + field_width, y_start + field_width*INVERT_FIELD_OFFSET, end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));

	//MoveWaypointsAlong(position_queue);
	//MOVE TO TARGET Y COORDINATES WITH 1/2 FIELD OFFSET
	position_queue.push(MV_POSITION(x_end + field_width, y_end + field_width*INVERT_FIELD_OFFSET_END, end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));

	//MoveWaypointsAlong(position_queue);
	//MOVE TO FIELD CENTER OR FURTHER TO MARK POS
	if(!is_end_park_pos) {
		position_queue.push(MV_POSITION(x_end, y_end, end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));
	}else
	{
		//TODO MOVE FROM THERE TO FINAL PARK POS
		makeMoveFromBoardToParkPosition(_move.to_field, position_queue, x_end + field_width, y_end + field_width);
	}

	
	//FINALLY EXECUTE MOVE
	MoveWaypointsAlong(position_queue);
	
	//HOME BOARD AFTER MOVE
	
	if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_BOARD_HOME_AFTER_MAKE_MOVE))
	{
		home_board();
		//WORKAROUND FOR FIRST DK HARDWARE THAT IS NOT ABLE TO GET PO PARK POSITIONS WITHOUT LOSING STEPS
			//THEN AFTER EACH MOVE THE MECHANIC RUNS A HOME CYCLE TO ENSURE THAT THE MECHANIC IS FOR THE NEXT MOVE IN THE CORRET POSITION
	}else
	{
		if (!HardwareInterface::getInstance()->is_production_hardware() && (isFieldParkPosition(_move.to_field) || isFieldParkPosition(_move.from_field)))
		{
			home_board();
		}
	}
	
	
	
	
	//FINALLY SAVE MADE MOVE INTO THE CURRENT BOARD
	ChessPiece::FIGURE*board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
	ChessPiece::FIGURE from_fig = getFigureOnField(board_current, _move.from_field);
	const int t_index = ChessField::field2Index(_move.to_field);
	board_current[t_index] = from_fig;
	//CREATE EMPTY FIGURE
	ChessPiece::FIGURE empty_fig;
	empty_fig.color = ChessPiece::COLOR_UNKNOWN;
	empty_fig.type = ChessPiece::TYPE_INVALID;
	empty_fig.is_empty = true;     //!!
	empty_fig.figure_number = -1;
	//THE FROM FIELS IS NOW EMPTY
	const int f_index = ChessField::field2Index(_move.from_field);
	board_current[f_index] = empty_fig;
#ifdef DEBUG
	printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);			  
#endif // DEBUG

	
	return ChessBoard::BOARD_ERROR::NO_ERROR;
}
	

ChessBoard::MovePiar ChessBoard::StringToMovePair(std::string _mv)
{
	MovePiar tmp;
	tmp.is_valid = true;
	//CHECK STRING POPULATES
	if(_mv.empty())
	{
		tmp.is_valid = false;
	}
	//CHECK STRING LENGHT OK
	if(_mv.size() != 4)
	{
		tmp.is_valid = false;
	}
	//STRING TO LOWER CASE	
	transform(_mv.begin(), _mv.end(), _mv.begin(),::tolower);
	
	//CHECK FORMAT BY REGEX
	if(!std::regex_match(_mv, std::regex(STRING_MOVE_REGEX))) {
		tmp.is_valid = false;
	}
	
	int fca = _mv.at(0);
	int fcb =  _mv.at(2);
	int fta = fca - 97;
	int ftb = fcb - 97;
	int ffa = _mv.at(1) - 48 - 1;
	int ffb = _mv.at(3) - 48 - 1;
	int ft = ffa + (fta * 8);
	int tf = ffb + (ftb * 8);
	
	
	
	tmp.from_field = ChessField::Index2Field(ft);
	tmp.to_field = ChessField::Index2Field(tf);
	
	return tmp;
}


bool ChessBoard::isBoardFieldOccupied(ChessPiece::FIGURE* _board, ChessField::CHESS_FILEDS _field)
{
	if (!_board)
	{
		return false;
		//TODO LOGGER
	}
	
	ChessPiece::FIGURE fd = getFigureOnField(_board, _field);
	if (!fd.is_empty)
	{
		return true;
	}
	
	
		
	return false;
}

ChessPiece::FIGURE ChessBoard::getFigureOnField(ChessPiece::FIGURE* _board, ChessField::CHESS_FILEDS _field)
{
	if (!_board)
	{
		ChessPiece::FIGURE empty_fig;
		empty_fig.color = ChessPiece::COLOR_UNKNOWN;
		empty_fig.type = ChessPiece::TYPE_INVALID;
		empty_fig.is_empty = true;         //!!
		empty_fig.figure_number = -1;
		return empty_fig;
		//TODO LOGGER
	}
	int fdindex = ChessField::field2Index(_field);
	ChessPiece::FIGURE fd = _board[fdindex];
	//IF is_empty flag is set but nor a valid fiure => set flag correctly
	//THIS IS A WORKAROUND FOR INVALD READ NFC TAGS
	if(!fd.is_empty && fd.type == ChessPiece::TYPE_INVALID && fd.color == ChessPiece::COLOR_UNKNOWN)
	{
		ChessPiece::FIGURE empty_fig;
		empty_fig.color = ChessPiece::COLOR_UNKNOWN;
		empty_fig.type = ChessPiece::TYPE_INVALID;
		empty_fig.is_empty = true;         //!!
		empty_fig.figure_number = -1;
		_board[ChessField::field2Index(_field)] = empty_fig;
		
		return empty_fig;
	}
	
	return fd;
}


bool ChessBoard::syncRealWithTargetBoard(MovePiar _ecevute_move) {	
	
	//CHECK IF GOT A MOVE
	if(!_ecevute_move.is_valid) {
		return syncRealWithTargetBoard();
		
	}
	
	
	std::vector<MovePiar> move_list;
	
	//CHECK IF TARGET FIELD IS OCCUPIED
	 ChessPiece::FIGURE*board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
	ChessPiece::FIGURE*board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);
	
	if (isBoardFieldOccupied(board_current, _ecevute_move.from_field) &&  isBoardFieldOccupied(board_current, _ecevute_move.to_field))
	{
		//AND MOVE FIGURE INTO PARKING 
		ChessPiece::FIGURE fig_to_remove = getFigureOnField(board_current, _ecevute_move.to_field);
		int fdIndex = get_next_free_park_position(ChessBoard::BOARD_TPYE::REAL_BOARD, fig_to_remove);
		if (fdIndex >= 0)
		{
			//ADD ADDITIONAL MOVE STEP TO MOVE THE FIGURE PLACED ON THE TARGET FIELD AWAY
			move_list.push_back(MovePiar(_ecevute_move.to_field, ChessField::Index2Field(fdIndex)));
			
		}
		//ADD FIRST MOVE => TARGET FIELD IS NOW EMPTY
		move_list.push_back(_ecevute_move);
		
	}if (!isBoardFieldOccupied(board_current, _ecevute_move.from_field) &&  isBoardFieldOccupied(board_current, _ecevute_move.to_field))
	{
		//ADD FIREGURE TO FIELD IF NEDDED
		ChessPiece::FIGURE fig_to_add = getFigureOnField(board_target, _ecevute_move.to_field);
		std::vector<ChessField::CHESS_FILEDS> fd = get_parking_fileld_occupied_by_figure_type(board_current, fig_to_add);
		if (fd.size() > 0)
		{
			move_list.push_back(MovePiar(fd.at(0), _ecevute_move.to_field));
		}
	}
	else {
		move_list.push_back(_ecevute_move);
	}

	//EXECUTE MOVES
	for(size_t i = 0 ; i < move_list.size() ; i++)
	{
		makeMoveSync(move_list.at(i), false, false, false);
	}
	copyBoard(board_current, board_target);
	return true;
}

void ChessBoard::initBoardArray(ChessPiece::FIGURE* _board)
{
	if (!_board)
	{
		return;
	}
	
	ChessPiece::FIGURE empty_fig;
	empty_fig.color = ChessPiece::COLOR_UNKNOWN;
	empty_fig.type = ChessPiece::TYPE_INVALID;
	empty_fig.is_empty = true;       //!!
	empty_fig.figure_number = -1;
	
	
	for (int i = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_A1); i < ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END); i++) {
		_board[i] = empty_fig;
	}
	
	
}


std::vector<ChessBoard::FigureFieldPair> ChessBoard::compareBoards(ChessPiece::FIGURE* _board_a, ChessPiece::FIGURE* _board_b, bool _include_park_pos)
{
	std::vector<ChessBoard::FigureFieldPair> diff_list;
	ChessPiece::FIGURE*board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
	ChessPiece::FIGURE*board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);
	
	if (!_board_a || !_board_b)
	{
		return diff_list;
	}

	int offset = 2;
	if (_include_park_pos)
	{
		offset = 0;
	}
	
	
	//NOW CHECK BOARD DIFFERENCES
	for(int i = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_A1) ; i < ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1) ; i++) {
		ChessPiece::FIGURE tmp_curr = getFigureOnField(board_current, ChessField::Index2Field(i));
		ChessPiece::FIGURE tmp_target = getFigureOnField(board_target, ChessField::Index2Field(i));
	
	
	
		//CHECK IF EQUAL FIGURES
		if(ChessPiece::compareFigures(tmp_curr, tmp_target))
		{
			continue;
		}
			
		ChessBoard::FigureFieldPair tmp;
		tmp.field_curr = ChessBoard::FigureField(ChessField::Index2Field(i), tmp_curr);
		tmp.field_target = ChessBoard::FigureField(ChessField::Index2Field(i), tmp_target);
		tmp.processed = false;
		diff_list.push_back(tmp);
	}		 

	return diff_list;
}


std::vector<ChessBoard::FigureFieldPair>::iterator ChessBoard::getFigureInDiffListPlacedOnEmptpy(std::vector<ChessBoard::FigureFieldPair>& _figlist, ChessPiece::FIGURE _fig)
{
	std::vector<FigureFieldPair>::iterator nit;	
	for (nit = _figlist.begin(); nit != _figlist.end(); nit++)
	{
		if (!nit->processed && nit->field_target.figure.type == _fig.type && nit->field_target.figure.color == _fig.color && !nit->field_target.figure.is_empty && nit->field_curr.figure.is_empty)
		{
			return nit;
			
		}
	}
	return _figlist.end();
	
}

bool ChessBoard::copyBoard(ChessPiece::FIGURE* _from, ChessPiece::FIGURE* _to) //a=from b=to
{
	if (!_from || !_to)
	{
		return false;
	}
	
	const int c_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_A1);
	const int c_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END);  
	
	for (int i = c_start; i < c_end; i++)
	{
		_to[i] = _from[i];
	}
	
	
	
}

bool ChessBoard::syncRealWithTargetBoard() {
	ChessPiece::FIGURE*board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
	ChessPiece::FIGURE*board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);
	volatile bool _changes_made = true;
	while (_changes_made) {
		//syncRealWithTargetBoard_add_remove_empty RETURNS FALSE IF NO MOVE WAS MADE => SO BOARDS ARE IN SYNC
		if(!syncRealWithTargetBoard_add_remove_empty()) {
			break;
		
			//	_changes_made = _changes_made | syncRealWithTargetBoard_not_empty();
		}
	}
	
	copyBoard(board_current, board_target);
	
#ifdef DEBUG
	printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);
	printBoard(ChessBoard::BOARD_TPYE::TARGET_BOARD);			  
#endif // DEBUG

	
	
	return true;
}


bool ChessBoard::syncRealWithTargetBoard_not_empty() {

	//USE MAKE MOVE TO GENERATE A MOVE LIST THEN EXECUTE THE MOVES
	 std::vector<ChessBoard::MovePiar> move_list;
	ChessPiece::FIGURE*board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
	ChessPiece::FIGURE*board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);
	
	if (!board_current || !board_target)
	{
		return false;
	}
	
	//LAMBDA FUNCTION FOR GETTING THE NEXT FIGURE THATS WITH THE SAME TYPE WHICH WAS ALSO MODIFIED
	std::vector<FigureFieldPair>::iterator gfidlp_it;
	//GET THE DIFFERNCE BETWEEN THE TWO BOARDS AS LIST
	std::vector<FigureFieldPair> diff = compareBoards(board_current, board_target, true);
	
	if (diff.size() == 0)
	{
		return false;
	}
	
	//NOW THE HAVE THE DIFFERENCES
	
	//ENTRY X )target: empty | current: black pawn
	//ENTRY X+n )target: black pawn | current: white pawn
	//ENTRY X+m )target: white pawn | current: empty
	//THESE THREE FIELDS CAN BE MERGED INTO ZWO MOVES
	
	//WITH FIRST MOVE X+m => PARKPOS
	// X+n => X+m
	// X => X+n
	
	//ITERATE THOUGHT LIST
	
	for(size_t it = 0 ; it < diff.size() ; it++) {
		if (diff.at(it).processed)
		{
			it++;
			continue;
		}
		
		
		//FIRGURE REMVOED/ADDED FROM/TO FIELD
		//WHERE THE DESTIANTATION FIELS IS NOT EMPTY
		if(!diff.at(it).field_curr.figure.is_empty && !diff.at(it).field_target.figure.is_empty)
		{
			//FIND A PLACE WHRE THE SAME FIGURE TYPE IS PLACES
			std::vector<FigureFieldPair>::iterator res_it = getFigureInDiffListPlacedOnEmptpy(diff, diff.at(it).field_curr.figure);
			//CHECK IF GOT AN RESULT
			//if(res_it != diff.end())
			//{
			//	move_list.push_back(MovePiar(diff.at(it).field_curr.field, res_it->field_target.field));
			//	diff.at(it).processed = true;
			//	res_it->processed = true;
			//	continue;
			//}
		}
	
	}

	//FINALLY EXECUTE MOVES
	for(size_t i = 0 ; i < move_list.size() ; i++)
	{
		makeMoveSync(move_list.at(i), false, false, false);
	}

	if (move_list.size() > 0)
	{
		return true;
	}
	return false;
}

bool ChessBoard::syncRealWithTargetBoard_add_remove_empty() {

	//USE MAKE MOVE TO GENERATE A MOVE LIST THEN EXECUTE THE MOVES
	 std::vector<ChessBoard::MovePiar> move_list;
	ChessPiece::FIGURE*board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
	ChessPiece::FIGURE*board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);
	
	if (!board_current || !board_target)
	{
		return false;
	}
	
	//LAMBDA FUNCTION FOR GETTING THE NEXT FIGURE THATS WITH THE SAME TYPE WHICH WAS ALSO MODIFIED
	std::vector<FigureFieldPair>::iterator gfidlp_it;
	//GET THE DIFFERNCE BETWEEN THE TWO BOARDS AS LIST
	std::vector<FigureFieldPair> diff = compareBoards(board_current, board_target, true);
	
	if (diff.size() == 0)
	{
		return false;
	}
	//FIND THREORETICAL MATCHES
	
	//FIND ICH EIN FELD BEI DER DAS CURRENT FELD = DEM TARGET FELD IST ?
	std::vector<MovePiar> diff_matched;
	
	for (size_t it = 0; it < diff.size(); it++) {
		if (diff.at(it).processed)
		{
			it++;
			continue;
		}
		
		
		
		
		
		
		
		//
		//1. FIELDS ARE NOT EMPTY
		if(!diff.at(it).field_curr.figure.is_empty && !diff.at(it).field_target.figure.is_empty)
		{
			//2. FIELDS ARE THE SAME => SO FIGURE HAS CHANGES ON FIELD
		//	if(diff.at(it).field_curr.field == diff.at(it).field_target.field) {
				//TARGET FIGURE IS THE CURRENT FIGURE OF THE FIELD
				if(!ChessPiece::compareFigures(diff.at(it).field_curr.figure, diff.at(it).field_target.figure)) {
						
					//THEN WE HAVE TO FIND A FIELD WHERE THE FIGURE ON it->field_curr
					//HAS A OTHER FIELD PAIR IF itd->field_TARGET->FIGURE && itd->field_current->empty
					//THEN THE TARGET FIELD FIGURE HAVE TO BE REMOVED FROM THE FIELD
					int found = -1;
					for (size_t itd = 0; itd < diff.size(); itd++) {
						if (diff.at(itd).field_curr.figure.is_empty &&  !ChessPiece::compareFigures(diff.at(it).field_curr.figure, diff.at(itd).field_target.figure)) {
							found = itd;
							break; //FOUND A FIGURE
						}
					}
					//IF A OTHER FIELD IS FOUND WITH THE FIGURE TO REMOVE => CREATE A MOVE
					if(found >= 0 && found < diff.size())
					{
							
						move_list.push_back(MovePiar(diff.at(it).field_curr.field, diff.at(found).field_target.field));
						diff.at(it).processed = true;
						break;
					}else //ELSE MOVE FIGURE TO PARKING SLOT
					{
						int fdIndex = get_next_free_park_position(ChessBoard::BOARD_TPYE::REAL_BOARD, diff.at(it).field_curr.figure);
						if (fdIndex >= 0)
						{
							move_list.push_back(MovePiar(diff.at(it).field_curr.field, ChessField::Index2Field(fdIndex)));
							diff.at(it).processed = true;
							break;
						}
					}			
				//}
			}
		}
		
		
		//ONE TARGET FIELD IS NOT EMPTY AND ONE OTHER FIELD THE CURRENT FIELD IS NOT EMPTY
		//AND THE FIGURE ARE THE SAME
		if(diff.at(it).field_curr.figure.is_empty && !diff.at(it).field_target.figure.is_empty && !ChessPiece::compareFigures(diff.at(it).field_curr.figure, diff.at(it).field_target.figure))
		{
			
			int found = -1;
			for (size_t itd = 0; itd < diff.size(); itd++) {
				if (!diff.at(itd).field_curr.figure.is_empty &&  diff.at(itd).field_target.figure.is_empty && ChessPiece::compareFigures(diff.at(it).field_target.figure, diff.at(itd).field_curr.figure)) {
					found = itd;
					break; //FOUND A FIGURE
				}
			}
			if (found >= 0 && found < diff.size())
			{
				move_list.push_back(MovePiar(diff.at(found).field_curr.field, diff.at(it).field_target.field));
				diff.at(it).processed = true;
				break;
			}
		}
		
		//MOVE FIGURE OUT OF THE FIELD
		if(!diff.at(it).field_curr.figure.is_empty && diff.at(it).field_target.figure.is_empty)
		{
			int fdIndex = get_next_free_park_position(ChessBoard::BOARD_TPYE::REAL_BOARD, diff.at(it).field_curr.figure);
			if (fdIndex >= 0)
			{
				move_list.push_back(MovePiar(diff.at(it).field_curr.field, ChessField::Index2Field(fdIndex)));
				diff.at(it).processed = true;
				break;
			}
		}
		//MOVE FIGURE INTO THE FIELD
		if(diff.at(it).field_curr.figure.is_empty && !diff.at(it).field_target.figure.is_empty)
		{
			std::vector<ChessField::CHESS_FILEDS> fields = get_parking_fileld_occupied_by_figure_type(board_current, diff.at(it).field_target.figure);
			if (fields.size() > 0)
			{
				move_list.push_back(MovePiar(fields.at(0), diff.at(it).field_target.field));
				diff.at(it).processed = true;
				break;
			}
		}
	}
	
	volatile bool move_made = false;
	if (move_list.size() > 0)
	{
		move_made = true;
	}
	//FINALLY EXECUTE MOVES
	for(size_t i = 0 ; i < move_list.size() ; i++)
	{
		makeMoveSync(move_list.at(i), false, false, false);
	}
	return move_made;
}

	

std::vector<ChessField::CHESS_FILEDS> ChessBoard::get_free_fields_on_the_board(ChessPiece::FIGURE* _board_pointer)
{
	std::vector<ChessField::CHESS_FILEDS> tmp;
	if (_board_pointer == nullptr)
	{
		return tmp;
	}
	
	for (size_t h = 0; h < BOARD_HEIGHT; h++) {
		
		for (size_t w = 2; w < 10; w++) {
			size_t index = w + (h*BOARD_WIDTH);
			ChessPiece::FIGURE tmp_fig = _board_pointer[index];
			if (tmp_fig.is_empty)
			{
				tmp.push_back(ChessField::get_field_from_board_index(index));
				
			}
		}
	}
	
	return tmp;
}



std::vector<ChessField::CHESS_FILEDS> ChessBoard::get_parking_fileld_occupied_by_figure_type(ChessPiece::FIGURE* _board_pointer, ChessPiece::FIGURE _fig)
{
	std::vector<ChessField::CHESS_FILEDS> res;
	if (_board_pointer == nullptr)
	{
		return res;
	}
	
	
	
	//DETERM SITE OFFSET
	int side_offset_start = -1;
	int side_offset_end = -1;
	if (_fig.color == ChessPiece::COLOR_WHITE) {
		side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1);
		side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);      
	}
	else if (_fig.color == ChessPiece::COLOR_BLACK) {
		side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);
		side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END);      
	}
	else {
		return res;
	}
		
	
	
	//NOW CHECK FIELDS FOR NEXT FREE SPACE	
	int counter = 0;
	for (int i = side_offset_start; i < side_offset_end; i++)
	{
		
		ChessField::CHESS_FILEDS field = ChessField::Index2Field(i);
		ChessPiece::FIGURE fig = getFigureOnField(_board_pointer, field);
		
		//CHECK IF FIELD EMPTY AND RIGHT TYPE
		if(!fig.is_empty && getSpecificFigureTypeForParkpostion(field) == _fig.type)
		{
			res.push_back(field);
		}
			
			
	}
		
	return res; 
}

std::vector<ChessField::CHESS_FILEDS> ChessBoard::get_chess_fields_occupied_from_figure(ChessPiece::FIGURE* _board_pointer, ChessPiece::FIGURE _fig, bool _board_only)
{
	
	std::vector<ChessField::CHESS_FILEDS> tmp;
	
	if (_board_pointer == nullptr || !ChessPiece::IsFigureValid(_fig))
	{
		return tmp;
	}
		
	int w_offset = 0;
	if (_board_only)
	{
		w_offset = 2;       //BOARD WIDTH - 2*2 FOR PARKING
	}
	
	for (size_t w = 0 + w_offset; w < BOARD_WIDTH - w_offset; w++) {
		for (size_t h = 0; h < BOARD_HEIGHT; h++) {
			size_t index = w + (h*BOARD_WIDTH);
			ChessPiece::FIGURE tmp_fig = _board_pointer[index];
			
			
			//CHECK IF EQUAL FIGURES
			if(ChessPiece::compareFigures(_fig, tmp_fig))
			{
				//ADD TO THE LIST
				tmp.push_back(ChessField::get_field_from_board_index(index));
			}
			
		}		 
	}
	
	
	return tmp;

}





ChessPiece::TYPE ChessBoard::getSpecificFigureTypeForParkpostion(ChessField::CHESS_FILEDS _field)
{
	
	//DETERM HERE THE TYPE OF FIGURE FOR WHICH PARK POS
	const int TYPE_SIZE = 16;    //IS THE NUMBER OF PARK POSITIONS
	const  ChessPiece::TYPE type_offset[16] = { ChessPiece::TYPE::TYPE_BISHOP, ChessPiece::TYPE::TYPE_BISHOP, ChessPiece::TYPE::TYPE_KING, ChessPiece::TYPE::TYPE_QUEEN, ChessPiece::TYPE::TYPE_KNIGHT, ChessPiece::TYPE::TYPE_KNIGHT, ChessPiece::TYPE::TYPE_ROOK, ChessPiece::TYPE::TYPE_ROOK, ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN };
	
	
	if (!isFieldParkPosition(_field))
	{
		return ChessPiece::TYPE::TYPE_INVALID;
	}
	
	int res = ChessField::parkpos2IDIndex(_field);
	if (res < 0)
	{
		return ChessPiece::TYPE::TYPE_INVALID;
	}
	
	return type_offset[res];

}

int ChessBoard::get_next_free_park_position(ChessBoard::BOARD_TPYE _target_board, ChessPiece::FIGURE _fig)
{

	ChessPiece::FIGURE*cb = get_board_pointer(_target_board);
	if (cb == nullptr)
	{
		return -1;
	}
	
	
	
	//DETERM SITE OFFSET
	int side_offset_start = -1;
	int side_offset_end = -1;
	if (_fig.color == ChessPiece::COLOR_WHITE) {
		side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1);
		side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);      
	}
	else if (_fig.color == ChessPiece::COLOR_BLACK) {
		side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);
		side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END);      
	}
	else {
		return -1;
	}
		
	
	
	//NOW CHECK FIELDS FOR NEXT FREE SPACE	
	int counter = 0;
	for (int i = side_offset_start; i < side_offset_end; i++)
	{
		
		ChessField::CHESS_FILEDS field = ChessField::Index2Field(i);
		ChessPiece::FIGURE fig = getFigureOnField(cb, field);
		
		//CHECK IF FIELD EMPTY AND RIGHT TYPE
		if(fig.is_empty && getSpecificFigureTypeForParkpostion(field) == _fig.type)
		{
			return i;
		}
			
			
	}
		
	return -1; //RETURN INVALID
}


ChessPiece::FIGURE* ChessBoard::get_board_pointer(ChessBoard::BOARD_TPYE _target_board)
{
	if (_target_board == ChessBoard::BOARD_TPYE::TARGET_BOARD)
	{
		return board_target;
	}
	else if (_target_board == ChessBoard::BOARD_TPYE::REAL_BOARD)
	{
		return board_current;
	}
	else if (_target_board == ChessBoard::BOARD_TPYE::TEMP_BOARD)
	{
		return board_temp;
	}
	else
	{
		return nullptr;//INVALID SELECTD BOARD
	}
}


int ChessBoard::get_figure_type_count(ChessBoard::BOARD_TPYE _target_board, char _type_char, bool _board_only)
{
	return get_figure_type_count(get_board_pointer(_target_board), _type_char, _board_only);

}

int ChessBoard::get_figure_type_count(ChessPiece::FIGURE* _board_pointer, char _type_char, bool _board_only) {
	if (_board_pointer == nullptr)
	{
		return -1;
	}
	
	
	int count = 0;
	int w_offset = 0;
	if (_board_only)
	{
		w_offset = 2;      //BOARD WIDTH - 2*2 FOR PARKING
	}
		
		
	for (size_t w = 0 + w_offset; w < BOARD_WIDTH - w_offset; w++) {
		for (size_t h = 0; h < BOARD_HEIGHT; h++) {
			size_t index = w + (h*BOARD_WIDTH);
			ChessPiece::FIGURE tmp_fig = _board_pointer[index];

			//CHECK IF EQUAL FIGURES
			if(ChessPiece::Figure2Charakter(tmp_fig) == _type_char && ChessPiece::IsFigureValid(tmp_fig))
			{
				count++;
			}
		}		 
	}
	
	
	return count;
}

bool ChessBoard::boardFromFen(std::string _fen, ChessBoard::BOARD_TPYE _target_board) {
	
	if (_fen.empty())
	{
		return false;
	}
		
	_fen += " ";      //THE BOARD BLOCK (CONTAINING FROM START OF THE STRING) TO THE FIRST WHITESPACE => EVERYTHING AFTER IS NOT IMPORTANT
	//CREATE AN EMPTY FIGURE
	
	ChessPiece::FIGURE empty_firegure;
	empty_firegure.is_empty = true;
	empty_firegure.figure_number = -1;
	empty_firegure.color = ChessPiece::COLOR_UNKNOWN;
	empty_firegure.type = ChessPiece::TYPE_INVALID;
	
	ChessPiece::FIGURE*cb = get_board_pointer(_target_board);
	if (cb == nullptr)
	{
		return false;
	}
	
	
	//WE NEED COUNTER FOR EACH TYPE OF FIGURE PLACED IN THE BOARD
	//TO DETERM THE AMOUNT OF GIRES IN THE PARKING SLOTS
	// INIT WIHT -1 INDICATED THAT NO FIGURE OF THIS TYPE IS SET
	std::map<char, int> figure_counter;
	//BLACK
	figure_counter['r'] = -1;
	figure_counter['n'] = -1;
	figure_counter['b'] = -1;
	figure_counter['q'] = -1;
	figure_counter['k'] = -1;
	figure_counter['p'] = -1;
	
	//WHITE
	figure_counter['R'] = -1;
	figure_counter['N'] = -1;
	figure_counter['B'] = -1;
	figure_counter['Q'] = -1;
	figure_counter['K'] = -1;
	figure_counter['P'] = -1;
	
	
	
	//CLEAR TARGET BOARD
	for(size_t h = 0 ; h < BOARD_HEIGHT ; h++) {
		for (size_t w = 0; w < BOARD_WIDTH; w++) {
			size_t index = w + (h*BOARD_WIDTH);
			cb[index] = empty_firegure;
		}		 
	}
	//	printBoard(_target_board);
		//ALGORITHM PARTLY MODIFIED FRO  POST 12550 SITE:
		//http://www.cplusplus.com/forum/general/181525/
		//CHECK FEN STRING
		int num_spaces = 0;
	int num_rows = 1;
	//FOR EACH CHARACTER IN FEN STRING
	
	for(const char *p =  _fen.c_str() ; *p != 0 ; ++p) {
		if (*p == ' ') {
			++num_spaces;
		}
		else if (*p == '/') {
			++num_rows; 
		}
		
	}
	//INVALID FEN
	if(num_rows > 8) //DISBALE SPACE CHECK => ONLY USED FOR EXTENDED FEN STRINGS num_spaces != 5 || 
	{			
		return false;
	}
	

	
	
	int board_position_x = 0;      //WIDTH
	int board_position_y = 0;      //HEIGHT
	const char *p =  _fen.c_str();
	for (; *p != ' '; ++p)
	{
		const char ch = *p;

		if (isdigit(ch)) {
			//IS A DIGIT => EMPTY SPACE FOR THE VALUE OF THE DIGIT
		   for(int i = 0 ; i < (ch - '0') ; ++i) {
				board_position_x++;      //STEP OVER FIELD	
			}
		}
		else if (ch == '/') {
			//NEW ROW
		   printf("\n\n");
			board_position_y++;      //GOTO NEXT ROW
			board_position_x = 0;       //AND STARTING A NEW LINE WITH X COUNTER AT 0
		}
		else if ((ch > 65 && ch < 90) || (ch > 97 && ch < 122)) {
			//IS A FIGURE 
			printf(" %c", ch); 
			ChessPiece::FIGURE tmp = ChessPiece::getFigureByCharakter(ch);
			//CHECK FIGURE IS VLAID
			if(!ChessPiece::IsFigureValid(tmp))
			{
				continue;
			}
				
				
			//ASSIGN FIGURE NUMBER
			figure_counter[ch]++;
			tmp.figure_number = figure_counter[ch];
			
			//PLACE THE FIGURE
			size_t index = board_position_x + (board_position_y*BOARD_WIDTH) + 2;      // + (board_position_y*BOARD_WIDTH);   //+2 FOR THE 2 SPACE FOR THE PARKING POSITIONS
			
			ChessField::CHESS_FILEDS field = ChessField::get_field_from_board_index(index);
			int bindex = ChessField::field2Index(field);
			
			if (bindex == 72)
			{
				int b = 0;
			}
				
			cb[bindex] = tmp;
			//STEP TO NEXT FIELD
			board_position_x++;  
#ifdef DEBUG
			//printBoard(_target_board);	  
			
#endif // DEBUG

			
		}
			
	}

	
	//printBoard(_target_board);
	
	//DETERMN FIGURES IN PARK POSITION
	const size_t FIGURE_COUNT = 12;
	const char FIGURES[FIGURE_COUNT] = { 'r', 'n', 'b', 'q', 'k', 'p', 'R', 'N', 'B', 'Q', 'K', 'P' };
	int park_pos_counter_white = 0;
	int park_pos_counter_black = 0;
	
	//FOR EACH FIGURE
	for(int i = 0 ; i < FIGURE_COUNT ; i++)
	{
		//GET TARGET COUNT
		const int target_count = ChessPiece::getFigureCountByChrakter(FIGURES[i]);
		const int current_count = figure_counter[FIGURES[i]] + 1;
		const int missing_count = llabs(current_count - target_count);
		//IF TARGET COUNT REACHED
		if(missing_count <= 0)
		{
			continue;
		}
		//ELSE FILL UP
		for(int curr_missing_count = 0 ; curr_missing_count < missing_count ; curr_missing_count++)
		{
			//CREATE FIGURE WITH NUMBER
			ChessPiece::FIGURE tmp = ChessPiece::getFigureByCharakter(FIGURES[i]);
			figure_counter[FIGURES[i]]++;
			tmp.figure_number = figure_counter[FIGURES[i]];
			//CHECK FIGURE IS VLAID
			if(!ChessPiece::IsFigureValid(tmp))
			{
				return false;
			}
			
			int pos_index = get_next_free_park_position(_target_board, tmp);
			//INVALID POSITION
			if(pos_index < 0)
			{
				continue;
			}
			//FINALLY PLACE FIGURE
			cb[pos_index] = tmp;
			//printBoard(_target_board);
		}
			
	}
	
	printBoard(_target_board);
	return true;
}
	


void ChessBoard::printBoard(ChessBoard::BOARD_TPYE _target_board)
{
	ChessPiece::FIGURE*cb;
	
	if (_target_board == ChessBoard::BOARD_TPYE::TARGET_BOARD)
	{
		cb = board_target;
		std::cout << "----- BOARD - TARGET -----------" << std::endl;
		
	}
	else if (_target_board == ChessBoard::BOARD_TPYE::REAL_BOARD)
	{
		cb = board_current;
		std::cout << "----- BOARD - REAL -------------" << std::endl;
		
	}
	else if (_target_board == ChessBoard::BOARD_TPYE::TEMP_BOARD)
	{
		cb = board_temp;
		std::cout << "----- BOARD - TEMP -------------" << std::endl;
		
	}
	else
	{
		return;
	}
	std::cout << "x = INVALID -----. = EMPTY FIELD" << std::endl;
	
	std::cout << "  W W | A B C D E F G H | B B " << std::endl;
	for (size_t h = 0; h < BOARD_HEIGHT; h++) {
		std::cout << (BOARD_HEIGHT - h) << " ";
		for (size_t w = 0; w < BOARD_WIDTH; w++) {
			
			const ChessField::CHESS_FILEDS f = ChessField::get_field_from_board_index(w + (h*BOARD_WIDTH));
			const int index = ChessField::field2Index(f);
			ChessPiece::FIGURE fig = getFigureOnField(cb, f);
			if (w == 2 || w == 10)
			{
				std::cout << "| ";
			}
				
			//size_t index = w + (h*BOARD_WIDTH);
			//PRINT BLANK IF FIGURE IS INVALID
			if(fig.is_empty)
			{
				std::cout << "." << " ";
				continue;
			}
			//PRINT X FOR AN INVALID FIGURE
			if(!ChessPiece::IsFigureValid(fig))
			{
				std::cout << "x" << " ";
				continue;
			}
			//ELSE PRINT THE CHARAKTER
			std::cout << ChessPiece::Figure2Charakter(fig) << " ";
		}	
		std::cout << std::endl;
	}
	std::cout << "----- END BOARD -------------------" << std::endl;
}


void ChessBoard::log_error(std::string _err)
{
#ifdef USE_STD_LOG
	std::cout << _err << std::endl;
#endif //USE_STD_LOG
	//	LOG_F(ERROR, "%s", _err.c_str());
}

void ChessBoard::getParkPositionCoordinates(ChessField::CHESS_FILEDS _index, int& _x, int& _y, IOController::COIL _coil, bool before_parkpostion_entry)
{
	//INDEX IS NOT A PARK POSITION
	if(!isFieldParkPosition(_index))
	{
		_x = -1;
		_y = -1;
		return;
	}
	//DETERM IF PARKING POSITION IS BLACK OR WHITE SLOT
	int is_black_park_position = 0;
	int field_index = static_cast<int>(_index);
	if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1) && field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_16)) {
		is_black_park_position = 1;
	}
	else if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1) && field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_16)) {
		is_black_park_position = 2;
	}
	
	if (is_black_park_position == 0) //NOT WHITE OR BLACK
		{
			return; //ERROR
		}
		
	//NOW LOAD THE OFFSETS OF THE PARKPOSTION WHITE OR BLACK SITE
	int line_offset = 0;
	int cell_size = 0;
	int y_offset = 0;
	int before_line_offset = 0;
	
	if (is_black_park_position == 2) //BLACK
		{
			ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE, line_offset);
			ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET, y_offset);
		}
	else
	{
		ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE, line_offset);
		ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET, y_offset);
	}
		
		
	//LOAD PARKING SLOT CELL SIZE
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE, cell_size);
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET, before_line_offset);
	
	
	//GET PARK POSITON INDEX 1-16
	int pp_entry = 0;
	if (is_black_park_position == 2) //BLACK
		{
			pp_entry = (magic_enum::enum_integer(_index) - magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1)) % 16;
		}
	else
	{
		pp_entry = (magic_enum::enum_integer(_index) - magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1)) % 16;
	}
	
	if (before_parkpostion_entry)
	{
		if (is_black_park_position == 2) //BLACK
			{
				_x = line_offset + before_line_offset;
			}
		else
		{
			_x = line_offset - before_line_offset;
		}
			
			
		_y = (pp_entry*cell_size) + y_offset;
	}
	else
	{
		_x = line_offset;
		_y = (pp_entry*cell_size) + y_offset;
	}
	
	//TODO -> lade offsets
	//hole seite
	//
}
//CONVERTS FIELD ID TO THE X Y  COORDINATES 0-7 0-7
void ChessBoard::getFieldCoordinates(ChessField::CHESS_FILEDS  _index, int& _x, int& _y, IOController::COIL _coil, bool _get_only_array_index, bool _get_field_center)
{
	
	//TODO CHECK PARK POSTION
	if(isFieldParkPosition(_index))
	{
		_x = -1;
		_y = -1;
		return;
	}
	
	
	int offset_x = 0;
	int offset_y = 0;
	//LOAD OFFSET FROM CONFIG FILE IF HOME POS IS NOT IN FIELD H1
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X, offset_x);
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y, offset_y);
	int board_width = 0;
	//LOAD OFFSET FROM CONFIG FILE IF HOME POS IS NOT IN FIELD H1
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_BOARD_WIDTH, board_width);
	int field_width = 50;
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH, field_width);
	
	
	
	//GET COIL OFFSET
	int coil_offset_x = 0;
	int coil_offset_y = 0;
	get_coil_offset(_coil, coil_offset_x, coil_offset_y);
	
	
	
	///CONVERT TO XY WITH A NORMAL CHESS FIELD WITH 8 by 8 FIELDS
	int field_index = static_cast<int>(_index);
	if (_get_field_center)
	{
		
		
	}
	else
	{
		offset_x += field_width / 2;
		offset_y += field_width / 2;
	}
	
	
	if (_get_only_array_index) {
		_x = (field_index / 8);
		_y = (field_index % 8);
	}
	else
	{
		_x = (board_width - ((field_index / 8)*field_width) - offset_x) - coil_offset_x;
		_y = (((field_index % 8)*field_width) + offset_y) + coil_offset_y;
	}
	

}
	

ChessBoard::BOARD_ERROR ChessBoard::scanBoard(bool _include_park_postion)
{
	//SAVE CURRENT FIELD	
	ChessField::CHESS_FILEDS original_position = current_field;
	IOController::COIL original_coil = current_selected_coil;
	
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
	
	int x = 0;
	int y = 0;
	for (int i = 0; i < magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1); i++)
	{
		//getFieldCoordinates((ChessField::CHESS_FILEDS)i, x, y, IOController::COIL::COIL_NFC, true, true);        //GET INDEX FOR ARRAY
		travelToField(static_cast<ChessField::CHESS_FILEDS>(i), IOController::COIL::COIL_NFC, true);             //TRAVEL TO NEXT FIELD
		ChessPiece::FIGURE tmop = HardwareInterface::getInstance()->ScanNFC(10);             //SCAN NFC TAG IF PRESENT
		ChessPiece::FigureDebugPrint(tmop);            //DEBUG PRINT FIGURE IF FOUND
		if(tmop.type == ChessPiece::TYPE::TYPE_INVALID) {
			tmop.is_empty = true;	
		}else
		{
			tmop.is_empty = false;
		}
		
		board_current[ChessField::get_board_index_from_field(static_cast<ChessField::CHESS_FILEDS>(i))] = tmop;                   //STORE FIGURE ON BOARD
	}
	
	//TODO SCAN PARK POSITION FOR WHITE
	for(int i = magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1) ; i < magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_16) ; i++)
	{
		
		//GOOT
		//getFieldCoordinates((ChessField::CHESS_FILEDS)i, x, y, IOController::COIL::COIL_NFC, true, true);   //GET INDEX FOR ARRAY
		travelToField(static_cast<ChessField::CHESS_FILEDS>(i), IOController::COIL::COIL_NFC, true);              //TRAVEL TO NEXT FIELD
		//ACTIVATE COIL FOR THE BLACK SITE COIL A IS NESSESSARY
		HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
		//MOVE OUT SLOW
		//MOVE NFC
		
		//SCAN POSSBILE NFC TAG
		ChessPiece::FIGURE tmop = HardwareInterface::getInstance()->ScanNFC(10);              //SCAN NFC TAG IF PRESENT
		ChessPiece::FigureDebugPrint(tmop);             //DEBUG PRINT FIGURE IF FOUND
		if(tmop.type == ChessPiece::TYPE::TYPE_INVALID) {
			tmop.is_empty = true;	
		}
		//ACTIVTE COIL A AGAIN
		HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
		//MOVE IN
		//DEACTIVATE COIL
		HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	}
	
	
	//TRAVEL BACK TO ORIGINAL STARTING POSITION
	travelToField(original_position, original_coil, true);
	
	return ChessBoard::BOARD_ERROR::NO_ERROR;
}
	

bool ChessBoard::isFieldParkPosition(ChessField::CHESS_FILEDS _field)
{	
	int field_index = static_cast<int>(_field);            //CAST FIELD ENUM TO A INDEX/NUMBER
	if(field_index < 64) //ALL PARKPOSITIONS ARE INDEX >=64
	{
		return false;
	}
	else
	{
		return true;
	}	
}
	
std::list<ChessPiece::FIGURE> checkBoardForFullFigureSet(ChessPiece::FIGURE(&board)[BOARD_WIDTH][BOARD_HEIGHT])
{
	//list missing or to much figures
}

IOController::COIL ChessBoard::getValidCoilTypeParkPosition(ChessField::CHESS_FILEDS _field, IOController::COIL _target)
{
	
	//if (!isFieldParkPosition(_field))
//	{
//		return _target;
//	}
	
	if(_target == IOController::COIL::COIL_NFC)
	{
		return _target;
	}
	
	//----- THIS CODE IS FOR THE PROD HARDWARE WITH ONLY ONE COIL ON POSITION A --------- //	
	if(HardwareInterface::getInstance()->is_production_hardware())
	{
		return IOController::COIL::COIL_A;
	}
	
	
	
	//----- THIS CODE IS FOR THE DK HARDWARE WITH TWO COILS TO SWITCH BETWEEN THEM --------- //	
	bool is_black_park_position = true;
	int field_index = static_cast<int>(_field);
	
	
	if (isFieldParkPosition(_field))
	{
		if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1) && field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_16))
		{
			return IOController::COIL::COIL_B;
		}
		else
		{
			return IOController::COIL::COIL_A;	
		}	
	}
	else
	{
		if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_A1) && field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_D8))
		{
			return IOController::COIL::COIL_B;
		}
		else
		{
			return IOController::COIL::COIL_A;	
		}
	}
	
	
	
	
}
	
ChessBoard::BOARD_ERROR ChessBoard::travelToField(ChessField::CHESS_FILEDS _field, IOController::COIL _coil, bool _to_field_center)
{
	
	//SHUT COILS OFF
//	iocontroller->setCoilState(IOController::COIL_A, false);
//	iocontroller->setCoilState(IOController::COIL_B, false);
	//SETUP_MOTORS
	HardwareInterface::getInstance()->set_speed_preset(HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
	
	
	//GET FIELD COORDIANTES
	int field_coordinates_x = -1;
	int field_coordinates_y = -1;
	//GET FIELD TYPE NORMAL BOARD FIELD OR PARK POSITION 
	if(!isFieldParkPosition(_field))
	{
		getFieldCoordinates(_field, field_coordinates_x, field_coordinates_y, _coil, false, _to_field_center);
	}
	else
	{
		getParkPositionCoordinates(_field, field_coordinates_x, field_coordinates_y, _coil, _to_field_center);
	}
	//CONVERSION ERROR HANDLING
	if(field_coordinates_x == -1 && field_coordinates_y == -1)
	{
		return ChessBoard::BOARD_ERROR::AXIS_TRAGET_ARRIVAL_FAILED;
	}
	//FINALLY MOVE TO POSITION
	/// NON BLOCKING BECUASE WE WANT TO MOVE SIMULATNIOUSLY
	HardwareInterface::getInstance()->move_to_postion_mm_absolute(field_coordinates_x, field_coordinates_y, false);

	//WAIT FOR MOTORS TO REACH POSTION
	int wait_counter = 0;
	while (!(HardwareInterface::getInstance()->is_target_position_reached())) {
		std::this_thread::sleep_for(std::chrono::milliseconds(WAITITME_FOR_MOTORS_TO_ARRIVE));
		wait_counter++;
		if (wait_counter > WAITIME_MULTIPLIER_AXIS_ARRIVAL)
		{
			break;
		}
	}
	//DISBALE MOTOR
	HardwareInterface::getInstance()->disable_motors();

	//CHECK IF MOTORS ARRIVED TARGET POSITION WITH NO ERROR
 	if(wait_counter > WAITIME_MULTIPLIER_AXIS_ARRIVAL)
	{
		log_error("ChessBoard::BOARD_ERROR::AXIS_TRAGET_ARRIVAL_FAILED");
		return ChessBoard::BOARD_ERROR::AXIS_TRAGET_ARRIVAL_FAILED;
	}	
	//SAVE CURRENT FIELD POSITION
	current_field = _field;
	return ChessBoard::BOARD_ERROR::NO_ERROR;
}


ChessBoard::BOARD_ERROR ChessBoard::get_coil_offset(IOController::COIL _coil, int& _x, int& _y)
{
	//LOAD COIL DISTANCE COIL_A <--- NFC ---> COIL_B THE DISTANCE IS COIL_A to COIL_B THE NFC COIL IS IN THE MIDDLE
	int coil_distance = 0;
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM, coil_distance);
	
	int mv_distance = 0;
	//coil_distance = coil_distance / 2;
	///IF SWITCHED TO B COIL THE TRAVEL DISTANCE IS NEGATIVE
	if(_coil == IOController::COIL::COIL_A)
	{
		mv_distance = -std::abs(coil_distance / 2);
	}else if(_coil == IOController::COIL::COIL_B)
	{
		mv_distance = std::abs(coil_distance / 2);
	}else if(_coil == IOController::COIL::COIL_NFC)
	{
		//	mv_distance = std::abs(coil_distance / 2);
	}

	_y = 0;            //THE COILS ARE ONLY MOVING IN X DIRECTION
	_x = mv_distance;
}
	



ChessBoard::BOARD_ERROR ChessBoard::initBoard(bool _with_scan)
{
	//CHECK HARDWARE INIT
	if(!HardwareInterface::getInstance()->check_hw_init_complete())
	{
		log_error("ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION");
		return ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION;
	}

	
	
	HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
	//FIRST DO HOMEING OF THE AXIS
	home_board();
	
	//TRAVEL TO THE "HOME FIELD H1" AND SWITCH ALL COILS OFF
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	
	
	
#ifdef DEBUG
	//MOVE TO FIELD H1 WITH COIL A => THAT THE VISIBLE REF POSITION FOR VISUAL INSPRECTION
	travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_H1, IOController::COIL_A, true);
#endif // DEBUG	
	
	
	//INIT BOARD ARRAYS => TO EMPY FIELDS
	initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD));
	initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD));
	initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TEMP_BOARD));
	
	
	//LOAD PRESETS
	loadBoardPreset(ChessBoard::BOARD_TPYE::TARGET_BOARD, ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
	loadBoardPreset(ChessBoard::BOARD_TPYE::REAL_BOARD, ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
	
	
	
	
	
	//NEXT SCAN THE FIELD WITH PARK POSTIONS
	if(_with_scan)
	{
		scanBoard(false);
		printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);
	}else
	{
	}
		
	//syncRealWithTargetBoard(StringToMovePair("f2h4"));
	//syncRealWithTargetBoard(StringToMovePair("g2h4"));
	
	
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
	//TODO ONLY OCCUPED FIELDS
	/*
	for (int i = 0; i < magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1); i++)
	{
		//getFieldCoordinates((ChessField::CHESS_FILEDS)i, x, y, IOController::COIL::COIL_NFC, true, true);        //GET INDEX FOR ARRAY
		HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
		HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
		travelToField(static_cast<ChessField::CHESS_FILEDS>(i), IOController::COIL::COIL_NFC, true);             //TRAVEL TO NEXT FIELD
		HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	*/
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
	
	//syncRealWithTargetBoard();
	//boardFromFen("r1bqkbnr/pp1ppppp/n7/8/2p5/1QP5/PP1PPPPP/RNB1KBNR", ChessBoard::BOARD_TPYE::REAL_BOARD);    //DAME BAUER	
	//boardFromFen("r1bqkbnr/pp1ppppp/n7/8/8/1pP5/PP1PPPPP/RNB1KBNR", ChessBoard::BOARD_TPYE::TARGET_BOARD); 	//BAUER SCHLAEGT DAME
	//syncRealWithTargetBoard();
	
	//boardFromFen("rn1qkbnr/pp2pppp/2p5/5b2/3PN3/8/PPP2PPP/R1BQKBNR", ChessBoard::BOARD_TPYE::TARGET_BOARD);       //d1c4 c2c3
	//syncRealWithTargetBoard();	
	
	//boardFromFen("rnbqkbnr/ppppp1pp/8/8/5P2/8/PPPPPP1P/RNBQKBNR", ChessBoard::BOARD_TPYE::TARGET_BOARD);  //e2e5
	//syncRealWithTargetBoard();	
	
	//makeMoveSync(ChessField::CHESS_FILEDS::CHESS_FIELD_H1, ChessField::CHESS_FILEDS::CHESS_FIELD_A1, true, false, true);   //WITH SCAN //DIRECTLY //OCCUPY CHECK
//	syncRealWithTargetBoard(StringToMovePair("f2h7"));
	//TODO SCAN FIELD ROUTINE
	
	//MOVE FIGURES TO STANDART START POSTION
			syncRealWithTargetBoard();
	

	
	HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);

	return ChessBoard::BOARD_ERROR::INIT_COMPLETE;
}

ChessBoard::BOARD_ERROR ChessBoard::calibrate_home_pos()
{
	//CHECK HARDWARE INIT
	if(!HardwareInterface::getInstance()->check_hw_init_complete())
	{
		log_error("ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION");
		return ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION;
	}
	//SOME USER NOTIFICATION

	HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);

	//FIRST DO HOMEING OF THE AXIS
	HardwareInterface::getInstance()->home_sync();
	//MOVE TO H1 WITH COILA_ACTIVE
	travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_H1, IOController::COIL_A, true);
	
	
	//ENABLE COIL A
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
	HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);
	return ChessBoard::BOARD_ERROR::NO_ERROR;
}

	
	
void ChessBoard::loadBoardPreset(ChessBoard::BOARD_TPYE _target_board, ChessBoard::BOARD_PRESET _preset)
{
	//CLEAR BOARD
	ChessPiece::FIGURE invlaid_piece;
	invlaid_piece.color = ChessPiece::COLOR_UNKNOWN;
	invlaid_piece.type = ChessPiece::TYPE_INVALID;
	invlaid_piece.unique_id = 0;
	invlaid_piece.figure_number = -1;
	//CLEAR TARGET BOARD
	initBoardArray(get_board_pointer(_target_board));
	//LOAD PRESET NOW
	if(_preset == ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION) {
		//LOAD DEFAULT FEN TO BOARD
	
		std::string tmp = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::BOARD_PRESET_START_POSITION_FEN);
		//IF CONFIG FEN IS EMPTY USE A DEFAULT
		if(tmp.empty())
		{
			tmp = DEFAULT_BOARD_FEN;
		}
		if (!boardFromFen(tmp, _target_board))
		{
			log_error("ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION");
		}
	
		
		
	}
	else if(_preset == ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_PARK_POSITION) //LOAD ALL FIGURES IN PARTKIN POSITION BOARD
	{
		boardFromFen("8/8/8/8/8/8/8/8", _target_board);
		
		
	}
	else if(_preset == ChessBoard::BOARD_PRESET::BOARD_PRESET_NO_FIGURES_PLACED) //LOAD ALL FIGURES IN PARTKIN POSITION BOARD
	{
		boardFromFen("8/8/8/8/8/8/8/8", _target_board);
	}
	
	
}

void ChessBoard::home_board()
{
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
	
	HardwareInterface::getInstance()->home_sync();
}