#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <cstdint>
#include <fstream>
#include <streambuf>
#include <regex>
#include <algorithm>
#include <thread>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif



//#include <format> // FOR FORMAT STRING

// INCLUDE AND TYPEDEF FOR SYSTEM CLOCK
//USED FOR SYSTEM TICK IN MAIN LOOP
#include <ctime>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::system_clock::time_point TimePoint;
#define CHRONO_DURATION_MS(x,y) std::chrono::duration_cast<std::chrono::milliseconds>(x - y).count()


//INCLUDE SHARES RESSOURCES
#include "SHARED/guicommunicator/guicommunicator.h"
#include "SHARED/loguru-master/loguru.hpp"
//INCLUDE LOCAL RESSOURCES
#include "TMC5160.h"
#include "BackendConnector.h"
#include "ChessPiece.h"
#include "ConfigParser.h"
#include "ChessBoard.h"
#include "IOController.h"
#include "StateMachine.h"
#include "HardwareInterface.h"
#include "UDPDiscovery.h"
#include "DGT3000Interface.h"
#include "UDPServerDiscovery.h"
//------- INCLUDE EXTENTIONS -------- //
#include "VoiceRecognitionExtention.h"


#include "GCodeSender.h"


//---------------------- CONFIG DEFINED --------------------------- //
#define ATC_CONTROLLER_VERSION "{{VERSION}}"
#define CONFIG_FILE_PATH "./atccontrollerconfig.ini"
#define LOG_FILE_PATH "/tmp/atc_controller_log.log"
#define LOG_FILE_PATH_ERROR "/tmp/atc_controller_error_log.log"
//THERE ARE TWO DEVELOPMENT HARDWARE VERSIONS OF THE TABLE
//DK => 50x50cm IKEA TABLE VERSION WITH TWO COILS
//PROD_V1 => 60x60cm ITEM VERSION WITH  SKR1.4 BOARD AND CORE_YX MECHANIC
#define HWID_DK_HARDWARE "b827ebad862f"
#define HWID_PROD_V1_HARDWARE "----"

#define INVALID_HWID "1nv4l1dm4c"
using namespace std;

bool flip_screen_state = false;
int game_running_state = 0;
int mainloop_running = 0;
int make_move_mode = 0;    //TO DETERM FOR WHICH PURPOSE THE PLAYER_MAKE_MANUAL_MOVE_SCREEN WAS OPENED 0=NOT OPEN 1=TEST 2=RUNNING GAME MOVE
int make_move_scan_timer = 10; //IF ENABLED AFTER X INTERVALS SCAN BOARD FOR A USER MOVE AUTOMATICLY
std::vector<std::string> possible_moves; //STORES ALL POSSIBLE MOVES THE USER CAN MAKE
BackendConnector* gamebackend_logupload = nullptr;     //USED ONLY FOR UPLOADING THE LOGS

//----- VARS FOR CALIBRATION SCREEN ----- //
int cal_pos_x = 0;
int cal_pos_y = 0;
int cal_move = -1; //-1 disbaled 0=h1 1=a8
int cal_move_step = 5;


//----- VARS FOR SOLANOID CALIBRATION SCREEN ---- //
int solcal_move = -1; //-1 disbaled 0=upper pos 1=lower pos
int solcal_pos = 0;


//!Reads the system HWID File from the location
std::string readHWID(std::string _file)
{
    std::ifstream t(_file.c_str());
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
    return str;
}


std::string get_interface_mac_address(const string& _ifname) {
    LOG_F(INFO, "get_interface_mac_address=>");
    LOG_F(INFO, "/sys/class/net/IF_NAME/address");
    LOG_F(INFO, _ifname.c_str());
    std::ifstream iface;


    iface.open("/sys/class/net/" + _ifname + "/address");
    if (!iface) {
        LOG_F(ERROR, "CANT OPEN FILE");
        LOG_F(ERROR, "/sys/class/net/IF_NAME/address");
        LOG_F(ERROR, _ifname.c_str());
        return INVALID_HWID;
    }

    //READ FILE
    std::string str((istreambuf_iterator<char>(iface)), istreambuf_iterator<char>());
    //CHECK LENGTH
    if(!str.empty() && str != "00:00:00:00:00:00") {
        LOG_F(INFO, str.c_str());
        //REPLACE ILLEGAL CHARAKTERS
        std::string hex = regex_replace(str, std::regex("[,:.;/\\\n\t\r ]"), "");
        LOG_F(INFO, hex.c_str());
        return hex;
    }
    else {
        return INVALID_HWID;
    }
}

std::string read_file_to_string(const std::string& _path) {
    ifstream iface(_path);
    std::string str((istreambuf_iterator<char>(iface)), istreambuf_iterator<char>());
    if (str.length() > 0) {
        return str;
    }
    else {
        return "";
    }
}



bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}


void update_gamescreen_with_player_state(BackendConnector::PLAYER_STATUS _ps, guicommunicator& _gui)
{
    if (_ps.game_state.game_running) {
        //gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::GAME_SCREEN);
        if(_ps.game_state.im_white_player)
        {
            _gui.createEvent(guicommunicator::GUI_ELEMENT::GAMESCREEN_PLAYER_COLOR, guicommunicator::GUI_VALUE_TYPE::CHESS_COLOR_WHITE);
        }
        else
        {
            _gui.createEvent(guicommunicator::GUI_ELEMENT::GAMESCREEN_PLAYER_COLOR, guicommunicator::GUI_VALUE_TYPE::CHESS_COLOR_BLACK);
        }
        if (_ps.game_state.is_my_turn && _ps.game_state.im_white_player)
        {
            _gui.createEvent(guicommunicator::GUI_ELEMENT::GAMESCREEN_PLAYER_TURN_COLOR, guicommunicator::GUI_VALUE_TYPE::CHESS_COLOR_WHITE);
        }
        else if (_ps.game_state.is_my_turn && !_ps.game_state.im_white_player)
        {
            _gui.createEvent(guicommunicator::GUI_ELEMENT::GAMESCREEN_PLAYER_TURN_COLOR, guicommunicator::GUI_VALUE_TYPE::CHESS_COLOR_BLACK);
        }

        if (_ps.game_state.legal_moves.size() > 0)
        {
            std::string tmp_lm = "";
            size_t max_items = 10;
            if (_ps.game_state.legal_moves.size() < max_items)
            {
                max_items = _ps.game_state.legal_moves.size();
            }

            for (size_t i = 0; i < max_items; i++)
            {
                tmp_lm += _ps.game_state.legal_moves.at(i) + "\n";
            }
            _gui.createEvent(guicommunicator::GUI_ELEMENT::GAMESCREEN_POSSIBLE_MOVES, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, tmp_lm);
        }
        else
        {
            _gui.createEvent(guicommunicator::GUI_ELEMENT::GAMESCREEN_POSSIBLE_MOVES, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, "");
        }
    }
}


void player_enter_manual_move_start_board_scan(guicommunicator& _gui, ChessBoard& _board, BackendConnector& _gamebackend, std::vector<std::string> _possible_moves){
    if(_possible_moves.size() > 0){
        //PARSE THE STRING MOVES INTO MovePair
        const std::vector<ChessBoard::MovePiar> moves = _board.StringToMovePair(_possible_moves,true);
        //SCAN THE BOARD FOR A POSSIBLE MOVE USING THE LEGAL MOVES
        const ChessBoard::PossibleUserMoveResult possible_move_result = _board.scanBoardForPossibleUserMove(moves);

        if(possible_move_result.error == ChessBoard::BOARD_ERROR::POSSIBLE_USER_MOVE_RESULT_MULTIBLE_MOVE_CANDIDATES){
            _gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,"MULTIBLE MOVES - PLEASE REWIND FIGURES AND ENTER MOVE MANUALLY",10000);
            _gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);

        }else if(possible_move_result.error == ChessBoard::BOARD_ERROR::POSSIBLE_USER_MOVE_MOVE_INVALID) {
            _gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,"MOVE INVALID - PLEASE REWIND FIGURES AND ENTER MOVE MANUALLY",10000);
            _gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);

        }else if(possible_move_result.possible_move.is_valid){
            //CONVERT MOVE BACK TO STRING
            std::string move_made_str = ChessField::field_to_string(possible_move_result.possible_move.from_field ) +ChessField::field_to_string(possible_move_result.possible_move.to_field);
            //SEND MOVE TO BACKEND
            if(_gamebackend.set_make_move(move_made_str))
            {
                //UPDATE THE REAL_BOARD WITH THE MADE MOVE =>
                _board.makeMoveSyncVirtual(_board.StringToMovePair(move_made_str));
                //board.syncRealWithTargetBoard();
                _gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::GAME_SCREEN);
                make_move_mode = 0; //MOVE VALID
            }else
            {
                _gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,"MOVE INVALID - PLEASE REWIND FIGURES AND ENTER MOVE MANUALLY",10000);
                _gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);
                make_move_mode = 2; //MOVE INVALID => SHOW USER MOVE ENTRY UI AGAIN
            }
        }
        //USER HAS NO MOVE OPTIONS
    }else{
        _gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,"NO LEGAL MOVE LEFT - PLEASE ENTER MOVE MANUALLY",10000);
        _gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);
    }
}

void signal_callback_handler(int signum)
{
    printf("Caught signal %d\n", signum);

    LOG_F(ERROR, "Caught signal %d\n", signum);
    loguru::flush();
    //IF GOT A SIGNAL READ LOGFILE AND UPLOAD THEM
    std::string log = read_file_to_string(LOG_FILE_PATH_ERROR);
    if (!log.empty() && gamebackend_logupload)
    {
        gamebackend_logupload->upload_logfile(log);
    }

    mainloop_running = signum;
    //exit(signum);
}


int main(int argc, char *argv[])
{
    //REGISTER SIGNAL HANDLER
    signal(SIGINT, signal_callback_handler);



    //---- PRINT HELP MESSAGE ------------------------------------------------------ //
    if (cmdOptionExists(argv, argv + argc, "-help"))
    {
        std::cout << "---- ATC_CONTROLLER HELP ----" <<std::endl;
        std::cout << "-help                   | prints this message" <<std::endl;
        std::cout << "-writeconfig            | overrides existing or creates new config file with defaults" <<std::endl;
        std::cout << "-writeconfig -hwvirtual | selects the virtual hardware which enables testing without hardware" <<std::endl;
        std::cout << "-writeconfig -hwdk      | selects the development kit DK hardware (RPI SPI interface)" <<std::endl;
        std::cout << "-writeconfig -hwprod1   | selects the production hardware version 1 (Marlin XY)" <<std::endl;
        std::cout << "-writeconfig -hwprod2   | selects the production hardware version 2 (Marlin CoreXY)" <<std::endl;
        std::cout << "-autoplayer             | enables the automatic player mode - table will make random moves" <<std::endl;
        std::cout << "-skipplacementdialog    | skips the initial chess board NFC scan and load the default fen from the config file" <<std::endl;
        std::cout << "-preventflipscreen      | ignore gui flip screen config" <<std::endl;
        std::cout << "---- END ATC_CONTROLLER HELP ----" <<std::endl;
        return 0;
    }

    //---- PRINT VERSION MESSAGE ------------------------------------------------------ //
    if (cmdOptionExists(argv, argv + argc, "-version"))
    {
        std::cout << "---- ATC_CONTROLLER VERSION ----" <<std::endl;
        std::cout << "atc_controller version:"<< ATC_CONTROLLER_VERSION <<std::endl;
        std::cout << "atc_controller build date:"<< __DATE__ <<std::endl;
        std::cout << "atc_controller build date:"<< __TIME__ <<std::endl;
        return 0;
    }







    //SETUP LOGGER
    loguru::init(argc, argv);
    loguru::add_file(LOG_FILE_PATH, loguru::Truncate, loguru::Verbosity_MAX);
    loguru::add_file(LOG_FILE_PATH_ERROR, loguru::Truncate, loguru::Verbosity_WARNING);


    loguru::g_stderr_verbosity = 1;
    LOG_SCOPE_F(INFO, "ATC CONTROLLER STARTED");



    //READ CONFIG FILE
    LOG_SCOPE_F(INFO, "LOADING CONFIG FILE ./atccontrollerconfig.ini");

    //GET HARDWARE REVISION
    std::string used_if_path = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE);
    std::string hwid = get_interface_mac_address(used_if_path);
    //IF INVALID TRY OTHER PATH IF

    if(hwid == INVALID_HWID){
        const size_t HWID_IF_ALTERNATIVES_COUNT = 5;
        const std::string HWID_IF_ALTERNATIVES[HWID_IF_ALTERNATIVES_COUNT] = { "eth0", "wlan0", "enp5s0","wlo1","docker0"};
        //CYCLE THOUGHT SOME ALTERNATIVES INTERFACE NAMES
        for(size_t i = 0; i < HWID_IF_ALTERNATIVES_COUNT;i++){
            LOG_SCOPE_F(INFO, "TRY ALTERNATIVE HWINTERFACE");
            LOG_SCOPE_F(INFO, HWID_IF_ALTERNATIVES[i].c_str());

            hwid = get_interface_mac_address(HWID_IF_ALTERNATIVES[i]);
            if(hwid != INVALID_HWID){
                used_if_path = HWID_IF_ALTERNATIVES[i]; //STORE USED INTERFACE NAME / PATH
                break;
            }
        }
        //used_if_path
    }
    LOG_F(INFO, (const char*)hwid.c_str());







    //OVERWRITE WITH EXISTSING CONFIG FILE SETTINGS
    bool new_config_file_written = false;
    if(!ConfigParser::getInstance()->loadConfigFile(CONFIG_FILE_PATH) || cmdOptionExists(argv, argv + argc, "-writeconfig"))
    {
        LOG_F(WARNING, "--- CREATE LOCAL CONFIG FILE -----");
        LOG_F(WARNING, "1. Failed to load atccontrollerconfig.ini");
        LOG_F(WARNING, "2. ARGUMENT -writeconfig SET => CREATE NEW CONFIG FILE FOR HWID");
        LOG_F(WARNING, hwid.c_str());
        //LOAD (PUPULATE) ALL CONFIG ENTRIES WITH THE DEFAULT CONFIG
        if (cmdOptionExists(argv, argv + argc, "-hwvirtual"))
        {
            LOG_F(WARNING, "ARGUMENT -hwvirtual SET");
            LOG_F(WARNING, "VIRTUAL/SIMULATED HARDWARE");
            ConfigParser::getInstance()->loadDefaults("VIRT");

        }else if (hwid == HWID_DK_HARDWARE || cmdOptionExists(argv, argv + argc, "-hwdk")){
            LOG_F(WARNING, "DK HARDWARE");
            ConfigParser::getInstance()->loadDefaults("DK");

        }else if (hwid == HWID_PROD_V1_HARDWARE || cmdOptionExists(argv, argv + argc, "-hwprod1")) {
            LOG_F(WARNING, "PROD_V1 HARDWARE");
            ConfigParser::getInstance()->loadDefaults("PROD_V1");
        }else if (hwid == INVALID_HWID || cmdOptionExists(argv, argv + argc, "-hwinvalid")){ //IF HWIF IS INVALID => RETURN ERROR
            LOG_F(ERROR, "--- GOT INVALID HWID TO AVOID DAMAGES ON HARDWARE => TERMINATE PROGRAM -----");
            std::raise(SIGINT);
        }else { //EVERY OTHER HWID IS PROD_V2 HWID
            LOG_F(WARNING, "PROD_V2 HARDWARE");
            ConfigParser::getInstance()->loadDefaults("PROD_V2");
        }

        //STORE INTERFACE PATH USED FOR HWID GENERATION
        ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE,used_if_path,"");

        //WRITE CONFIG FILE TO FILESYSTEM
        ConfigParser::getInstance()->createConfigFile(CONFIG_FILE_PATH, false);
        ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE,used_if_path,CONFIG_FILE_PATH);


        LOG_F(ERROR, "WRITE NEW CONFIGFILE DUE MISSING ONE atccontrollerconfig.ini");
        new_config_file_written = true;
    }
    LOG_F(INFO, "CONFIG FILE LOADED");




    //---- ENABLE AUTOPLAYER ------------------------------------------------------ //
    if (cmdOptionExists(argv, argv + argc, "-autoplayer"))
    {
        LOG_F(WARNING, "ARGUMENT -autoplayer SET");
        ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::GENERAL_ENABLE_RANDOM_MOVE_MATCH, "1", CONFIG_FILE_PATH);
    }



    //---- ENABLE UDP DISCOVERY FOR EXTERNAL EXTENTIONS LIKE THE DGT3000 EXTENTION --- ///
    UDPDiscovery udpdic = UDPDiscovery(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::NETWORK_UDP_DISCOVERABLE_USER_DATA));
    if(udpdic.enable_service(ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::NETWORK_ENABLE_UDP_DISCOVERABLE)) && ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::NETWORK_ENABLE_UDP_DISCOVERABLE)){
        LOG_F(INFO,"UDPDiscovery ENABLED");
    }else{
        LOG_F(INFO,"UDPDiscovery DISABLED");
    }

    //--- ENABLE DGT3000 EXTENTION ----------------------------------------------- ///
    //read_file_to_string
    //read_file_to_string
    std::string fwver_tb = read_file_to_string(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH));
    if(fwver_tb.empty()){
        fwver_tb = "-ATCTABLE-";
    }
    DGT3000Interface dgt3000if = DGT3000Interface(fwver_tb);
    dgt3000if.enable_service(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_RESERVED_EXTENTION_DGT3000_INTERFACE_ENABLED));
    dgt3000if.set_dgt3000_text("booting");



    //---------  INIT HARDWARE ----------------------------------- ///
    if(!HardwareInterface::getInstance()->check_hw_init_complete())
    {
        LOG_F(ERROR, "check_hw_init_complete failed");
        std::raise(SIGINT);
    }



    // ------------ INIT GUI ---------------------------------- ///
    std::string fwver = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH);
    std::string hwrev = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_HWREV_FILE_PATH);
    std::string bootpart = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_BOOT_PARTION_INFO_FILE_PATH);
    LOG_F(INFO, "FWVER: %s HWREV: %s BOOTPARTITION: %s",fwver.c_str(),hwrev.c_str(),bootpart.c_str());

    //SARTING GUI COMMUNICATOR PROCESS
    guicommunicator gui;
    //USE GUICOMMUNICATOR ONLY IF WE HAVE REAL HARDWARE
    if(!HardwareInterface::getInstance()->is_simulates_hardware()){
        LOG_F(INFO, "guicommunicator startig ipc thread");
        gui.start_recieve_thread();    //START RECEIEVE WEBSERVER

        //USER_GENERAL_EN_ATCGUI_COMMUNICATION
        bool cfg_res = false;
        if (ConfigParser::getInstance()->getBool(ConfigParser::CFG_ENTRY::GENERAL_EN_ATCGUI_COMMUNICATION, cfg_res)) {
            gui.enable_qt_communication(cfg_res);
        }
        //WAIT FOR GUI TO BECOME REACHABLE
        int gui_wait_counter = 0;
        //TRY X TIMES BEFORE RETURN AN ERROR
        const int GUI_WAIT_COUNTER_MAX = 100;
        while (!gui.check_guicommunicator_reachable())
        {
            gui_wait_counter++;
            if (gui_wait_counter > GUI_WAIT_COUNTER_MAX)
            {
                break;
            }
        }
        if(gui_wait_counter > GUI_WAIT_COUNTER_MAX)
        {
            LOG_F(WARNING, "guicommunicator check_guicommunicator_reachable check failed");
            std::raise(SIGINT);
        }
        //RESET THE UI TO A CLEAN STATE
        gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_RESET, guicommunicator::GUI_VALUE_TYPE::ENABLED);

        //ROTATE SCREEN IF NEEDED
        if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_QTUI_FLIP_ORIENTATION) && !cmdOptionExists(argv, argv + argc, "-preventflipscreen")){
            gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_SET_ORIENTATION_180, guicommunicator::GUI_VALUE_TYPE::ENABLED);
            flip_screen_state = true;
        }else{
            gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_SET_ORIENTATION_0, guicommunicator::GUI_VALUE_TYPE::ENABLED);
            flip_screen_state = false;
        }
        //SET PROCESSING SCREEN
        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
        //CHECK VERSION ON GUI SIDE
        if(gui.check_guicommunicator_version())
        {
            LOG_F(WARNING, "guicommunicator version check failed");
        }
        //DETERM THE HWID BY USING THE MAC ADDRESS OF THE OUTGOING INTERNFACE NAME
        //DISPLAY FIRMARE VERSION NUMBER
        gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_HWID_LABEL, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, hwid);
        gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_RANK_LABEL, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, fwver + "|" + hwrev + "|" + bootpart);
    }













    //INIT CHESSBOARD
    dgt3000if.set_dgt3000_text("init board");
    ChessBoard board;
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_WHITE_TURN);
    bool board_scan = false;
    //ASKS THE USER TO PLACE THE FIGURE CORRECTLY OR A SCAN SHOULD BE DONE
    if(cmdOptionExists(argv, argv + argc, "-skipplacementdialog") || ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG))
    {
        board_scan = false;
        LOG_F(WARNING, "USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG IS SET skip board scan");
    }else if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTOLOGIN)){
        board_scan = true;
    }else if(HardwareInterface::getInstance()->is_simulates_hardware()){
        board_scan = false;
    }

    //INIT THE CHESS BOARD MECHANIC
    //=> HOME, SETUP COILS
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
    gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);

    ChessBoard::BOARD_ERROR board_init_err = ChessBoard::BOARD_ERROR::ERROR;
    while (board_init_err != ChessBoard::BOARD_ERROR::INIT_COMPLETE)
    {
        //INIT BOARD
        board_init_err = board.initBoard(board_scan);
        //IF NO ERROR GO FURTHER
        if(board_init_err == ChessBoard::BOARD_ERROR::INIT_COMPLETE){
            break;
        }
        //CHECK FOR FIGURE MISSING ERROR
        if(board_scan && board_init_err == ChessBoard::BOARD_ERROR::FIGURES_MISSING){
            if (gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_A_OK_CANCEL, "FIGURE MISSING ! PLEASE PLACE ALL FIGURES INSIDE THE FIELDS (H1 to A8)", 5000) != guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_OK) {
                break;
            }
        }else{
            if (gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_A_OK_CANCEL, "BOARD_INIT_FAILED RETRY?", 5000) != guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_OK) {
                break;
            }
        }
    }





    //-------------- SEARCH FOR LOCAL ATC SERVER IN NETZWORK -------------------------- //
    std::string start_backend_url = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL);
    if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::NETWORK_UDP_DISCOVER_ATC_SERVER_ENABLE)){
        UDPServerDiscovery server_disc = UDPServerDiscovery();
        const std::string sds_epc = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::NETWORK_UDP_DISCOVER_ATC_SERVER_USER_DATA_EXPECTED);

        std::string reip = server_disc.search_for_server(sds_epc,6,1000);
        if(!reip.empty()){
            reip = "http://" + reip + ":" + ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::NETWORK_UDP_DISCOVER_ATC_SERVER_PORT);
            start_backend_url = reip;
        }
    }

    //CREATE GAME BACKEND INSTANCE => THIS IS THE CONNECTION TO THE GAME SERVER
    BackendConnector gamebackend(start_backend_url, ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE), hwid);
    //SET HEARTBEAT INTERVAL
    int gamebackend_heartbeat_interval = 5;
    ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::NETWORK_HEARTBEAT_INTERVAL_SECS, gamebackend_heartbeat_interval);
    gamebackend.setHearbeatCallInterval(gamebackend_heartbeat_interval);
    //NOW TRY TO CONNECT TO THE SERVER, WE USE SOME HARDCODED FALL BACK URLs
    std::string ALTERNATIVE_BACKEND_URL[] = { ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL),"http://atomicchess.de:3000", "http://marcelochsendorf.com:3000", "http://marcelochsendorf.com:3001", "http://marcelochsendorf.com:3002", "http://prodevmo.com:3001", "http://prodevmo.com:3002", "http://127.0.0.1:3000","http://127.0.0.1:3001" };
    //CHECK IF GAMESERVER IS REACHABLE ELSE TRY AND ITERATE THOUGH THE OTHER HARDCODED URLS
    volatile int abu_counter = 0;
    volatile bool abu_result = true;
    while (!gamebackend.check_connection()) {
        gamebackend.set_backend_base_url(ALTERNATIVE_BACKEND_URL[abu_counter]);
        LOG_F(WARNING, "gamebackend - change backendurl due prev not abariable");
        LOG_F(WARNING, "%s", ALTERNATIVE_BACKEND_URL[abu_counter].c_str());
#ifdef DEBUG
        gui.show_error_message_on_gui("" + gamebackend.get_backend_base_url() + "");
#endif
        abu_counter++;
        //MARK SEARCH FAILED
        if(abu_counter >= 6)
        {
            abu_result = false;
            break;
        }
    }
    //SET POINTER TO GAMEBACKEND FOR UPLOAD THE LOGFILES TO CLOUD IF CATCH A SIGNAL
    gamebackend_logupload = &gamebackend;
    //UPDATE GUI THAT NETWORK IS ONLINE
    if(abu_result) {
        gui.createEvent(guicommunicator::GUI_ELEMENT::NETWORK_STATUS, guicommunicator::GUI_VALUE_TYPE::ONLINE);
        //SHOW MESSAGEBOX IF THE CURRENT URL IS A DIFFERENT THAN IN THE CONFIG
        if(gamebackend.get_backend_base_url() != ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL))
        {
            if (gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "GAME_SERVER_URL_CHANGED:" + gamebackend.get_backend_base_url(), 100) != guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_OK) {
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
            }
        }
    }
    else {
        //CONNECTION FAILED => EXIT
        gui.createEvent(guicommunicator::GUI_ELEMENT::NETWORK_STATUS, guicommunicator::GUI_VALUE_TYPE::OFFLINE);
        gui.show_error_message_on_gui("Cant connect to game server. (ERR01) [" + gamebackend.get_backend_base_url() + "]");

        std::raise(SIGINT);
    }




    //------ INIT VOICE EXTENTION ---------------------------------------- ///
    //USES THE GAME_BACKEND FOR SERVER COMMUNICATION THE THIS HAVE TO INIT FIRST
    VoiceRecognitionExtention vr_extention(hwid,gamebackend.get_interface_name()); //USE THE SAME ETH INTERFACE AS BACKEND
    if(!HardwareInterface::getInstance()->is_simulates_hardware()){
        vr_extention.enable_service(true);
    }





    //PERFORM A LOGOUT => IF A PREVIOUS SESSION IS ACTIVE
    if(gamebackend.logout())
    {
        LOG_F(INFO, "gamebackend - LOGOUT SUCCESS");
    }else
    {
        LOG_F(ERROR, "gamebackend - logout failed");
        gui.show_error_message_on_gui("LOGOUT FAILED");

    }

    //AUTOLOGIN => IF SET PERFORM AN AUTOLOGIN AND GOTO THE MAIN MENU
    if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTOLOGIN))
    {
        if (gamebackend.login(BackendConnector::PLAYER_TYPE::PT_HUMAN) && !gamebackend.get_session_id().empty())
        {
            //LOAD USER CONFIG FROM SERVER (MAYBE)
            //IF NOT EXISTS UPLOAD THEM
            if(!gamebackend.download_config(ConfigParser::getInstance(), true)) {
                LOG_F(WARNING, "download_config failed - upload current config");
                gamebackend.upload_config(ConfigParser::getInstance());
            }
            //START HEARTBEAT THREAD
            if(gamebackend.start_heartbeat_thread()) {
                //SWITCH TO MAIN MENU
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);
                dgt3000if.set_dgt3000_text("main menu");
                //PLACE THE GOT SESSION ID ON THE INFO SCREEN
                gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_SESSIONID_LABEL, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, gamebackend.get_session_id());
                //SHOW PLAYERNAME ON INFO SCREEN
                gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_VERSION, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, "Playername: " + gamebackend.getPlayerProfile().friendly_name + " | " + gamebackend.getPlayerProfile().elo_rank_readable);


                //SET USER TO AUTO SEARCHING
                if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE))
                {
                    LOG_F(WARNING, "USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE IS SET call set_player_state");
                    gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_SEARCHING);
                }

                //IF NEW CONFIG FILE WAS CREATED => UPLOAD IT TO SERVER
                if(new_config_file_written){
                    gamebackend.upload_config(ConfigParser::getInstance());
                    loguru::flush();
                    gamebackend.upload_logfile(read_file_to_string(LOG_FILE_PATH_ERROR));
                }

            }else {
                gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOGIN_FAILED_HEARTBEAT", 4000);
                LOG_F(ERROR, "GOT LOGIN_FAILED_HEARTBEAT START THREAD");
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
            }
        }
    }
    else {
        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
    }



    //INIT SYSTEM TIMER
#ifdef __MACH__
    struct timespec t1;
    struct timespec t2;
#else
    TimePoint t1 = Clock::now();
	TimePoint t2 = Clock::now();
#endif

    int SYSTEM_TICK_DELAY = 0;
    if (!ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::GENERAL_SYSTEM_TICK_INTERVAL_MS, SYSTEM_TICK_DELAY)) {
        LOG_F(ERROR, "GENERAL_SYSTEM_TICK_INTERVAL_MS - config get entry  failed");
    }
    LOG_F(INFO, "SYSTEM_TICK_DELAY SET %i", SYSTEM_TICK_DELAY);

    //INIT SYSTEM STATE MACHINE
    StateMachine state_machiene;
    BackendConnector::PLAYER_STATUS current_player_state;
    DGT3000Buttons dgt_btn_state_old;
    //DISCARD ALL GUI EVENTS
    gui.clearPreviousEvents();
    //ENTERING MIAN LOOP
    HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);
    while (mainloop_running == 0)
    {
//SOME PREPOS DEFINES FOR MAC OSX
#ifdef __MACH__
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        t2.tv_sec = mts.tv_sec;
        t2.tv_nsec = mts.tv_nsec;
        //IF TIME DIFFERENCE IS BIG ENOUGHT => CALL THE TIMER EVENT
        if((t2.tv_sec -t1.tv_sec) > SYSTEM_TICK_DELAY)
        {
            host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
            clock_get_time(cclock, &mts);
            mach_port_deallocate(mach_task_self(), cclock);
            t1.tv_sec = mts.tv_sec;
            t1.tv_nsec = mts.tv_nsec;
#else
            t2 = Clock::now();
		//IF TIME DIFFERENCE IS BIG ENOUGHT => CALL THE TIMER EVENT
		if(CHRONO_DURATION_MS(t2, t1) > SYSTEM_TICK_DELAY)
		{
			t1 = Clock::now();
#endif

            //IF A VALID SESSION
            if(gamebackend.check_login_state())
            {
                //THE THE PLAYER STATUS
                //WHICH CONTAINS ALL INFORMATION ABOUT THE CURRENT STATE OF GAME / MATCHMAKING / GENERAL STATE OF THE CLIENT
                current_player_state = gamebackend.get_player_state();
                //BASIC ERROR HANDLING
                //HANDLING LOGOUT / INVALID SESSION
                if(current_player_state.err == "err_session_key_sid_check_failed" || current_player_state.err == "err_session_check_failed" || current_player_state.err == "err_session_key_sid_check_failed") {
                    if (gamebackend.stop_heartbeat_thread())
                    {
                        gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOGOUT_FAILED_HEARTBEAT_STOP", 4000);
                        LOG_F(ERROR, "GOT LOGIN_FAILED_HEARTBEAT STOP THREAD");
                    }

                    gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);

                }else if(current_player_state.err == "err_query_paramter_hwid_or_sid_or_not_set")
                {
                    gui.show_error_message_on_gui("get_player_state - ERROR err_query_paramter_hwid_or_sid_or_not_set");
                    LOG_F(ERROR, "GOT err_query_paramter_hwid_or_sid_or_not_set");
                }


                if(current_player_state.game_state.game_running)
                {
                    //IN THIS SECIONT THE IMMEDIATES STATE WILL BE HANDLED
                    //EG FROM NO_GAME_RUNNING TO GAME_RUNNING, THE SCREEN HAVE TO BE SWITCHED, ...

                    //IF A GAME IS RUNNING UPDATE THE UI
                    update_gamescreen_with_player_state(current_player_state, gui);

                    //IF BOARD IS IN SYNC PHASE => SETUP BOARD BIECES
                    if (current_player_state.game_state.is_syncing_phase)
                    {
                        game_running_state =1;
                        make_move_mode = 0;
                        //SHOW THE PROCESSING SCREEN
                        //gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::GAME_SCREEN);
                        HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
                        //LOAD THE FEN TO THE TARGET BOARD
                        //IF RETURNS FALSE => THE FEN IS INVALID
                        if(!board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD))
                        {
                            gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "INVALID BOARD FEN - CANCEL GAME", 4000);
                            LOG_F(ERROR, current_player_state.game_state.current_board_fen.c_str());
                            gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_IDLE);
                            gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);
                        }
                        //NOW SYNC THE TWO BOARDS => THE TARGET CHESS POSITIONS WITH THE REAL WORLD MECHANICAL POSTIONS
                        if(board.syncRealWithTargetBoard())
                        {
                            //BOARD SYNCED => UPDATE STATE ON SERVER THAT BOARD IS SYNCED
                            gamebackend.set_player_setup_confirmation(BackendConnector::PLAYER_SETUP_STATE::PSP_READY);
                            gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::GAME_SCREEN);
                        }else{
                            //BOARD INIT FAILED => SHOW OPTION TO CONTIOUE
                            if(gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_A_OK_CANCEL, "BOARD SETUP FAILED? CONTINUE", 4000) == guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_OK)
                            {
                                //SEND BOARD INIT COMPLETE CALL
                                gamebackend.set_player_setup_confirmation(BackendConnector::PLAYER_SETUP_STATE::PSP_READY);
                                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::GAME_SCREEN);
                            }else{
                                //RESET PLAYER STATE => ABORTS GAME
                                gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_IDLE);
                                //GO BACK TO MAIN_MENU SCREEN
                                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);
                            }
                        }


                    //ELSE IF THE OPPONEND TURN
                    }else if (!current_player_state.game_state.is_my_turn) {
                        make_move_mode = 0;
                        //SET TURN LIGHT RIGHT
                        if (current_player_state.game_state.im_white_player)
                        {
                            dgt3000if.set_dgt3000_text("--WAIT --");
                            HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_BLACK_TURN);
                        }
                        else
                        {
                            HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_WHITE_TURN);
                        }
                        //SYNC BOARDS
                        board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD);
                        board.syncRealWithTargetBoard();
                        make_move_mode = 0;
                    //ELSE IF IF MY TURN
                    }else if (current_player_state.game_state.is_my_turn) {//IS MY TURN TRIGGER DIALOG
                        //SET TURN LIGHT RIGHT
                        if (current_player_state.game_state.im_white_player)
                        {
                            dgt3000if.set_dgt3000_text("YOUR TURN WHITE");
                            HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_WHITE_TURN);
                        }
                        else
                        {
                            dgt3000if.set_dgt3000_text("YOUR TURN BLACK");
                            HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_BLACK_TURN);
                        }

                        //SYNC BOARDS
                        board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD);
                        board.syncRealWithTargetBoard();



                        //ENABLE AUTO PLAY FEATURE
                        if(cmdOptionExists(argv, argv + argc, "-autoplayer") || ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_RANDOM_MOVE_MATCH)) {
                            if (current_player_state.game_state.legal_moves.size() > 0)
                            {
                                const int rnd_idnex = (int)(std::rand() % (current_player_state.game_state.legal_moves.size() - 1));
                                if(!gamebackend.set_make_move(current_player_state.game_state.legal_moves.at(rnd_idnex))){
                                }
                            }else{
                                gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LEGAL_MOVES_EMPTY - CANCEL GAME", 4000);
                                LOG_F(ERROR, "LEGAL_MOVES_EMPTY - CANCEL GAME");
                            }
                        }else{
                            //ELSE SHOW MANUAL MOVE ENTER SCREEN
                            if (make_move_mode != 2)
                            {
                                //SHOW UP MAKE MANUAL MOVE GUI
                                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);
                                make_move_mode = 2;
                                //STORE ALL POSSIBLE MOVES
                                possible_moves = current_player_state.game_state.legal_moves; //STORE MOVES
                               //ENABLE BOARD AUTOSCAN AFTER X SECONDS
                                if(!ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::USER_RESERVED_AUTO_SCAN_BOARD_TIME_IF_USERS_TURN,make_move_scan_timer)){
                                    make_move_scan_timer = -1;
                                }


                                //RESET VR EXTENTION
                                vr_extention.reset_move();

                            //AUTO TIMER FUNCTION
                            }else if(make_move_mode == 2){
                                //IF TIMER = 0 TRIGGER EVENT FOR SCAN
                                if(make_move_scan_timer == 0){
                                    //TRIGGER A SCAN EVENT
                                    player_enter_manual_move_start_board_scan(gui,board,gamebackend,possible_moves);
                                    //DISBALE TIMER
                                    make_move_scan_timer = -1;
                                }else if(make_move_scan_timer > 0){
                                    make_move_scan_timer--;
                                }



                                //PROCESS EXTENTIONS
                                //EXNTENTION HANDLING ENABLE ALEXA SUPPORT
                                const VoiceRecognitionExtention::VR_MOVE vre_move = vr_extention.get_move();
                                ChessPiece::COLOR owcol = ChessPiece::COLOR_UNKNOWN;
                                if (current_player_state.game_state.im_white_player)
                                {
                                    owcol = ChessPiece::COLOR_WHITE;
                                }else{
                                    owcol = ChessPiece::COLOR_BLACK;
                                }

                                if(vre_move.valid){
                                    //PARSE STRINGS INTO USABLE OBJECTS
                                    const ChessPiece::FIGURE start_figure =  ChessPiece::getFigureByName(vre_move.figure,owcol);
                                    if(start_figure.type == ChessPiece::TYPE_INVALID){
                                        vr_extention.reset_move();

                                    }else {
                                        ChessBoard::MovePiar mvpair_tmp;
                                        if (vre_move.to.length() == 4) {
                                            mvpair_tmp = board.StringToMovePair(vre_move.to);
                                        } else {
                                            mvpair_tmp = board.StringToMovePair(vre_move.to + "a1");
                                        }

                                        const ChessField::CHESS_FILEDS dest_field = mvpair_tmp.from_field; //A1xx B3xx => hacked usage of StringToMovePair

                                        //GET A POSSIBLE MOVE BY USING THE MOVES LIST
                                        const std::vector<ChessBoard::MovePiar> mves = board.StringToMovePair(
                                                possible_moves, true);
                                        const ChessBoard::PossibleUserMoveResult mvpair = board.calculcate_move_from_figure_and_destination(
                                                start_figure, dest_field, mves);

                                        if (mvpair.error == ChessBoard::BOARD_ERROR::NO_ERROR) {
                                            //PARSE MOVE TO STRING MOVE
                                            const std::string mv =
                                                    ChessField::field_to_string(mvpair.possible_move.from_field) +
                                                    ChessField::field_to_string(mvpair.possible_move.to_field);
                                            //MAKE MOVE
                                            if (!mv.empty() && gamebackend.set_make_move(mv)) {
                                            }
                                            //MOVE SUCCESS RESET IT
                                            vr_extention.reset_move();
                                        } else {
                                            //MOVE SUCCESS RESET IT
                                            vr_extention.reset_move();
                                        }

                                    }
                                }

                            }




                            board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD);
                            board.syncRealWithTargetBoard();

                        }
                    }else if(current_player_state.game_state.is_game_over) {
                        //IS GAME OVER => ABORT GAME TO SERVER AND  GOTO MAIN MENU
                        board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD);
                        board.syncRealWithTargetBoard();
                        //SET GAME OVER AND GOTO MAIN MENU
                        gamebackend.set_abort_game();
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);
                    }else{
                        //SYNC THE TWO BORDS
                        board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD);
                        board.syncRealWithTargetBoard();
                    }
                    
                }else{
                    //RETURN TO THE MAIN MENU IF GAME WAS STARTEN AND NOW EXITED
                    if(game_running_state == 1){

                        game_running_state = 2;
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);
                    }
                }
            }
        }


		//player_enter_manual_move_start_board_scan(gui,board,gamebackend,possible_moves);
        DGT3000Buttons dgt_btn_state = dgt3000if.get_dgt3000_buttons_state();
		if(dgt_btn_state.is_event_valid){
		    if(dgt_btn_state.play_btn){
                if(!gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_SEARCHING)) {
                    dgt3000if.set_dgt3000_text("SEARCHING");
                    gui.show_error_message_on_gui("ENABLE MATCHMAKING FAILED");
                    LOG_F(WARNING, "ENABLE MATCHMAKING FAILED TRIGGERED BY USER BUTTON");
                }
		    }
            if(current_player_state.game_state.game_running && current_player_state.game_state.is_my_turn) {
                //CHECK IF LEVER POSITION HAS CHANGED
                if (dgt_btn_state_old.lever_right_down != dgt_btn_state.lever_right_down) {
                    gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                    dgt3000if.set_dgt3000_text("PROCESSING");
                    player_enter_manual_move_start_board_scan(gui, board, gamebackend, possible_moves);
                }
            }
            dgt_btn_state_old = dgt_btn_state;
		}





        //HANDLE UI EVENTS UI LOOP
        guicommunicator::GUI_EVENT ev = gui.get_gui_update_event();
        if (ev.is_event_valid) {

            //-----------------------------------------------------------
            //---------------- PROCESS EVENTS ---------------------------
            //-----------------------------------------------------------
            if (ev.event == guicommunicator::GUI_ELEMENT::BEGIN_BTN_SCAN &&
                ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //-----------------------------------------------------------
                //----------------LOGIN BUTTON WITH BOARD INIT AND NFC SCAN--
                //-----------------------------------------------------------
                //PERFORM A LOGIN AS HUMAN
                if (gamebackend.login(BackendConnector::PLAYER_TYPE::PT_HUMAN) &&
                    !gamebackend.get_session_id().empty()) {
                    //LOAD USER CONFIG FROM SERVER (MAYBE)
                    //IF NOT EXISTS UPLOAD THEM
                    if (!gamebackend.download_config(ConfigParser::getInstance(), true)) {
                        LOG_F(WARNING, "download_config failed - upload current config");
                        gamebackend.upload_config(ConfigParser::getInstance());
                    }
                    //START HEARTBEAT THREAD
                    if (gamebackend.start_heartbeat_thread()) {
                        //PLACE THE GOT SESSION ID ON THE INFO SCREEN
                        gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_SESSIONID_LABEL,
                                        guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING,
                                        gamebackend.get_session_id());
                        //SHOW PLAYERNAME ON INFO SCREEN
                        gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_VERSION,
                                        guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING,
                                        "Playername: " + gamebackend.getPlayerProfile().friendly_name + " | " +
                                        gamebackend.getPlayerProfile().elo_rank_readable);
                        //NOW INIT BOARD AGAIN WITH SCAN
                        HardwareInterface::getInstance()->setTurnStateLight(
                                HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                        guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                        board.initBoard(true);
                        HardwareInterface::getInstance()->setTurnStateLight(
                                HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                        guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);
                        //SET USER TO AUTO SEARCHING
                        if (ConfigParser::getInstance()->getBool_nocheck(
                                ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE)) {
                            gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_SEARCHING);
                        }
                    } else {
                        gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                             "LOGIN_FAILED_HEARTBEAT", 4000);
                        LOG_F(ERROR, "GOT LOGIN_FAILED_HEARTBEAT START THREAD");
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                        guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
                    }
                } else {
                    gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOGIN_FAILED", 4000);
                    LOG_F(ERROR, "GOT LOGIN FAILED");
                    gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                    guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
                }

            } else if (ev.event == guicommunicator::GUI_ELEMENT::BEGIN_BTN_DEFAULT &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //-------------------------------------------------------------
                //LOGIN TO SERVER WITH INIT BOARD WITH DEFAULT FIGURE POSTION--
                //-------------------------------------------------------------
                //PERFORM A LOGIN AS HUMAN
                if (gamebackend.login(BackendConnector::PLAYER_TYPE::PT_HUMAN) &&
                    !gamebackend.get_session_id().empty()) {
                    //LOAD USER CONFIG FROM SERVER (MAYBE)
                    //IF NOT EXISTS UPLOAD THEM
                    if (!gamebackend.download_config(ConfigParser::getInstance(), true)) {
                        LOG_F(WARNING, "download_config failed - upload current config");
                        gamebackend.upload_config(ConfigParser::getInstance());
                    }
                    //START HEARTBEAT THREAD
                    if (gamebackend.start_heartbeat_thread()) {
                        //PLACE THE GOT SESSION ID ON THE INFO SCREEN
                        gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_SESSIONID_LABEL,
                                        guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING,
                                        gamebackend.get_session_id());
                        //SHOW PLAYERNAME ON INFO SCREEN
                        gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_VERSION,
                                        guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING,
                                        "Playername: " + gamebackend.getPlayerProfile().friendly_name + " | " +
                                        gamebackend.getPlayerProfile().elo_rank_readable);

                        //NOW INIT BOARD AGAIN WITH SCAN
                        HardwareInterface::getInstance()->setTurnStateLight(
                                HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                        guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                        //INIT BOARD
                        board_init_err = board.initBoard(false);
                        if (board_init_err == ChessBoard::BOARD_ERROR::FIGURES_MISSING) {
                            //FIGURE IS MISSING ON BOARD
                            gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                                 "FIGURES MISSING - PLEASE CHECK ON BOARD", 10000);
                            gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                            guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);

                        } else if (board_init_err == ChessBoard::BOARD_ERROR::INIT_COMPLETE) {
                            //IF NO ERROR GOTO MAIN MENU
                            gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                            guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);
                            HardwareInterface::getInstance()->setTurnStateLight(
                                    HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);
                            //SET USER TO AUTO SEARCHING
                            if (ConfigParser::getInstance()->getBool_nocheck(
                                    ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE)) {
                                gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_SEARCHING);
                            }

                        } else {
                            //UNKNOWN ERROR => GO BACK LOGIN SCREEN
                            gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                                 "BOARD INIT FAILED UNKNOWN ERROR", 10000);
                            gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                            guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
                        }


                    } else {
                        gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                             "LOGIN_FAILED_HEARTBEAT", 4000);
                        LOG_F(ERROR, "GOT LOGIN_FAILED_HEARTBEAT START THREAD");
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                        guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
                    }

                } else {
                    gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOGIN_FAILED", 4000);
                    LOG_F(ERROR, "GOT LOGIN FAILED");
                    gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                    guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
                }


            } else if (ev.event == guicommunicator::GUI_ELEMENT::INIT_BTN &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------BOARD INIT BUTTON-----------------------
                //--------------------------------------------------------
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                //INIT BOARD ON PRODUCTATION HARDWARE ALWAYS
                board_init_err = board.initBoard(
                        board_scan | HardwareInterface::getInstance()->is_production_hardware());
                //CHECK FOR FIGURE MISSING ERROR
                if (board_init_err == ChessBoard::BOARD_ERROR::FIGURES_MISSING) {
                    //gui.show_error_message_on_gui("FIGURES MISSING");
                    gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                         "FIGURES MISSING - PLEASE CHECK ON BOARD", 10000);
                } else if (board_init_err != ChessBoard::BOARD_ERROR::INIT_COMPLETE) {
                    //gui.show_error_message_on_gui("BOARD INIT FAILED UNKNOWN ERROR");
                    gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                         "BOARD INIT FAILED UNKNOWN ERROR", 10000);
                }
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::SETTINGS_SCREEN);


            } else if ((ev.event == guicommunicator::GUI_ELEMENT::SCAN_BOARD_BTN) &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------CALIBRATE BOARD BTN---------------------
                //--------------------------------------------------------
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                if (board.calibrate_home_pos() == ChessBoard::BOARD_ERROR::NO_ERROR) {
                    gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                         "TABLE REACHED HOME POSITION", 10000);
                } else {
                    gui.show_error_message_on_gui("board.initBoard() FAILED");
                }
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::SETTINGS_SCREEN);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_B &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - LOAD CONFIG BUTTON--------------
                //--------------------------------------------------------
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                ConfigParser::getInstance()->createConfigFile(CONFIG_FILE_PATH, true);
                gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOADED DEFAULT CONFIG",
                                     10000);
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::SETTINGS_SCREEN);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_C &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - MAKE MOVE --------------
                //--------------------------------------------------------
                gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK,
                                     "MOVE TEST POPULATE CHESSBOARD IN START POSITION", 10000);
                board.test_make_move_func();
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::DEBUG_SCREEN);


            } else if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_D &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - CORNER TEST--------------
                //--------------------------------------------------------
                board.corner_move_test();

            }
            if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_E &&
                ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - UPLOAD CONFIG BUTTON------------
                //--------------------------------------------------------
                gamebackend.upload_config(ConfigParser::getInstance());

            } else if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_F &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - UPLOAD LOG BUTTON---------------
                //--------------------------------------------------------
                LOG_F(WARNING, "MANUAL LOG UPLOAD");
                LOG_F(WARNING, LOG_FILE_PATH_ERROR);
                loguru::flush();
                //IF GOT A SIGNAL READ LOGFILE AND UPLOAD THEM
                std::string log = read_file_to_string(LOG_FILE_PATH_ERROR);
                if (!log.empty()) {
                    gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                    guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                    gamebackend.upload_logfile(log);
                    gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                    guicommunicator::GUI_VALUE_TYPE::DEBUG_SCREEN);
                }

            } else if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_H &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - DOWNLOAD CONFIG BUTTON----------
                //--------------------------------------------------------
                LOG_F(WARNING, "DEBUG -SHOW MAKE MOVE SCREEN ");
                make_move_mode = 1;
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_G &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - DOWNLOAD CONFIG BUTTON----------
                //--------------------------------------------------------
                LOG_F(WARNING, "DEBUG - DOWNLOAD CONFIG BUTTON TRIGGERED ");
                gamebackend.download_config(ConfigParser::getInstance(), true);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_I &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------DEBUG - FLIP SCREEN---------------------
                //--------------------------------------------------------
                LOG_F(WARNING, "DEBUG - FLIP ROATION ");
                flip_screen_state = !flip_screen_state;
                if (flip_screen_state) {
                    gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_SET_ORIENTATION_180,
                                    guicommunicator::GUI_VALUE_TYPE::ENABLED);
                } else {
                    gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_SET_ORIENTATION_0,
                                    guicommunicator::GUI_VALUE_TYPE::ENABLED);
                }

            } else if (ev.event == guicommunicator::GUI_ELEMENT::LOGOUT_BTN &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------LOGOUT BUTTON --------------------------
                //--------------------------------------------------------
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
                //LOGOUT AND GOTO LOGIN SCREEN
                //if(gamebackend.stop_heartbeat_thread()) {
                gamebackend.logout();
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
                //}

            } else if (ev.event == guicommunicator::GUI_ELEMENT::MAINMENU_START_AI_MATCH_BTN &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::ENABLED) {
                //--------------------------------------------------------
                //----------------ENABLE MATCHMAKING BUTTON --------------
                //--------------------------------------------------------
                //SET PLAYERSTATE TO OPEN FO A MATCH
                if (!gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_SEARCHING)) {
                    gui.show_error_message_on_gui("ENABLE MATCHMAKING FAILED");
                    LOG_F(WARNING, "ENABLE MATCHMAKING FAILED TRIGGERED BY USER BUTTON");
                }

            } else if (ev.event == guicommunicator::GUI_ELEMENT::MAINMENU_START_AI_MATCH_BTN &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::DISBALED) {
                //--------------------------------------------------------
                //----------------DISBALE MATCHMAKING BUTTON -------------
                //--------------------------------------------------------
                //SET PLAYERSTATE TO OPEN FO A MATCH
                if (!gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_IDLE)) {
                    gui.show_error_message_on_gui("DISBALE MATCHMAKING FAILED");
                    LOG_F(WARNING, "DISBALE MATCHMAKING FAILED TRIGGERED BY USER BUTTON");
                }

            } else if (ev.event == guicommunicator::GUI_ELEMENT::GAMESCREEN_ABORT_GAME &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------ABORT GAME BUTTON ----------------------
                //--------------------------------------------------------
                //SET PLAYERSTATE TO OPEN FO A MATCH
                //if(gamebackend.set_abort_game()) {

                //}
                //gui.show_error_message_on_gui("GAME STOPPED");
                LOG_F(WARNING, "GAME STOPPED TRIGGERED BY USER BUTTON");
                gamebackend.logout();
                gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_RESET, guicommunicator::GUI_VALUE_TYPE::ENABLED);
                //ROTATE SCREEN IF NEEDED
                if (ConfigParser::getInstance()->getBool_nocheck(
                        ConfigParser::CFG_ENTRY::HARDWARE_QTUI_FLIP_ORIENTATION) &&
                    !cmdOptionExists(argv, argv + argc, "-preventflipscreen")) {
                    gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_SET_ORIENTATION_180,
                                    guicommunicator::GUI_VALUE_TYPE::ENABLED);
                    flip_screen_state = true;
                } else {
                    gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_SET_ORIENTATION_0,
                                    guicommunicator::GUI_VALUE_TYPE::ENABLED);
                    flip_screen_state = false;
                }
                gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::PLAYER_EMM_INPUT &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING && make_move_mode > 0) {
                //--------------------------------------------------------
                //----------------PLAYER MAKE MANUAL MOVE-----------------
                //--------------------------------------------------------
                make_move_scan_timer = -1;
                ChessBoard::MovePiar mvpair = board.StringToMovePair(ev.value);
                if (mvpair.is_valid) {
                    //FOR TEST
                    if (make_move_mode == 1) {
                        //PERFORM THE MOVE ON THE BOARD => MOVE FIGURES
                        board.makeMoveSync(mvpair, false, false, false);
                        if (ConfigParser::getInstance()->getBool_nocheck(
                                ConfigParser::CFG_ENTRY::USER_RESERVED_MAKE_MOVE_MANUAL_TEST_DO_SYNC)) {
                            LOG_F(INFO,
                                  "USER_RESERVED_MAKE_MOVE_MANUAL_TEST_DO_SYNC IS SET => PERFORMING  board.syncRealWithTargetBoard();");
                            board.syncRealWithTargetBoard();
                        }
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                        guicommunicator::GUI_VALUE_TYPE::DEBUG_SCREEN);
                        make_move_mode = 0;
                    } else if (make_move_mode == 2) {
                        //FOR RUNNING GAME FIRST CHECK VIA BACKEN IF THE MOVE IS VALID
                        gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                        guicommunicator::GUI_VALUE_TYPE::GAME_SCREEN);
                        if (gamebackend.set_make_move(ev.value)) {
                            //board.makeMoveSync(board.StringToMovePair(ev.value),false,true,false);
                            //board.syncRealWithTargetBoard();
                            make_move_mode = 0; //MOVE VALID
                        } else {
                            make_move_mode = 2;
                            gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU,
                                            guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);
                        }
                    }
                }

            } else if (ev.event == guicommunicator::GUI_ELEMENT::PLAYER_EMM_SCAN_BOARD &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED &&
                       make_move_mode > 0) { //THE SCNA BOARD FOR MOVE BTN
                //--------------------------------------------------------
                //--LAYER ENTER MANUAL MOVE SCAN BOARD FOR MOVE BUTTON ---
                //--------------------------------------------------------
                player_enter_manual_move_start_board_scan(gui, board, gamebackend, possible_moves);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_H1POS &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------CALIBRATION SCREEN ---------------------
                //--------------------------------------------------------
                HardwareInterface::getInstance()->set_speed_preset(
                        HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
                cal_move = 0;
                cal_move_step = 5; //SET USER ARROW KEY TO 5mm PER PRESS
                LOG_F(WARNING, "CALIBRATION SCREEN - H1 POSITION");
                //MOVE HOME POS
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                HardwareInterface::getInstance()->home_sync();

                //READ CONFIG VALUES
                cal_pos_x = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X);
                cal_pos_y = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y);
                //MOVE TO NEW H1 POSITION
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);


            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_A8POS &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //--------------------------------------------------------
                //----------------CALIBRATION SCREEN CAL A8 POSITION------
                //--------------------------------------------------------
                HardwareInterface::getInstance()->set_speed_preset(
                        HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
                cal_move = 1;
                cal_move_step = 5; //SET USER ARROW KEY TO 5mm PER PRESS
                LOG_F(WARNING, "CALIBRATION SCREEN - H1 POSITION");
                //MOVE HOME POS
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                HardwareInterface::getInstance()->home_sync();

                //READ CONFIG VALUES
                //THE CHESS BOARD IS SQUARED SO X AND Y ARE DIAGONALLY THE SAME
                cal_pos_x = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X) * 7;
                cal_pos_y = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y) * 7;

                //MOVE TO NEW A8 POSITION
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                        true);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, true);
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x +
                                                                              ConfigParser::getInstance()->getInt_nocheck(
                                                                                      ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                                                                              cal_pos_y +
                                                                              ConfigParser::getInstance()->getInt_nocheck(
                                                                                      ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                                                                              true);


            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_PPBLACK1 &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                HardwareInterface::getInstance()->set_speed_preset(
                        HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
                cal_move = 2;
                cal_move_step = 2; //SET USER ARROW KEY TO 2mm PER PRESS
                LOG_F(WARNING, "CALIBRATION SCREEN -PARK POSITION BLACK 1");
                //MOVE HOME POS
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                HardwareInterface::getInstance()->home_sync();
                //MOVE H1
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                        true);
                //ENABLE COIL FOR RIGHT SITE
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
                //MOVE TO PARK POS BLACK 1
                cal_pos_x = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE);
                cal_pos_y = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET);
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_PPWHITE1 &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                HardwareInterface::getInstance()->set_speed_preset(
                        HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
                cal_move = 3;
                cal_move_step = 2; //SET USER ARROW KEY TO 2mm PER PRESS
                LOG_F(WARNING, "CALIBRATION SCREEN -PARK POSITION WHITE 1");
                //MOVE HOME POS
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                HardwareInterface::getInstance()->home_sync();
                //MOVE H1
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                        true);
                //ENABLE COIL FOR RIGHT SITE
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, true);
                //MOVE TO PARK POS BLACK 1
                cal_pos_x = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE);
                cal_pos_y = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET);
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_PPBLACK16 &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                HardwareInterface::getInstance()->set_speed_preset(
                        HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
                cal_move = 4;
                cal_move_step = 2; //SET USER ARROW KEY TO 2mm PER PRESS
                LOG_F(WARNING, "CALIBRATION SCREEN -PARK POSITION BLACK 16");
                //MOVE HOME POS
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                HardwareInterface::getInstance()->home_sync();
                //MOVE H1
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                        ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                        true);
                //ENABLE COIL FOR RIGHT SITE
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
                //MOVE TO PARK POS BLACK 16
                cal_pos_y = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET);
                //GET THE Y POSITION OF THE 16th PARK POS
                cal_pos_y += (16 - 1) * ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE);

                //FIRST MOVE TO PP BLACK 16 BEFORE ENTRY
                cal_pos_x = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE);
                cal_pos_x = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET);
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);
                //THEN INTO THE PARK POSITION
                cal_pos_x = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE);
                HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_MVUP &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                LOG_F(WARNING, "CALIBRATION SCREEN - UP");
                //MOVE TO NEW POSITION
                cal_pos_y += cal_move_step;

                //WORKAROUND FOR A8
                if (cal_move == 1) {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                                                                                  cal_pos_y +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                                                                                  true);
                } else {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);
                }


            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_MVDOWN &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                LOG_F(WARNING, "CALIBRATION SCREEN - DOWN");
                //MOVE TO NEW POSITION
                cal_pos_y -= cal_move_step;
                //WORKAROUND FOR A8
                if (cal_move == 1) {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                                                                                  cal_pos_y +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                                                                                  true);
                } else {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);
                }
            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_MVLEFT &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                LOG_F(WARNING, "CALIBRATION SCREEN - LEFT");
                //MOVE TO NEW POSITION
                cal_pos_x += cal_move_step;
                //WORKAROUND FOR A8
                if (cal_move == 1) {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                                                                                  cal_pos_y +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                                                                                  true);
                } else {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);
                }
            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_MVRIGHT &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                LOG_F(WARNING, "CALIBRATION SCREEN - RIGHT");
                //MOVE TO NEW POSITION
                cal_pos_x -= cal_move_step;

                //WORKAROUND FOR A8
                if (cal_move == 1) {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X),
                                                                                  cal_pos_y +
                                                                                  ConfigParser::getInstance()->getInt_nocheck(
                                                                                          ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y),
                                                                                  true);
                } else {
                    HardwareInterface::getInstance()->move_to_postion_mm_absolute(cal_pos_x, cal_pos_y, true);
                }

                //SAVE CALIBRAION DATA
            } else if (ev.event == guicommunicator::GUI_ELEMENT::CALIBRATIONSCREEN_SAVE &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                LOG_F(WARNING, "CALIBRATION SCREEN - SAVE");
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);

                //INVALID SAVE COMMAND
                if (cal_move == -1) {

                    gui.show_error_message_on_gui("CALIBRATION SAVE FAILED -> PLEASE SELECT CONRNER");

                    //SAVE H1
                } else if (cal_move == 0) {
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X, cal_pos_x,
                                                        CONFIG_FILE_PATH);
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y, cal_pos_y,
                                                        CONFIG_FILE_PATH);
                    gui.show_error_message_on_gui("CALIBRATION SAVED FOR H1");
                    LOG_F(INFO, "CALIBRATION SAVED FOR H1 H1_OFFSET_MM_X %i H1_OFFSET_MM_Y %i", cal_pos_x, cal_pos_y);
                    //SAVE A8
                } else if (cal_move == 1) {

                    //ONLY DIV 7 DUE 8 but only 7 moves from a1->a8 or a1->h1
                    cal_pos_x = cal_pos_x / (8 - 1);
                    cal_pos_y = cal_pos_y / (8 - 1);

                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X,
                                                        cal_pos_x, CONFIG_FILE_PATH); //WRITE FIELD WIDTH
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y,
                                                        cal_pos_y,
                                                        CONFIG_FILE_PATH); //= FW*8 WRITE BOARD WITH = NEEDED FOR ChessBoardClass
                    gui.show_error_message_on_gui("CALIBRATION SAVED FOR A8");
                    LOG_F(INFO, "CALIBRATION SAVED FOR A8 FILED_WIDTH %i, BOARD_WIDTH X:%i Y:%i", cal_pos_x, cal_pos_y);

                    //SAVE PARKPOS BLACK 1 => LINE OFFSET X AND START
                } else if (cal_move == 2) {

                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE,
                                                        cal_pos_x, CONFIG_FILE_PATH);
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET,
                                                        cal_pos_y, CONFIG_FILE_PATH);
                    gui.show_error_message_on_gui("CALIBRATION SAVED FOR PARK POSITION BLACK 1");
                    LOG_F(INFO,
                          "CALIBRATION SAVED FOR PARK POS BLACK 1 MECHANIC_PARK_POS_BLACK_X_LINE %i MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET %i",
                          cal_pos_x, cal_pos_y);

                    //SAVE PARKPOS WHITE 1 => LINE OFFSET X AND START
                } else if (cal_move == 3) {
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE,
                                                        cal_pos_x, CONFIG_FILE_PATH);
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET,
                                                        cal_pos_y, CONFIG_FILE_PATH);
                    gui.show_error_message_on_gui("CALIBRATION SAVED FOR PARK POSITION WHITE 1");
                    LOG_F(INFO,
                          "CALIBRATION SAVED FOR PARK POS WHITE 1 MECHANIC_PARK_POS_WHITE_X_LINE %i MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET %i",
                          cal_pos_x, cal_pos_y);


                } else if (cal_move == 4) {
                    //INTS ONLY NEEDED FOR ONE SITE BECAUSE THE WHITE PARK POS ARE THE SAME DIMENSIONS
                    //GET OFFSET FROM FIRST CELL
                    const int first_park_cell_offset = ConfigParser::getInstance()->getInt_nocheck(
                            ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET);
                    //CALC PARK POS CELL POS
                    const int park_positions_length = (cal_pos_y - first_park_cell_offset) / (16 - 1);
                    //SAVE PARK POS CELL OFFSET
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE,
                                                        park_positions_length, CONFIG_FILE_PATH);
                    gui.show_error_message_on_gui("CALIBRATION SAVED FOR PARK POSITION CELL SIZE");
                    LOG_F(INFO,
                          "CALIBRATION SAVED FOR PARK POS WHITE 1 MECHANIC_PARK_POS_WHITE_X_LINE %i MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET %i",
                          cal_pos_x, cal_pos_y);
                }

                //WRITE CONFIG TO FILE
                ConfigParser::getInstance()->writeConfigFile(CONFIG_FILE_PATH);
                //RESET CAL MENU
                cal_move = -1;




                //--------------------------------------------------------
                //----------------SOLANOID CALIBRATION SCREEN ---------------------
                //--------------------------------------------------------
            } else if (ev.event == guicommunicator::GUI_ELEMENT::SOLANOIDSCREEN_UPPER_POS &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                HardwareInterface::getInstance()->set_speed_preset(
                        HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
                solcal_move = 0;

                LOG_F(WARNING, "SOLANOID CALIBRATION SCREEN - MAGNET UPPER POSITION");
                //MOVE HOME POS
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                HardwareInterface::getInstance()->home_sync();

                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, true);
                //READ CONFIG VALUES
                solcal_pos = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS);

            } else if (ev.event == guicommunicator::GUI_ELEMENT::SOLANOIDSCREEN_BOTTOM_POS &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                HardwareInterface::getInstance()->set_speed_preset(
                        HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE);
                solcal_move = 1;

                LOG_F(WARNING, "SOLANOID CALIBRATION SCREEN - MAGNET BOTTOM POSITION");
                //MOVE HOME POS
                //READ CONFIG VALUES
                solcal_pos = ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS);

                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                HardwareInterface::getInstance()->home_sync();
            } else if (ev.event == guicommunicator::GUI_ELEMENT::SOLANOIDSCREEN_MVUP &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //INCREASE POSITION
                solcal_pos += 5;
                if (solcal_pos >= 255) {
                    solcal_pos = 255;
                }
                LOG_F(WARNING, "SOLANOID CALIBRATION SCREEN - NEW POS %i FOR SOLANOID_MODE %i", solcal_pos,
                      solcal_move);
                //SAVE NEW VALUE AND WRITE NEW VALUE TO SERVO
                /// WORKS ONLY ON PRODUCATION HARDWARE EQUIPPED WITH THE SERVO
                if (solcal_move == 0 && HardwareInterface::getInstance()->is_production_hardware()) {
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS,
                                                        solcal_pos, CONFIG_FILE_PATH);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, true);
                } else if (solcal_move == 1 && HardwareInterface::getInstance()->is_production_hardware()) {
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS,
                                                        solcal_pos, CONFIG_FILE_PATH);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                }

            } else if (ev.event == guicommunicator::GUI_ELEMENT::SOLANOIDSCREEN_MVDONW &&
                       ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
                //INCREASE POSITION
                solcal_pos -= 5;
                if (solcal_pos < 0) {
                    solcal_pos = 0;
                }
                LOG_F(WARNING, "SOLANOID CALIBRATION SCREEN - NEW POS %i FOR SOLANOID_MODE %i", solcal_pos,
                      solcal_move);
                //SAVE NEW VALUE AND WRITE NEW VALUE TO SERVO
                /// WORKS ONLY ON PRODUCATION HARDWARE EQUIPPED WITH THE SERVO
                if (solcal_move == 0 && HardwareInterface::getInstance()->is_production_hardware()) {
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS,
                                                        solcal_pos, CONFIG_FILE_PATH);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, true);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, true);
                } else if (solcal_move == 1 && HardwareInterface::getInstance()->is_production_hardware()) {
                    ConfigParser::getInstance()->setInt(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS,
                                                        solcal_pos, CONFIG_FILE_PATH);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
                    HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
                }
            }
        }
    } //END MAIN WHILE
//UPLOAD LOGILES
    loguru::flush();
    gamebackend.upload_logfile(read_file_to_string(LOG_FILE_PATH_ERROR));
    gamebackend.logout();

    return 0;
}





