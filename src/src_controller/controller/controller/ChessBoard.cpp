#include "ChessBoard.h"

ChessBoard::ChessBoard() {

    if (!ConfigParser::getInstance()->configLoaded()) {
        LOG_F(ERROR, "CONFIG NOT LOADED");
    }

    if (!HardwareInterface::getInstance()->check_hw_init_complete()) {
        LOG_F(ERROR, "ChessBoard::ChessBoard HARDWARE INIT NOT COMPLETE LOADED");
    }
    //INIT BOARD ARRAYS => TO EMPY FIELDS
    initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD));
    initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD));
    initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TEMP_BOARD));
}

ChessBoard::~ChessBoard() {
}

void ChessBoard::test_make_move_static() {
    MovePiar tmp_pair;
    HardwareInterface::getInstance()->home_sync();
    tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_G2;
    tmp_pair.to_field = ChessField::CHESS_FILEDS::CHESS_FIELD_A2;
    makeMoveSync(tmp_pair, false, false, false);
}

bool ChessBoard::test_make_move_func() {

    home_board();
    MovePiar tmp_pair;
    tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_H1;
    tmp_pair.to_field = ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1;
    makeMoveSync(tmp_pair, false, false, false);

    tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_E1;
    tmp_pair.to_field = ChessField::CHESS_FILEDS::CHESS_FIELD_A6;
    makeMoveSync(tmp_pair, false, false, false);

    tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1;
    tmp_pair.to_field = ChessField::CHESS_FILEDS::CHESS_FIELD_E1;
    makeMoveSync(tmp_pair, false, false, false);

    tmp_pair.from_field = ChessField::CHESS_FILEDS::CHESS_FIELD_A6;
    tmp_pair.to_field = ChessField::CHESS_FILEDS::CHESS_FIELD_H1;
    makeMoveSync(tmp_pair, false, false, false);

    return true;
}

bool ChessBoard::MoveWaypointsAlong(std::queue<MV_POSITION> &_mv) {

    //LOAD CONFIG FOR MOVEMENT //TODO MOVE TO CONSTRUCTOR
    bool EN_COIL_UPDATE_ALWAYS = ConfigParser::getInstance()->getBool_nocheck(
            ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_MAKE_MOVE);
    bool EN_COILE_ALWAYS_WRITE_OFF = ConfigParser::getInstance()->getBool_nocheck(
            ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_WRITE_OFF);




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
    while (!_mv.empty()) {
        if (EN_COILE_ALWAYS_WRITE_OFF) {
            HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
            HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
        }


        const MV_POSITION tmp = _mv.front();

        //SOME IMPROVEMENTS => SPI WRITING IS EXPENSIVE
        if (EN_COIL_UPDATE_ALWAYS || ca_changed != tmp.coil_a_state) {
            ca_changed = tmp.coil_a_state;
            HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, tmp.coil_a_state);
        }
        if (EN_COIL_UPDATE_ALWAYS || cb_changed != tmp.coil_b_state) {
            cb_changed = tmp.coil_b_state;
            HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, tmp.coil_b_state);
        }


        HardwareInterface::getInstance()->move_to_postion_mm_absolute(tmp.x, tmp.y,
                                                                      false);           //MOVE TO X AND NOT WAIT
        //WAIT FO BOTH MOTORS
        while (!(HardwareInterface::getInstance()->is_target_position_reached())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAITITME_FOR_MOTORS_TO_ARRIVE));
        }
        _mv.pop();
    }
    //DISBALE MOTOR
    HardwareInterface::getInstance()->disable_motors();
    //DISABLE COILS
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);

    return true;
}

//GET NEXT FIELD TO PARK POS
ChessField::CHESS_FILEDS ChessBoard::getNextHalfFieldFromParkPos(ChessField::CHESS_FILEDS _board_field) {

    IOController::COIL parkpos_needed_coil = getValidCoilTypeParkPosition(_board_field, IOController::COIL::COIL_A);
    //IF NOT PARK POS RETURN THE BOARD FIELD COODIANTES
    if (!isFieldParkPosition(_board_field)) {
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
    for (int i = start_index; i < stop_index; i++) {
        //GET FIELD COODIANTES
        getFieldCoordinates(ChessField::Index2Field(i), tmp_x, tmp_y, parkpos_needed_coil, false);
        //USE PYTHAGORAS TO GET THE DISTANCE BEWEEN THE POINTS
        distance = std::hypotl(tmp_x - pp_x, tmp_y - pp_y);
        if (distance < smallest_distance) {
            smallest_distance = distance;
            nearest_field = ChessField::Index2Field(i);
        }
    }


    return nearest_field;
}

bool ChessBoard::makeMoveFromBoardToParkPosition(ChessField::CHESS_FILEDS _park_pos,
                                                 std::queue<MV_POSITION> &_generated_waypoint_list, int _current_x,
                                                 int _current_y) {
    //IF NOT PARK POS RETURN ADD THE FINAL TARGET FIELD POSITION TO THE QUEUE
    if (!isFieldParkPosition(_park_pos)) {
        IOController::COIL parkpos_needed_coil = getValidCoilTypeParkPosition(_park_pos, IOController::COIL::COIL_A);
        getFieldCoordinates(_park_pos, _current_x, _current_y, parkpos_needed_coil, false);
        _generated_waypoint_list.push(
                MV_POSITION(_current_x, _current_y, parkpos_needed_coil == IOController::COIL::COIL_A,
                            parkpos_needed_coil == IOController::COIL::COIL_B));
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
    _generated_waypoint_list.push(
            MV_POSITION(pp_pos_before_x, _current_y, parkpos_needed_coil == IOController::COIL::COIL_A,
                        parkpos_needed_coil == IOController::COIL::COIL_B));
    //	MoveWaypointsAlong(_generated_waypoint_list);
    //MOVE TO PARK POS BEFORE ENTRY
    _generated_waypoint_list.push(
            MV_POSITION(pp_pos_before_x, pp_pos_before_y, parkpos_needed_coil == IOController::COIL::COIL_A,
                        parkpos_needed_coil == IOController::COIL::COIL_B));
    //	MoveWaypointsAlong(_generated_waypoint_list);

    //FINALLY MOVE IN
    _generated_waypoint_list.push(
            MV_POSITION(pp_pos_inside_x, pp_pos_inside_y, parkpos_needed_coil == IOController::COIL::COIL_A,
                        parkpos_needed_coil == IOController::COIL::COIL_B));
    //	MoveWaypointsAlong(_generated_waypoint_list);
    //TURN OFF COILS
    _generated_waypoint_list.push(MV_POSITION(pp_pos_inside_x, pp_pos_inside_y, false, false));
    //	MoveWaypointsAlong(_generated_waypoint_list);
    return true;
}

bool ChessBoard::makeMoveFromParkPositionToBoard(ChessField::CHESS_FILEDS _park_pos, ChessField::CHESS_FILEDS _dest_pos,
                                                 std::queue<MV_POSITION> &_generated_waypoint_list, int &_dest_pos_x,
                                                 int &_dest_pos_y, bool _EN_BOARD_SIZE_WORKAROUND) {
    if (!isFieldParkPosition(_park_pos)) {
        return false;
    }
    //GET FIELD WITDTH /2
    int field_width_y =
            ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y) /
            2;//WANT TO TRAVEL BETWEEN TWO FIELDS

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
    _generated_waypoint_list.push(
            MV_POSITION(pp_pos_inside_x, pp_pos_inside_y, parkpos_needed_coil == IOController::COIL::COIL_A,
                        parkpos_needed_coil == IOController::COIL::COIL_B));
    //MoveWaypointsAlong(_generated_waypoint_list);
    //MOVE OUT THE PARK POS COILS
    _generated_waypoint_list.push(
            MV_POSITION(pp_pos_before_x, pp_pos_before_y, parkpos_needed_coil == IOController::COIL::COIL_A,
                        parkpos_needed_coil == IOController::COIL::COIL_B));
    //MoveWaypointsAlong(_generated_waypoint_list);

    //MOVE THE FIGURE TO THE _target_field y position
    int target_pos_x = 0;
    int target_pos_y = 0;
    getFieldCoordinates(_dest_pos, target_pos_x, target_pos_y, target_coil, false);

    //TARGET POS IS EXACLTY BETWEEN TWO FIELDS
    if (_EN_BOARD_SIZE_WORKAROUND) {
        _generated_waypoint_list.push(MV_POSITION(pp_pos_before_x, target_pos_y - field_width_y,
                                                  parkpos_needed_coil == IOController::COIL::COIL_A,
                                                  parkpos_needed_coil == IOController::COIL::COIL_B));
    } else {
        _generated_waypoint_list.push(MV_POSITION(pp_pos_before_x, target_pos_y + field_width_y,
                                                  parkpos_needed_coil == IOController::COIL::COIL_A,
                                                  parkpos_needed_coil == IOController::COIL::COIL_B));

    }
    //MoveWaypointsAlong(_generated_waypoint_list);
    _dest_pos_x = pp_pos_before_x;
    _dest_pos_y = target_pos_y;

    return true;
}

ChessBoard::BOARD_ERROR
ChessBoard::makeMoveSync(ChessBoard::MovePiar _move, bool _with_scan, bool _directly, bool _occupy_check) {
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
    //IF FIELDS EQUAL NOTHING TODO
    if (_move.to_field == _move.from_field) {
        return ChessBoard::BOARD_ERROR::NO_ERROR;
    }

    int coil_switch_pos = 200;
    if (!ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_COIL_SWITCH_POSTION_TRIGGER,
                                             coil_switch_pos)) {
        coil_switch_pos = 200;
    }


    //------------------- BOARD SIZE LIMIT WORKAROUND ---------------------- //
    bool EN_BSIZE_WORKAROUND = false;
    if (!ConfigParser::getInstance()->getBool(ConfigParser::CFG_ENTRY::MECHANIC_BOARD_SIZE_550MM_WORKAROUND,
                                              EN_BSIZE_WORKAROUND)) {
        EN_BSIZE_WORKAROUND = true;
    }
    //GET FIELD WITDTH /2
    int field_width_x =
            ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X) / 2;
    int field_width_y =
            ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y) / 2;
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
    if (EN_BSIZE_WORKAROUND && (_move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_A8 ||
                                _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_B8 ||
                                _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_C8 ||
                                _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_D8 ||
                                _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_E8 ||
                                _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_F8 ||
                                _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_G8 ||
                                _move.from_field == ChessField::CHESS_FILEDS::CHESS_FIELD_H8)) {
        EN_START_WORKAROUND = true;
        INVERT_FIELD_OFFSET = -1;
    }
    if (EN_BSIZE_WORKAROUND && (_move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_A8 ||
                                _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_B8 ||
                                _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_C8 ||
                                _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_D8 ||
                                _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_E8 ||
                                _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_F8 ||
                                _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_G8 ||
                                _move.to_field == ChessField::CHESS_FILEDS::CHESS_FIELD_H8)) {
        EN_END_WORKAROUND = true;
        INVERT_FIELD_OFFSET_END = -1;
    }
    //
    if (ChessField::is_row_over_row(_move.from_field, _move.to_field)) {
        EN_END_WORKAROUND = true;
        INVERT_FIELD_OFFSET_END = -1;
    }

    //FIST GET THE MOTOR COORINATES
    if (isFieldParkPosition(_move.from_field)) {
        makeMoveFromParkPositionToBoard(_move.from_field, _move.to_field, position_queue, x_start, y_start,
                                        EN_END_WORKAROUND);
        is_start_park_pos = true;
        //------------------- BOARD SIZE LIMIT WORKAROUND ---------------------- //
        if (EN_END_WORKAROUND) {
            EN_START_WORKAROUND = true;
            INVERT_FIELD_OFFSET = -1;
        }

    } else {
        getFieldCoordinates(_move.from_field, x_start, y_start, start_coil, false);
        start_coil = getValidCoilTypeParkPosition(_move.from_field, IOController::COIL::COIL_B);

    }

    if (isFieldParkPosition(_move.to_field)) {
        ChessField::CHESS_FILEDS end_field_tmp = getNextHalfFieldFromParkPos(
                _move.to_field);     // GET NEAREST FIELD NEXT TO PARK POS
        getFieldCoordinates(end_field_tmp, x_end, y_end, end_coil,
                            false);     //CALCULATE NEW COORDINATES FOR THE CHANGED FIELD
        end_coil = getValidCoilTypeParkPosition(end_field_tmp, IOController::COIL::COIL_B);
        is_end_park_pos = true;
    } else {
        getFieldCoordinates(_move.to_field, x_end, y_end, end_coil, false);
    }

    //FIRST TRAVEL TO START POSITON
    if (!is_start_park_pos) {
        position_queue.push(MV_POSITION(x_start, y_start, false, false));
        position_queue.push(MV_POSITION(x_start, y_start, start_coil == IOController::COIL::COIL_A,
                                        start_coil == IOController::COIL::COIL_B));
    }

    //MoveWaypointsAlong(position_queue);
    //SECOND TRAVEL 1/2 FIELD UP OR DOWN IF START WORKAROUND
    position_queue.push(
            MV_POSITION(x_start, y_start + field_width_y * INVERT_FIELD_OFFSET,
                        start_coil == IOController::COIL::COIL_A,
                        start_coil == IOController::COIL::COIL_B));


    //	MoveWaypointsAlong(position_queue);

    //CHECK IF COIL_SWITCH NEEDED
    //INERT A BREAKPOINT BEWEEN FIELD D AND E ON THE SAME Y LINE

    if (start_coil != end_coil) {
        int coil_offset = 0;

        //	MoveWaypointsAlong(position_queue);
        //MOVE TO THE COIL SWITCH POSITION WITH THE START COIL
        if (start_coil == IOController::COIL::COIL_A) {
            coil_offset = coil_switch_pos;
        } else if (start_coil == IOController::COIL::COIL_B) {
            coil_offset = coil_switch_pos - coil_switch_pos / 2;
        }


        position_queue.push(MV_POSITION(coil_offset, y_start + field_width_y * INVERT_FIELD_OFFSET,
                                        start_coil == IOController::COIL::COIL_A,
                                        start_coil == IOController::COIL::COIL_B));



        //MoveWaypointsAlong(position_queue);
        //TURN COILS OFF
        position_queue.push(MV_POSITION(coil_offset, y_start + field_width_y * INVERT_FIELD_OFFSET, false, false));
        //MoveWaypointsAlong(position_queue);
        //GET COORDINATES FOR THE COIL SWITCH
        //=> SWITCH TO B COIL => -coil_offset
        if (start_coil == IOController::COIL::COIL_A && end_coil == IOController::COIL::COIL_B) {
            position_queue.push(
                    MV_POSITION(coil_offset - coil_switch_pos / 2, y_start + field_width_y * INVERT_FIELD_OFFSET, false,
                                false));
            //MoveWaypointsAlong(position_queue);
            position_queue.push(
                    MV_POSITION(coil_offset - coil_switch_pos / 2, y_start + field_width_y * INVERT_FIELD_OFFSET,
                                end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));
        } else if (start_coil == IOController::COIL::COIL_B && end_coil == IOController::COIL::COIL_A) {
            position_queue.push(
                    MV_POSITION(coil_offset + coil_switch_pos / 2, y_start + field_width_y * INVERT_FIELD_OFFSET, false,
                                false));
            //MoveWaypointsAlong(position_queue);
            position_queue.push(
                    MV_POSITION(coil_offset + coil_switch_pos / 2, y_start + field_width_y * INVERT_FIELD_OFFSET,
                                end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));

        }
        //MoveWaypointsAlong(position_queue);

    }

    //MOVE TO TARGET X COORDINATES WITH 1/2 FIELD OFFSET
    position_queue.push(MV_POSITION(x_end + field_width_x, y_start + field_width_y * INVERT_FIELD_OFFSET,
                                    end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));

    //MoveWaypointsAlong(position_queue);
    //MOVE TO TARGET Y COORDINATES WITH 1/2 FIELD OFFSET
    position_queue.push(MV_POSITION(x_end + field_width_x, y_end + field_width_y * INVERT_FIELD_OFFSET_END,
                                    end_coil == IOController::COIL::COIL_A, end_coil == IOController::COIL::COIL_B));

    //MoveWaypointsAlong(position_queue);
    //MOVE TO FIELD CENTER OR FURTHER TO MARK POS
    if (!is_end_park_pos) {
        position_queue.push(MV_POSITION(x_end, y_end, end_coil == IOController::COIL::COIL_A,
                                        end_coil == IOController::COIL::COIL_B));
    } else {
        //TODO MOVE FROM THERE TO FINAL PARK POS
        makeMoveFromBoardToParkPosition(_move.to_field, position_queue, x_end + field_width_x, y_end + field_width_y);
    }


    //FINALLY EXECUTE MOVE
    MoveWaypointsAlong(position_queue);

    //HOME BOARD AFTER MOVE

    if (ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_BOARD_HOME_AFTER_MAKE_MOVE)) {
        home_board();
        //WORKAROUND FOR FIRST DK HARDWARE THAT IS NOT ABLE TO GET PO PARK POSITIONS WITHOUT LOSING STEPS
        //THEN AFTER EACH MOVE THE MECHANIC RUNS A HOME CYCLE TO ENSURE THAT THE MECHANIC IS FOR THE NEXT MOVE IN THE CORRET POSITION
    } else {
        if (!HardwareInterface::getInstance()->is_production_hardware() &&
            (isFieldParkPosition(_move.to_field) || isFieldParkPosition(_move.from_field))) {
            home_board();
        }
    }




    //FINALLY SAVE MADE MOVE INTO THE CURRENT BOARD
    ChessPiece::FIGURE *board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
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

ChessBoard::BOARD_ERROR ChessBoard::makeMoveSyncVirtual(ChessBoard::MovePiar _move) {
    //FINALLY SAVE MADE MOVE INTO THE CURRENT BOARD
    ChessPiece::FIGURE *board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
    if (board_current == nullptr) {
        LOG_F(ERROR, "makeMoveSyncVirtual board_current pointer is empty");
        return ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION;
    }

    const ChessPiece::FIGURE from_fig = getFigureOnField(board_current, _move.from_field);
    const ChessPiece::FIGURE to_fig = getFigureOnField(board_current, _move.to_field);

    printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);

    //CREATE EMPTY FIGURE
    ChessPiece::FIGURE empty_fig;
    empty_fig.color = ChessPiece::COLOR_UNKNOWN;
    empty_fig.type = ChessPiece::TYPE_INVALID;
    empty_fig.is_empty = true;     //!!
    empty_fig.figure_number = -1;

    //THE DESITNATION FIELD IS NOT EMPTY => THE FIGURE
    if (!to_fig.is_empty && !from_fig.is_empty) {

        //THE DESITNATION FIELD IS NOT EMPTY => THE FIGURE
        const int pp_field_index = get_next_free_park_position(ChessBoard::BOARD_TPYE::REAL_BOARD, to_fig);
        board_current[pp_field_index] = to_fig;
    }

    //SET FIGURE FROM FIELD TO THE TO FIELD FIGURE
    const int t_index = ChessField::field2Index(_move.to_field);
    board_current[t_index] = from_fig;
    //THE FROM FIELS IS NOW EMPTY
    const int f_index = ChessField::field2Index(_move.from_field);
    board_current[f_index] = empty_fig;

    printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);
    //const int t_index = ChessField::field2Index(_move.to_field);
    //board_current[t_index] = from_fig;
    //CREATE EMPTY FIGURE

    return ChessBoard::BOARD_ERROR::NO_ERROR;
}

ChessBoard::MovePiar ChessBoard::StringToMovePair(std::string _mv) {
    MovePiar tmp;
    tmp.is_valid = true;
    //CHECK STRING FOR CONTENT AND LENGTH
    if (_mv.empty() || _mv.size() != 4) {
        tmp.is_valid = false;
        return tmp;
    }
    //STRING TO LOWER CASE
    transform(_mv.begin(), _mv.end(), _mv.begin(), ::tolower);

    //CHECK FORMAT BY REGEX
    if (!std::regex_match(_mv, std::regex(STRING_MOVE_REGEX))) {
        tmp.is_valid = false;
        return tmp;
    }

    int fca = _mv.at(0);
    int fcb = _mv.at(2);
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

bool ChessBoard::isBoardFieldOccupied(ChessPiece::FIGURE *_board, ChessField::CHESS_FILEDS _field) {
    if (!_board) {
        return false;
        //TODO LOGGER
    }

    ChessPiece::FIGURE fd = getFigureOnField(_board, _field);
    if (!fd.is_empty) {
        return true;
    }


    return false;
}

ChessPiece::FIGURE ChessBoard::getFigureOnField(ChessPiece::FIGURE *_board, ChessField::CHESS_FILEDS _field) {
    if (!_board) {
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
    if (!fd.is_empty && fd.type == ChessPiece::TYPE_INVALID && fd.color == ChessPiece::COLOR_UNKNOWN) {
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
    if (!_ecevute_move.is_valid) {
        return syncRealWithTargetBoard();

    }


    std::vector<MovePiar> move_list;

    //CHECK IF TARGET FIELD IS OCCUPIED
    ChessPiece::FIGURE *board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
    ChessPiece::FIGURE *board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);

    if (isBoardFieldOccupied(board_current, _ecevute_move.from_field) &&
        isBoardFieldOccupied(board_current, _ecevute_move.to_field)) {
        //AND MOVE FIGURE INTO PARKING
        ChessPiece::FIGURE fig_to_remove = getFigureOnField(board_current, _ecevute_move.to_field);
        int fdIndex = get_next_free_park_position(ChessBoard::BOARD_TPYE::REAL_BOARD, fig_to_remove);
        if (fdIndex >= 0) {
            //ADD ADDITIONAL MOVE STEP TO MOVE THE FIGURE PLACED ON THE TARGET FIELD AWAY
            move_list.push_back(MovePiar(_ecevute_move.to_field, ChessField::Index2Field(fdIndex)));

        }
        //ADD FIRST MOVE => TARGET FIELD IS NOW EMPTY
        move_list.push_back(_ecevute_move);

    }
    if (!isBoardFieldOccupied(board_current, _ecevute_move.from_field) &&
        isBoardFieldOccupied(board_current, _ecevute_move.to_field)) {
        //ADD FIREGURE TO FIELD IF NEDDED
        ChessPiece::FIGURE fig_to_add = getFigureOnField(board_target, _ecevute_move.to_field);
        std::vector<ChessField::CHESS_FILEDS> fd = get_parking_fileld_occupied_by_figure_type(board_current,
                                                                                              fig_to_add);
        if (fd.size() > 0) {
            move_list.push_back(MovePiar(fd.at(0), _ecevute_move.to_field));
        }
    } else {
        move_list.push_back(_ecevute_move);
    }

    //EXECUTE MOVES


    for (size_t i = 0; i < move_list.size(); i++) {
        makeMoveSync(move_list.at(i), false, false, false);
    }
    copyBoard(board_current, board_target);
    return true;
}

void ChessBoard::initBoardArray(ChessPiece::FIGURE *_board) {
    if (!_board) {
        return;
    }

    ChessPiece::FIGURE empty_fig;
    empty_fig.color = ChessPiece::COLOR_UNKNOWN;
    empty_fig.type = ChessPiece::TYPE_INVALID;
    empty_fig.is_empty = true;       //!!
    empty_fig.figure_number = -1;


    for (int i = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_A1);
         i < ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END); i++) {
        _board[i] = empty_fig;
    }


}

std::vector<ChessBoard::FigureFieldPair>
ChessBoard::compareBoards(ChessPiece::FIGURE *_board_a, ChessPiece::FIGURE *_board_b, bool _include_park_pos) {
    std::vector<ChessBoard::FigureFieldPair> diff_list;
    ChessPiece::FIGURE *board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
    ChessPiece::FIGURE *board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);

    if (!_board_a || !_board_b) {
        return diff_list;
    }

    int offset = 2;
    if (_include_park_pos) {
        offset = 0;
    }


    //NOW CHECK BOARD DIFFERENCES
    for (int i = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_A1);
         i < ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1); i++) {
        ChessPiece::FIGURE tmp_curr = getFigureOnField(board_current, ChessField::Index2Field(i));
        ChessPiece::FIGURE tmp_target = getFigureOnField(board_target, ChessField::Index2Field(i));



        //CHECK IF EQUAL FIGURES
        if (ChessPiece::compareFigures(tmp_curr, tmp_target)) {
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

std::vector<ChessBoard::FigureFieldPair>::iterator
ChessBoard::getFigureInDiffListPlacedOnEmptpy(std::vector<ChessBoard::FigureFieldPair> &_figlist,
                                              ChessPiece::FIGURE _fig) {
    std::vector<FigureFieldPair>::iterator nit;
    for (nit = _figlist.begin(); nit != _figlist.end(); nit++) {
        if (!nit->processed && nit->field_target.figure.type == _fig.type &&
            nit->field_target.figure.color == _fig.color && !nit->field_target.figure.is_empty &&
            nit->field_curr.figure.is_empty) {
            return nit;

        }
    }
    return _figlist.end();

}

bool ChessBoard::copyBoard(ChessPiece::FIGURE *_from, ChessPiece::FIGURE *_to) {
    if (!_from || !_to) {
        return false;
    }

    const int c_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_A1);
    const int c_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END);

    for (int i = c_start; i < c_end; i++) {
        _to[i] = _from[i];
    }


    return true;
}

bool ChessBoard::syncRealWithTargetBoard() {
    HardwareInterface::getInstance()->set_fan_state(HardwareInterface::HI_FAN::HI_FAN_MOTOR_DRIVER, true);
    ChessPiece::FIGURE *board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
    ChessPiece::FIGURE *board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);
    volatile bool _changes_made = true;
    while (_changes_made) {
        //syncRealWithTargetBoard_add_remove_empty RETURNS FALSE IF NO MOVE WAS MADE => SO BOARDS ARE IN SYNC
        if (!syncRealWithTargetBoard_add_remove_empty()) {
            break;

            //	_changes_made = _changes_made | syncRealWithTargetBoard_not_empty();
        }
    }

    copyBoard(board_current, board_target);

#ifdef DEBUG
    printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);
    printBoard(ChessBoard::BOARD_TPYE::TARGET_BOARD);
#endif // DEBUG

    HardwareInterface::getInstance()->set_fan_state(HardwareInterface::HI_FAN::HI_FAN_MOTOR_DRIVER, true);
    return true;
}

bool ChessBoard::syncRealWithTargetBoard_add_remove_empty() {
    //TODO
    //USE MAKE MOVE TO GENERATE A MOVE LIST THEN EXECUTE THE MOVES
    std::vector<ChessBoard::MovePiar> move_list;
    ChessPiece::FIGURE *board_current = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
    ChessPiece::FIGURE *board_target = get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD);

    if (!board_current || !board_target) {
        return false;
    }

    //LAMBDA FUNCTION FOR GETTING THE NEXT FIGURE THATS WITH THE SAME TYPE WHICH WAS ALSO MODIFIED
    std::vector<FigureFieldPair>::iterator gfidlp_it;
    //GET THE DIFFERNCE BETWEEN THE TWO BOARDS AS LIST
    std::vector<FigureFieldPair> diff = compareBoards(board_current, board_target, true);
    //BOARD IS EQUAL = NO CHANGES
    if (diff.size() == 0) {
        return false;
    }


    //FIND ICH EIN FELD BEI DER DAS CURRENT FELD = DEM TARGET FELD IST ?
    std::vector<MovePiar> diff_matched;



    //NORMALER ZUG VON A NACH B WENN B LEER UND FIGURTYP IST GLEICH
    if (move_list.empty()) {
        for (size_t it = 0; it < diff.size(); it++) {
            if (diff.at(it).field_curr.figure.is_empty && !diff.at(it).field_target.figure.is_empty &&
                !ChessPiece::compareFigures(diff.at(it).field_curr.figure, diff.at(it).field_target.figure)) {

                int found = -1;
                for (size_t itd = 0; itd < diff.size(); itd++) {
                    if (!diff.at(itd).field_curr.figure.is_empty && diff.at(itd).field_target.figure.is_empty &&
                        ChessPiece::compareFigures(diff.at(it).field_target.figure, diff.at(itd).field_curr.figure)) {
                        found = itd;
                        break; //FOUND A FIGURE
                    }
                }
                if (found >= 0 && found < diff.size()) {
                    move_list.push_back(MovePiar(diff.at(found).field_curr.field, diff.at(it).field_target.field));
                    diff.at(it).processed = true;
                    break;
                }
            }
        }
    }


    //A IST BESETZT UND B IST LEER => ENTFERNEN DER FIGUR VOM FELD
    if (move_list.empty()) {
        for (size_t it = 0; it < diff.size(); it++) {
            if (diff.at(it).processed) {
                it++;
                continue;
            }
            //ZIELFELD NICHT LEER => ALOS VON EINER ANDEREN FIGUR SPÄTER BESETZT
            if (!diff.at(it).field_curr.figure.is_empty) {//&& !diff.at(it).field_target.figure.is_empty
                //2. FIELDS ARE THE SAME => SO FIGURE HAS CHANGES ON FIELD
                //	if(diff.at(it).field_curr.field == diff.at(it).field_target.field) {
                //TARGET FIGURE IS THE CURRENT FIGURE OF THE FIELD
                if (!ChessPiece::compareFigures(diff.at(it).field_curr.figure, diff.at(it).field_target.figure)) {

                    //THEN WE HAVE TO FIND A FIELD WHERE THE FIGURE ON it->field_curr
                    //HAS A OTHER FIELD PAIR IF itd->field_TARGET->FIGURE && itd->field_current->empty
                    //THEN THE TARGET FIELD FIGURE HAVE TO BE REMOVED FROM THE FIELD

                    //TODO ERROR REWORK
                    int found = -1;
                    for (size_t itd = 0; itd < diff.size(); itd++) {
                        //FINDE EIN WEITERES FELD BEI DER DIE TAGET FIGUR :
                        // CURRENT FIELD LERR
                        // TARGET FIELD MIT DER PASSENDEND FIGUR BELEGT UND NICHT LEER
                        if (diff.at(itd).field_curr.figure.is_empty &&
                            diff.at(it).field_curr.figure.type == diff.at(itd).field_target.figure.type &&
                            diff.at(it).field_curr.figure.color == diff.at(itd).field_target.figure.color &&
                            !diff.at(itd).field_target.figure.is_empty) {
                            found = itd;
                            break; //FOUND A FIGURE
                        }
                    }
                    //IF A OTHER FIELD IS FOUND WITH THE FIGURE TO REMOVE => CREATE A MOVE
                    if (found >= 0 && found < diff.size()) {

                        move_list.push_back(MovePiar(diff.at(it).field_curr.field, diff.at(found).field_target.field));
                        diff.at(it).processed = true;
                        break;
                    } else //ELSE MOVE FIGURE TO PARKING SLOT
                    {
                        int fdIndex = get_next_free_park_position(ChessBoard::BOARD_TPYE::REAL_BOARD,
                                                                  diff.at(it).field_curr.figure);
                        if (fdIndex >= 0) {
                            move_list.push_back(
                                    MovePiar(diff.at(it).field_curr.field, ChessField::Index2Field(fdIndex)));
                            diff.at(it).processed = true;
                            break;
                        }
                    }
                    //}
                }
            }
        }
    }


    //LETZTE OPTION IST WENN FIGUREN AUF DEM BRETT FEHLEN => A LEER B GEFÜLLT
    if (move_list.empty()) {
        //TODO
        for (size_t it = 0; it < diff.size(); it++) {
            if (diff.at(it).processed) {
                it++;
                continue;
            }
            //A EMPTY AND B NOT EMPTY
            if (diff.at(it).field_curr.figure.is_empty && !diff.at(it).field_target.figure.is_empty) {
                //GET A MACHTING FIGURE FROM PARK POSITIONS
                std::vector<ChessField::CHESS_FILEDS> fields = get_parking_fileld_occupied_by_figure_type(board_current,
                                                                                                          diff.at(it).field_target.figure);
                if (fields.size() > 0) {
                    //CREATE MOVE FROM PARKPOS INTO FIELD
                    move_list.push_back(MovePiar(fields.at(0), diff.at(it).field_target.field));
                    diff.at(it).processed = true;
                    break; //BREAK OUTHER LOOP
                }
            }
        }
    }


    volatile bool move_made = false;
    if (move_list.size() > 0) {
        move_made = true;
    }
    //FINALLY EXECUTE MOVES
    for (size_t i = 0; i < move_list.size(); i++) {

        const MovePiar mv = move_list.at(i); //DEBUG
        LOG_F(INFO, "MOVE_PAIR %s",
              (ChessField::field_to_string(mv.from_field) + ChessField::field_to_string(mv.to_field).c_str()).c_str());

        makeMoveSync(mv, false, false, false);
        printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);
        int ij = 0;
    }
    return move_made;
}

std::vector<ChessField::CHESS_FILEDS> ChessBoard::get_free_fields_on_the_board(ChessPiece::FIGURE *_board_pointer) {
    std::vector<ChessField::CHESS_FILEDS> tmp;
    if (_board_pointer == nullptr) {
        return tmp;
    }

    for (size_t h = 0; h < BOARD_HEIGHT; h++) {

        for (size_t w = 2; w < 10; w++) {
            size_t index = w + (h * BOARD_WIDTH);
            ChessPiece::FIGURE tmp_fig = _board_pointer[index];
            if (tmp_fig.is_empty) {
                tmp.push_back(ChessField::get_field_from_board_index(index));

            }
        }
    }

    return tmp;
}

std::vector<ChessField::CHESS_FILEDS>
ChessBoard::get_parking_fileld_occupied_by_figure_type(ChessPiece::FIGURE *_board_pointer, ChessPiece::FIGURE _fig) {
    std::vector<ChessField::CHESS_FILEDS> res;
    if (_board_pointer == nullptr) {
        return res;
    }



    //DETERM SITE OFFSET
    int side_offset_start = -1;
    int side_offset_end = -1;
    if (_fig.color == ChessPiece::COLOR_WHITE) {
        side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1);
        side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);
    } else if (_fig.color == ChessPiece::COLOR_BLACK) {
        side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);
        side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END);
    } else {
        return res;
    }



    //NOW CHECK FIELDS FOR NEXT FREE SPACE
    int counter = 0;
    for (int i = side_offset_start; i < side_offset_end; i++) {

        ChessField::CHESS_FILEDS field = ChessField::Index2Field(i);
        ChessPiece::FIGURE fig = getFigureOnField(_board_pointer, field);

        //CHECK IF FIELD EMPTY AND RIGHT TYPE
        if (!fig.is_empty && getSpecificFigureTypeForParkpostion(field) == _fig.type) {
            res.push_back(field);
        }


    }

    return res;
}

std::vector<ChessField::CHESS_FILEDS>
ChessBoard::get_chess_fields_occupied_from_figure(ChessPiece::FIGURE *_board_pointer, ChessPiece::FIGURE _fig,
                                                  bool _board_only) {

    std::vector<ChessField::CHESS_FILEDS> tmp;

    if (_board_pointer == nullptr || !ChessPiece::IsFigureValid(_fig)) {
        return tmp;
    }

    int w_offset = 0;
    if (_board_only) {
        w_offset = 2;       //BOARD WIDTH - 2*2 FOR PARKING
    }

    for (size_t w = 0 + w_offset; w < BOARD_WIDTH - w_offset; w++) {
        for (size_t h = 0; h < BOARD_HEIGHT; h++) {
            size_t index = w + (h * BOARD_WIDTH);
            ChessPiece::FIGURE tmp_fig = _board_pointer[index];


            //CHECK IF EQUAL FIGURES
            if (ChessPiece::compareFigures(_fig, tmp_fig)) {
                //ADD TO THE LIST
                tmp.push_back(ChessField::get_field_from_board_index(index));
            }

        }
    }


    return tmp;

}

ChessPiece::TYPE ChessBoard::getSpecificFigureTypeForParkpostion(ChessField::CHESS_FILEDS _field) {

    //DETERM HERE THE TYPE OF FIGURE FOR WHICH PARK POS
    const int TYPE_SIZE = 16;    //IS THE NUMBER OF PARK POSITIONS
    const ChessPiece::TYPE type_offset[16] = {ChessPiece::TYPE::TYPE_BISHOP, ChessPiece::TYPE::TYPE_BISHOP,
                                              ChessPiece::TYPE::TYPE_KING, ChessPiece::TYPE::TYPE_QUEEN,
                                              ChessPiece::TYPE::TYPE_KNIGHT, ChessPiece::TYPE::TYPE_KNIGHT,
                                              ChessPiece::TYPE::TYPE_ROOK, ChessPiece::TYPE::TYPE_ROOK,
                                              ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN,
                                              ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN,
                                              ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN,
                                              ChessPiece::TYPE::TYPE_PAWN, ChessPiece::TYPE::TYPE_PAWN};


    if (!isFieldParkPosition(_field)) {
        return ChessPiece::TYPE::TYPE_INVALID;
    }

    int res = ChessField::parkpos2IDIndex(_field);
    if (res < 0) {
        return ChessPiece::TYPE::TYPE_INVALID;
    }

    return type_offset[res];

}

int ChessBoard::get_next_free_park_position(ChessBoard::BOARD_TPYE _target_board, ChessPiece::FIGURE _fig) {

    ChessPiece::FIGURE *cb = get_board_pointer(_target_board);
    if (cb == nullptr) {
        return -1;
    }



    //DETERM SITE OFFSET
    int side_offset_start = -1;
    int side_offset_end = -1;
    if (_fig.color == ChessPiece::COLOR_WHITE) {
        side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1);
        side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);
    } else if (_fig.color == ChessPiece::COLOR_BLACK) {
        side_offset_start = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1);
        side_offset_end = ChessField::field2Index(ChessField::CHESS_FILEDS::CHESS_FIELD_END);
    } else {
        return -1;
    }



    //NOW CHECK FIELDS FOR NEXT FREE SPACE
    int counter = 0;
    for (int i = side_offset_start; i < side_offset_end; i++) {

        ChessField::CHESS_FILEDS field = ChessField::Index2Field(i);
        ChessPiece::FIGURE fig = getFigureOnField(cb, field);

        //CHECK IF FIELD EMPTY AND RIGHT TYPE
        if (fig.is_empty && getSpecificFigureTypeForParkpostion(field) == _fig.type) {
            return i;
        }


    }

    return -1; //RETURN INVALID
}

ChessPiece::FIGURE *ChessBoard::get_board_pointer(ChessBoard::BOARD_TPYE _target_board) {
    if (_target_board == ChessBoard::BOARD_TPYE::TARGET_BOARD) {
        return board_target;
    } else if (_target_board == ChessBoard::BOARD_TPYE::REAL_BOARD) {
        return board_current;
    } else if (_target_board == ChessBoard::BOARD_TPYE::TEMP_BOARD) {
        return board_temp;
    } else {
        return nullptr;//INVALID SELECTD BOARD
    }
}

int ChessBoard::get_figure_type_count(ChessBoard::BOARD_TPYE _target_board, char _type_char, bool _board_only) {
    return get_figure_type_count(get_board_pointer(_target_board), _type_char, _board_only);

}

int ChessBoard::get_figure_type_count(ChessPiece::FIGURE *_board_pointer, char _type_char, bool _board_only) {
    if (_board_pointer == nullptr) {
        return -1;
    }


    int count = 0;
    int w_offset = 0;
    if (_board_only) {
        w_offset = 2;      //BOARD WIDTH - 2*2 FOR PARKING
    }


    for (size_t w = 0 + w_offset; w < BOARD_WIDTH - w_offset; w++) {
        for (size_t h = 0; h < BOARD_HEIGHT; h++) {
            size_t index = w + (h * BOARD_WIDTH);
            ChessPiece::FIGURE tmp_fig = _board_pointer[index];

            //CHECK IF EQUAL FIGURES
            if (ChessPiece::Figure2Charakter(tmp_fig) == _type_char && ChessPiece::IsFigureValid(tmp_fig)) {
                count++;
            }
        }
    }


    return count;
}

bool ChessBoard::boardFromFen(std::string _fen, ChessBoard::BOARD_TPYE _target_board) {

    if (_fen.empty()) {
        return false;
    }

    _fen += " ";      //THE BOARD BLOCK (CONTAINING FROM START OF THE STRING) TO THE FIRST WHITESPACE => EVERYTHING AFTER IS NOT IMPORTANT
    //CREATE AN EMPTY FIGURE

    ChessPiece::FIGURE empty_firegure;
    empty_firegure.is_empty = true;
    empty_firegure.figure_number = -1;
    empty_firegure.color = ChessPiece::COLOR_UNKNOWN;
    empty_firegure.type = ChessPiece::TYPE_INVALID;

    ChessPiece::FIGURE *cb = get_board_pointer(_target_board);
    if (cb == nullptr) {
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
    for (size_t h = 0; h < BOARD_HEIGHT; h++) {
        for (size_t w = 0; w < BOARD_WIDTH; w++) {
            size_t index = w + (h * BOARD_WIDTH);
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

    for (const char *p = _fen.c_str(); *p != 0; ++p) {
        if (*p == ' ') {
            ++num_spaces;
        } else if (*p == '/') {
            ++num_rows;
        }

    }
    //INVALID FEN
    if (num_rows > 8) //DISBALE SPACE CHECK => ONLY USED FOR EXTENDED FEN STRINGS num_spaces != 5 ||
    {
        return false;
    }


    int board_position_x = 0;      //WIDTH
    int board_position_y = 0;      //HEIGHT
    const char *p = _fen.c_str();
    for (; *p != ' '; ++p) {
        const char ch = *p;

        if (isdigit(ch)) {
            //IS A DIGIT => EMPTY SPACE FOR THE VALUE OF THE DIGIT
            for (int i = 0; i < (ch - '0'); ++i) {
                board_position_x++;      //STEP OVER FIELD
            }
        } else if (ch == '/') {
            //NEW ROW
            printf("\n\n");
            board_position_y++;      //GOTO NEXT ROW
            board_position_x = 0;       //AND STARTING A NEW LINE WITH X COUNTER AT 0
        } else if ((ch > 65 && ch < 90) || (ch > 97 && ch < 122)) {
            //IS A FIGURE
            printf(" %c", ch);
            ChessPiece::FIGURE tmp = ChessPiece::getFigureByCharakter(ch);
            //CHECK FIGURE IS VLAID
            if (!ChessPiece::IsFigureValid(tmp)) {
                continue;
            }


            //ASSIGN FIGURE NUMBER
            figure_counter[ch]++;
            tmp.figure_number = figure_counter[ch];

            //PLACE THE FIGURE
            size_t index = board_position_x + (board_position_y * BOARD_WIDTH) +
                           2;      // + (board_position_y*BOARD_WIDTH);   //+2 FOR THE 2 SPACE FOR THE PARKING POSITIONS

            ChessField::CHESS_FILEDS field = ChessField::get_field_from_board_index(index);
            int bindex = ChessField::field2Index(field);

            if (bindex == 72) {
                int b = 0;
            }

            cb[bindex] = tmp;
            //STEP TO NEXT FIELD
            board_position_x++;

        }
    }


    //printBoard(_target_board);

    //DETERMN FIGURES IN PARK POSITION
    const size_t FIGURE_COUNT = 12;
    const char FIGURES[FIGURE_COUNT] = {'r', 'n', 'b', 'q', 'k', 'p', 'R', 'N', 'B', 'Q', 'K', 'P'};
    int park_pos_counter_white = 0;
    int park_pos_counter_black = 0;

    //FOR EACH FIGURE
    for (int i = 0; i < FIGURE_COUNT; i++) {
        //GET TARGET COUNT
        const int target_count = ChessPiece::getFigureCountByChrakter(FIGURES[i]);
        const int current_count = figure_counter[FIGURES[i]] + 1;
        const int missing_count = llabs(current_count - target_count);
        //IF TARGET COUNT REACHED
        if (missing_count <= 0) {
            continue;
        }
        //ELSE FILL UP
        for (int curr_missing_count = 0; curr_missing_count < missing_count; curr_missing_count++) {
            //CREATE FIGURE WITH NUMBER
            ChessPiece::FIGURE tmp = ChessPiece::getFigureByCharakter(FIGURES[i]);
            figure_counter[FIGURES[i]]++;
            tmp.figure_number = figure_counter[FIGURES[i]];
            //CHECK FIGURE IS VLAID
            if (!ChessPiece::IsFigureValid(tmp)) {
                return false;
            }

            int pos_index = get_next_free_park_position(_target_board, tmp);
            //INVALID POSITION
            if (pos_index < 0) {
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

void ChessBoard::printBoard(ChessBoard::BOARD_TPYE _target_board) {
    ChessPiece::FIGURE *cb;

    if (_target_board == ChessBoard::BOARD_TPYE::TARGET_BOARD) {
        cb = board_target;
        std::cout << "----- BOARD - TARGET -----------" << std::endl;

    } else if (_target_board == ChessBoard::BOARD_TPYE::REAL_BOARD) {
        cb = board_current;
        std::cout << "----- BOARD - REAL -------------" << std::endl;

    } else if (_target_board == ChessBoard::BOARD_TPYE::TEMP_BOARD) {
        cb = board_temp;
        std::cout << "----- BOARD - TEMP -------------" << std::endl;

    } else {
        return;
    }
    std::cout << "x = INVALID -----. = EMPTY FIELD" << std::endl;

    std::cout << "  W W | A B C D E F G H | B B " << std::endl;
    for (size_t h = 0; h < BOARD_HEIGHT; h++) {
        std::cout << (BOARD_HEIGHT - h) << " ";
        for (size_t w = 0; w < BOARD_WIDTH; w++) {

            const ChessField::CHESS_FILEDS f = ChessField::get_field_from_board_index(w + (h * BOARD_WIDTH));
            const int index = ChessField::field2Index(f);
            ChessPiece::FIGURE fig = getFigureOnField(cb, f);
            if (w == 2 || w == 10) {
                std::cout << "| ";
            }

            //size_t index = w + (h*BOARD_WIDTH);
            //PRINT BLANK IF FIGURE IS INVALID
            if (fig.is_empty) {
                std::cout << "." << " ";
                continue;
            }
            //PRINT X FOR AN INVALID FIGURE
            if (!ChessPiece::IsFigureValid(fig)) {
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

void ChessBoard::log_error(std::string _err) {
#ifdef USE_STD_LOG
    std::cout << _err << std::endl;
#endif //USE_STD_LOG
    //	LOG_F(ERROR, "%s", _err.c_str());
}

void ChessBoard::getParkPositionCoordinates(ChessField::CHESS_FILEDS _index, int &_x, int &_y, IOController::COIL _coil,
                                            bool before_parkpostion_entry) {
    //INDEX IS NOT A PARK POSITION
    if (!isFieldParkPosition(_index)) {
        _x = -1;
        _y = -1;
        return;
    }
    //DETERM IF PARKING POSITION IS BLACK OR WHITE SLOT
    int is_black_park_position = 0;
    int field_index = static_cast<int>(_index);
    if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1) &&
        field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_16)) {
        is_black_park_position = 1;
    } else if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1) &&
               field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_16)) {
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
    } else {
        ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE, line_offset);
        ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET, y_offset);
    }


    //LOAD PARKING SLOT CELL SIZE
    ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE, cell_size);
    ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET,
                                        before_line_offset);


    //GET PARK POSITON INDEX 1-16
    int pp_entry = 0;
    if (is_black_park_position == 2) //BLACK
    {
        pp_entry = (magic_enum::enum_integer(_index) -
                    magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_BLACK_1)) % 16;
    } else {
        pp_entry = (magic_enum::enum_integer(_index) -
                    magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1)) % 16;
    }

    if (before_parkpostion_entry) {
        if (is_black_park_position == 2) //BLACK
        {
            _x = line_offset + before_line_offset;
        } else {
            _x = line_offset - before_line_offset;
        }


        _y = (pp_entry * cell_size) + y_offset;
    } else {
        _x = line_offset;
        _y = (pp_entry * cell_size) + y_offset;
    }

}

void ChessBoard::getFieldCoordinates(ChessField::CHESS_FILEDS _index, int &_x, int &_y, IOController::COIL _coil,
                                     bool _get_only_array_index) {

    //TODO CHECK PARK POSTION
    if (isFieldParkPosition(_index)) {
        _x = -1;
        _y = -1;
        return;
    }


    int h1_offset_x = 0;
    int h1_offset_y = 0;
    //LOAD OFFSET FROM CONFIG FILE IF HOME POS IS NOT IN FIELD H1
    ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X, h1_offset_x);
    ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y, h1_offset_y);

    int field_with_x = ConfigParser::getInstance()->getInt_nocheck(
            ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X);
    int field_with_y = ConfigParser::getInstance()->getInt_nocheck(
            ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y);

    ///CONVERT TO XY WITH A NORMAL CHESS FIELD WITH 8 by 8 FIELDS
    int field_index = static_cast<int>(_index);



    //GET COIL OFFSET
    int coil_offset_x = 0;
    int coil_offset_y = 0;
    get_coil_offset(_coil, coil_offset_x, coil_offset_y);

    const int field_index_x = field_index / 8;
    const int field_index_y = field_index % 8;

    const int board_width_x = field_with_x * (BOARD_WIDTH - 4 - 1); //-4 for 2x 2 rows parking pos //-1 for 0-7 fields
    const int board_width_y = field_with_y * (BOARD_HEIGHT - 4 - 1);


    if (_get_only_array_index) {
        _x = field_index_x;
        _y = field_index_y;
    } else {
        //CALC COORDIANTES FOR FIELDSS
        _x = board_width_x - ((field_index_x * field_with_x));
        _y = ((field_index_y * field_with_y));
        //ADD COIL OFFSET
        _x += coil_offset_x;
        _y += coil_offset_y;
        //ADD HOME FIELD H1 OFFSET
        _x += h1_offset_x;
        _y += h1_offset_y;
    }


}

ChessBoard::BOARD_ERROR ChessBoard::scanBoard(bool _include_park_postion) {
    //SAVE CURRENT FIELD
    ChessField::CHESS_FILEDS original_position = current_field;
    IOController::COIL original_coil = current_selected_coil;

    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);


    HardwareInterface::getInstance()->set_speed_preset(HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_TRAVEL);
    int x = 0;
    int y = 0;
    for (int i = 0; i < magic_enum::enum_integer(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1); i++) {

        travelToField(static_cast<ChessField::CHESS_FILEDS>(i),
                      IOController::COIL::COIL_NFC);             //TRAVEL TO NEXT FIELD

        //ON PRODUCTION HARDWARE WE NEED TO LIFT THE NFC HEAD
        if (HardwareInterface::getInstance()->is_production_hardware()) {
            HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
        }

        //SCAN NFC TAG OF FIGURE

        ChessPiece::FIGURE tmop = HardwareInterface::getInstance()->ScanNFC();
        if (tmop.type == ChessPiece::TYPE::TYPE_INVALID) {
            tmop.is_empty = true;
        } else {
            tmop.is_empty = false;
        }

        //ON PRODUCTION HARDWARE WE NEED TO LIFT DOWN THE NFC HEAD
        if (HardwareInterface::getInstance()->is_production_hardware()) {
            HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
        }
        //STORE FIGURE ON CURRENT BOARD
        board_current[ChessField::field2Index(static_cast<ChessField::CHESS_FILEDS>(i))] = tmop;
        //PRINT THE NEW BOARD
        printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);

    }

    //TRAVEL BACK TO ORIGINAL STARTING POSITION
    travelToField(original_position, original_coil);

    return ChessBoard::BOARD_ERROR::NO_ERROR;
}

bool ChessBoard::isFieldParkPosition(ChessField::CHESS_FILEDS _field) {
    int field_index = static_cast<int>(_field);            //CAST FIELD ENUM TO A INDEX/NUMBER
    if (field_index < 64) //ALL PARKPOSITIONS ARE INDEX >=64
    {
        return false;
    } else {
        return true;
    }
}

std::list<ChessPiece::FIGURE> ChessBoard::checkBoardForFullFigureSet(ChessBoard::BOARD_TPYE _target_board) {
    std::list<ChessPiece::FIGURE> tmp;
    //FIGURES TO CHECK
    const int CFIG = 12;
    char figures[CFIG] = {'r', 'n', 'b', 'q', 'k', 'p', 'R', 'N', 'B', 'Q', 'K', 'P'};
    //CHECK FOR MISSING FIGURES
    for (int i = 0; i < CFIG; i++) {
        if (get_figure_type_count(_target_board, figures[i], false) !=
            ChessPiece::getFigureCountByChrakter(figures[i])) {
            tmp.push_back(ChessPiece::getFigureByCharakter(figures[i]));
            LOG_F(WARNING, "BOARD MISSING FIGURE: %c", figures[i]);
        }
    }
    //RETURN LIST OF MISSING FIGURE TYPE!!
    return tmp;
}

IOController::COIL
ChessBoard::getValidCoilTypeParkPosition(ChessField::CHESS_FILEDS _field, IOController::COIL _target) {

    //if (!isFieldParkPosition(_field))
//	{
//		return _target;
//	}

    if (_target == IOController::COIL::COIL_NFC) {
        return _target;
    }

    //----- THIS CODE IS FOR THE PROD HARDWARE WITH ONLY ONE COIL ON POSITION A --------- //
    if (HardwareInterface::getInstance()->is_production_hardware()) {
        return IOController::COIL::COIL_A;
    }



    //----- THIS CODE IS FOR THE DK HARDWARE WITH TWO COILS TO SWITCH BETWEEN THEM --------- //
    bool is_black_park_position = true;
    int field_index = static_cast<int>(_field);


    if (isFieldParkPosition(_field)) {
        if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_1) &&
            field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_PARK_POSTION_WHITE_16)) {
            return IOController::COIL::COIL_B;
        } else {
            return IOController::COIL::COIL_A;
        }
    } else {
        if (field_index >= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_A1) &&
            field_index <= static_cast<int>(ChessField::CHESS_FILEDS::CHESS_FIELD_D8)) {
            return IOController::COIL::COIL_B;
        } else {
            return IOController::COIL::COIL_A;
        }
    }


}

ChessBoard::BOARD_ERROR ChessBoard::travelToField(ChessField::CHESS_FILEDS _field, IOController::COIL _coil) {


    HardwareInterface::getInstance()->set_speed_preset(HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);


    //GET FIELD COORDIANTES
    int field_coordinates_x = -1;
    int field_coordinates_y = -1;
    //GET FIELD TYPE NORMAL BOARD FIELD OR PARK POSITION
    if (!isFieldParkPosition(_field)) {
        getFieldCoordinates(_field, field_coordinates_x, field_coordinates_y, _coil, false);
    } else {
        getParkPositionCoordinates(_field, field_coordinates_x, field_coordinates_y, _coil, true);
    }
    //CONVERSION ERROR HANDLING
    if (field_coordinates_x < 0 || field_coordinates_y < 0) {
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
        if (wait_counter > WAITIME_MULTIPLIER_AXIS_ARRIVAL) {
            break;
        }
    }
    //DISBALE MOTOR
    HardwareInterface::getInstance()->disable_motors();

    //CHECK IF MOTORS ARRIVED TARGET POSITION WITH NO ERROR
    if (wait_counter > WAITIME_MULTIPLIER_AXIS_ARRIVAL) {
        log_error("ChessBoard::BOARD_ERROR::AXIS_TRAGET_ARRIVAL_FAILED");
        return ChessBoard::BOARD_ERROR::AXIS_TRAGET_ARRIVAL_FAILED;
    }
    //SAVE CURRENT FIELD POSITION
    current_field = _field;
    return ChessBoard::BOARD_ERROR::NO_ERROR;
}

ChessBoard::BOARD_ERROR ChessBoard::get_coil_offset(IOController::COIL _coil, int &_x, int &_y) {
    //LOAD COIL DISTANCE COIL_A <--- NFC ---> COIL_B THE DISTANCE IS COIL_A to COIL_B THE NFC COIL IS IN THE MIDDLE
    int coil_distance = 0;
    ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM, coil_distance);

    int mv_distance = 0;
    //coil_distance = coil_distance / 2;
    ///IF SWITCHED TO B COIL THE TRAVEL DISTANCE IS NEGATIVE
    if (_coil == IOController::COIL::COIL_A) {
        mv_distance = -std::abs(coil_distance / 2);
    } else if (_coil == IOController::COIL::COIL_B) {
        mv_distance = std::abs(coil_distance / 2);
    } else if (_coil == IOController::COIL::COIL_NFC) {
        //	mv_distance = std::abs(coil_distance / 2);
    }

    _y = 0;            //THE COILS ARE ONLY MOVING IN X DIRECTION
    _x = mv_distance;
    return ChessBoard::BOARD_ERROR::NO_ERROR;
}

ChessBoard::BOARD_ERROR ChessBoard::corner_move_test() {
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
    HardwareInterface::getInstance()->home_sync();

    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, true);

    travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_H1, IOController::COIL::COIL_A);
    travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_A1, IOController::COIL::COIL_B);
    travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_A8, IOController::COIL::COIL_B);
    travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_H8, IOController::COIL::COIL_A);
    travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_H1, IOController::COIL::COIL_A);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);

    return ChessBoard::BOARD_ERROR::NO_ERROR;
}

std::vector<ChessBoard::MovePiar> ChessBoard::StringToMovePair(std::vector<std::string> _mv, bool _only_valid_ones) {
    std::vector<ChessBoard::MovePiar> tmp;
    for (int i = 0; i < _mv.size(); i++) {
        const MovePiar m = StringToMovePair(_mv.at(i));
        if (m.is_valid || _only_valid_ones) {
            tmp.push_back(m);
        }
    }
    return tmp;
}


ChessBoard::PossibleUserMoveResult
ChessBoard::calculcate_move_from_figure_and_destination(ChessPiece::FIGURE _figure, ChessField::CHESS_FILEDS _field,
                                                        std::vector<ChessBoard::MovePiar> _pseudo_legal_moves) {
//GET ALL MOVES IF DST FIELD
//CHECK START FIELDS IF FIGURE IS ON IT
    PossibleUserMoveResult res;

    ChessPiece::FIGURE *cb = get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD);
    if (cb == nullptr) {
        LOG_F(ERROR, "PossibleUserMoveResult cb is nullptr");
        res.error = ChessBoard::BOARD_ERROR::ERROR;
        return res;
    }


    std::vector<ChessBoard::MovePiar> mv_dst_field;
    //ITERATE THROUGH ALL
    for (int i = 0; i < _pseudo_legal_moves.size(); i++) {
        const MovePiar m = _pseudo_legal_moves.at(i);
        if (m.is_valid && m.to_field == _field) {
            //CHECK FIGURE ON START FIELD
            const ChessPiece::FIGURE ff = getFigureOnField(cb, m.from_field);
            if (ChessPiece::compareFigures(ff, _figure)) {
                ChessBoard::MovePiar mvv;
                mvv.is_valid = true;
                mvv.to_field = _field;
                mvv.from_field = m.from_field;
                mv_dst_field.push_back(mvv);
            }


        }
    }
    //CHOOSE A MOVE
    if(mv_dst_field.size() > 0){
        res.possible_move=mv_dst_field.at(0);
        res.error = ChessBoard::BOARD_ERROR::NO_ERROR;
    }

    return res;
}


std::vector<ChessField::CHESS_FILEDS>
ChessBoard::getMinimalFieldsToCheckForChanges(std::vector<ChessBoard::MovePiar> _mv) {

    std::vector<ChessField::CHESS_FILEDS> tmp;

    std::map<ChessField::CHESS_FILEDS, int> field_occourance;
    //INCREASE COUNTER AT VERY OCCURANCE FROM THE START to_field
    for (int i = 0; i < _mv.size(); i++) {
        const MovePiar m = _mv.at(i);
        if (m.is_valid) {
            field_occourance[m.from_field]++;
        }
    }
    //REPACK INTO VECTOR
    std::map<ChessField::CHESS_FILEDS, int>::iterator fo_it;
    for (fo_it = field_occourance.begin(); fo_it != field_occourance.end(); fo_it++) {
        tmp.push_back(fo_it->first);
    }

    return tmp;
}

ChessPiece::FIGURE ChessBoard::scanField(ChessField::CHESS_FILEDS _field) {
    //DISABLE ALL COILS
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
    //TRAVEL TO FIELD
    HardwareInterface::getInstance()->set_speed_preset(HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_TRAVEL);
    travelToField(_field, IOController::COIL::COIL_NFC);
    //ON PRODUCTION HARDWARE WE NEED TO LIFT UP COIL_A => NFC HEAD
    if (HardwareInterface::getInstance()->is_production_hardware()) {
        HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
    }
    //SCAN THE PIECE IF EXISTS
    ChessPiece::FIGURE tmop = HardwareInterface::getInstance()->ScanNFC();
    if (tmop.type == ChessPiece::TYPE::TYPE_INVALID) {
        tmop.is_empty = true;
    } else {
        tmop.is_empty = false;
    }
    //ON PRODUCTION HARDWARE WE NEED TO LIFT DOWN THE NFC HEAD
    if (HardwareInterface::getInstance()->is_production_hardware()) {
        HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    }
    return tmop;
}

std::vector<ChessBoard::FigureField>
ChessBoard::scanBoardForChangesByGivenFields(std::vector<ChessField::CHESS_FILEDS> _fd,
                                             ChessBoard::BOARD_TPYE _target_board) {
    std::vector<ChessBoard::FigureField> tmp;


    ChessPiece::FIGURE *cb = get_board_pointer(_target_board);
    if (cb == nullptr) {
        LOG_F(ERROR, "scanBoardForChangesByGivenFields cb is nullptr");
        return tmp;
    }

    //ITERATE OVER FIELDS AND SCAN THEM
    for (int i = 0; i < _fd.size(); i++) {
        const ChessField::CHESS_FILEDS field_to_scan = _fd.at(i);
        //SCAN THE FIELD
        const ChessPiece::FIGURE figure_scan = scanField(field_to_scan);
        const ChessPiece::FIGURE figure_on_board = getFigureOnField(cb, field_to_scan);

        //CHECK IF FIGURES ARE EQUAL => NO CHANGES IF THE SAME SO NO NEED TO SCAN THE to_field
        if (ChessPiece::compareFigures(figure_scan, figure_on_board)) {
            continue;
        }
        //ADD TO CHANGES QUEUE
        ChessBoard::FigureField changes;
        changes.field = field_to_scan;
        changes.figure = figure_on_board;
        changes.figure_scan = figure_scan;

        tmp.push_back(changes);
        //SKIP LOOP AFTER FIRST CHANGE WAS DETECTED
        if (ConfigParser::getInstance()->getBool_nocheck(
                ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_USER_NFC_MOVE_SEARCH_AFTER_FIRST_CHANGE)) {
            break;
        }
    }
    return tmp;
}

std::vector<ChessField::CHESS_FILEDS>
ChessBoard::getAllTargetFieldsFromGivenFieldAndMove(std::vector<ChessBoard::MovePiar> _moves,
                                                    std::vector<ChessBoard::FigureField> _fields) {
    //TODO CHANGE TO DICT
    std::vector<ChessField::CHESS_FILEDS> tmp;

    for (int i = 0; i < _moves.size(); i++) {
        const ChessBoard::MovePiar move = _moves.at(i);
        for (int i = 0; i < _fields.size(); i++) {
            const ChessBoard::FigureField start_field = _fields.at(i);

            if (move.from_field == start_field.field) {
                tmp.push_back(move.to_field);
            }
        }
    }
    return tmp;
}

ChessBoard::BOARD_ERROR ChessBoard::initBoard(bool _with_scan) {
    //CHECK HARDWARE INIT
    if (!HardwareInterface::getInstance()->check_hw_init_complete()) {
        log_error("ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION");
        return ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION;
    }
    //SET LIGHT TO PROCESSING
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
    //HOME AXIS AND MECHANIC
    home_board();
    //DISBALE COILS
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    //INIT BOARD ARRAYS => TO EMPY FIELDS
    initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TARGET_BOARD));
    initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::REAL_BOARD));
    initBoardArray(get_board_pointer(ChessBoard::BOARD_TPYE::TEMP_BOARD));
    //LOAD FIGURE PRESETS TO THE REAL AND TARGET BOARD
    loadBoardPreset(ChessBoard::BOARD_TPYE::TARGET_BOARD,
                    ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
    loadBoardPreset(ChessBoard::BOARD_TPYE::REAL_BOARD, ChessBoard::BOARD_PRESET::BOARD_PRESET_NO_FIGURES_PLACED);


    printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);
    printBoard(ChessBoard::BOARD_TPYE::TARGET_BOARD);

    //NEXT SCAN THE FIELD WITH PARK POSTIONS
    if (_with_scan) {
        //LOAD PRESENT WITH NO FIGURE ON THE PARKING POSITIONS
        loadBoardPreset(ChessBoard::BOARD_TPYE::REAL_BOARD,
                        ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
        //SCAN THE BOARD
        scanBoard(false);
        printBoard(ChessBoard::BOARD_TPYE::REAL_BOARD);
        //CHECK BOARD FOR A FULL SET OF CHESS FIGURES
        if (checkBoardForFullFigureSet(ChessBoard::BOARD_TPYE::REAL_BOARD).size() > 0) {
            return ChessBoard::BOARD_ERROR::FIGURES_MISSING;
        }
        //MOVE FIGURES TO START POSITION
        loadBoardPreset(ChessBoard::BOARD_TPYE::TARGET_BOARD,
                        ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
        syncRealWithTargetBoard();
    } else {
        loadBoardPreset(ChessBoard::BOARD_TPYE::TARGET_BOARD,
                        ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
        loadBoardPreset(ChessBoard::BOARD_TPYE::REAL_BOARD,
                        ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
        syncRealWithTargetBoard();
    }

    //DISBALE COILS (NOT NEEDED)
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
    //SET LIGHT TO IDLE
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);


    loadBoardPreset(ChessBoard::BOARD_TPYE::TARGET_BOARD,
                    ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION);
    //loadBoardPreset(ChessBoard::BOARD_TPYE::TARGET_BOARD,ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_PARK_POSITION);
    syncRealWithTargetBoard();
    return ChessBoard::BOARD_ERROR::INIT_COMPLETE;
}


ChessBoard::PossibleUserMoveResult
ChessBoard::scanBoardForPossibleUserMove(std::vector<ChessBoard::MovePiar> _pseudo_legal_moves) {
    //CREATE RESULT VAR
    ChessBoard::PossibleUserMoveResult result;
    result.error = ChessBoard::BOARD_ERROR::POSSIBLE_USER_MOVE_RESULT_OK;
    //GET ONLY NEEDED MOVES TO CHECK WITH NFC => REDUCE TIME
    const std::vector<ChessField::CHESS_FILEDS> minimal_start_fields = getMinimalFieldsToCheckForChanges(
            _pseudo_legal_moves);
    //SCAN THE START FIELDS AND RETURN ALL CHANGES
    const std::vector<ChessBoard::FigureField> changes_start_fields = scanBoardForChangesByGivenFields(
            minimal_start_fields, ChessBoard::BOARD_TPYE::REAL_BOARD);
    //GET ALL END FIELDS BY A SELECTED START FIELD
    const std::vector<ChessField::CHESS_FILEDS> minimal_end_fields = getAllTargetFieldsFromGivenFieldAndMove(
            _pseudo_legal_moves, changes_start_fields);
    //FINALLY SCAN ALL END FIELDS RESULTING OF THE CHANGED START FIELDS
    const std::vector<ChessBoard::FigureField> changes_end_fields = scanBoardForChangesByGivenFields(minimal_end_fields,
                                                                                                     ChessBoard::BOARD_TPYE::REAL_BOARD);
    //NOW SEARCH FOR A MATCHING MOVE WITH FROM_FIELD AND TO_FIELD MATCHES THE PESUDO_LEGAL_MOVE_LIST
    ChessBoard::MovePiar move_made;
    move_made.is_valid = false;
    //CHECK FOR MULTIBLE MOVES AND P
    if (changes_start_fields.size() == 1 && changes_end_fields.size() == 1) {
        //GET EXECUTED MOVE
        for (int i = 0; i < _pseudo_legal_moves.size(); i++) {
            if (_pseudo_legal_moves.at(i).is_valid &&
                _pseudo_legal_moves.at(i).from_field == changes_start_fields.at(0).field &&
                _pseudo_legal_moves.at(i).to_field == changes_end_fields.at(0).field) {
                move_made = _pseudo_legal_moves.at(i);
                move_made.is_valid = true;
                break;
            }
        }
    } else {
        result.error = ChessBoard::BOARD_ERROR::POSSIBLE_USER_MOVE_RESULT_MULTIBLE_MOVE_CANDIDATES;
    }
    //CHECK FOR VALID MOVE FOUND
    if (!move_made.is_valid) {
        result.error = ChessBoard::BOARD_ERROR::POSSIBLE_USER_MOVE_MOVE_INVALID;
    }
    //ASSIGN MOVE
    result.possible_move = move_made;

    return result;

}


ChessBoard::BOARD_ERROR ChessBoard::calibrate_home_pos() {
    //CHECK HARDWARE INIT
    if (!HardwareInterface::getInstance()->check_hw_init_complete()) {
        log_error("ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION");
        return ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION;
    }
    //SOME USER NOTIFICATION

    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);

    //FIRST DO HOMEING OF THE AXIS
    HardwareInterface::getInstance()->home_sync();
    //MOVE TO H1 WITH COILA_ACTIVE
    travelToField(ChessField::CHESS_FILEDS::CHESS_FIELD_H1, IOController::COIL_A);


    //ENABLE COIL A
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);
    return ChessBoard::BOARD_ERROR::NO_ERROR;
}

void ChessBoard::loadBoardPreset(ChessBoard::BOARD_TPYE _target_board, ChessBoard::BOARD_PRESET _preset) {
    //CLEAR BOARD
    ChessPiece::FIGURE invlaid_piece;
    invlaid_piece.color = ChessPiece::COLOR_UNKNOWN;
    invlaid_piece.type = ChessPiece::TYPE_INVALID;
    invlaid_piece.unique_id = 0;
    invlaid_piece.figure_number = -1;
    //CLEAR TARGET BOARD
    initBoardArray(get_board_pointer(_target_board));
    //LOAD PRESET NOW
    if (_preset == ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_START_POSTITION) {
        //LOAD DEFAULT FEN TO BOARD
        std::string tmp = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::BOARD_PRESET_START_POSITION_FEN);
        //IF CONFIG FEN IS EMPTY USE A DEFAULT
        if (tmp.empty()) {
            tmp = DEFAULT_BOARD_FEN;
        }
        if (!boardFromFen(tmp, _target_board)) {
            log_error("ChessBoard::BOARD_ERROR::INIT_NULLPTR_EXECPTION");
        }

    } else if (_preset ==
               ChessBoard::BOARD_PRESET::BOARD_PRESET_ALL_FIGURES_IN_PARK_POSITION) //LOAD ALL FIGURES IN PARTKIN POSITION BOARD
    {
        boardFromFen("8/8/8/8/8/8/8/8", _target_board);
    } else if (_preset ==
               ChessBoard::BOARD_PRESET::BOARD_PRESET_NO_FIGURES_PLACED) //LOAD ALL FIGURES IN PARTKIN POSITION BOARD
    {
        initBoardArray(get_board_pointer(_target_board));

    } else if (_preset == ChessBoard::BOARD_PRESET::BOARD_TEST_PATTERN) {
        boardFromFen("8/1pppppp1/1p4p1/1rnbbnr1/1RNBBNR1/1P4P1/1PPPPPP1/8 w - - 0 1", _target_board);
    }

}

void ChessBoard::home_board() {
    //DISBALE COILS TO AVOID MOVING A FIGURE
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
    //HOME MECHANIC
    HardwareInterface::getInstance()->home_sync();
}