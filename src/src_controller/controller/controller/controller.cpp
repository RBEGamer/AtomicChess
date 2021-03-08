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



#include "GCodeSender.h"


//---------------------- CONFIG DEFINED --------------------------- //
#define CONFIG_FILE_PATH "./atccontrollerconfig.ini"
#define LOG_FILE_PATH "/tmp/atc_controller_log.log"
#define LOG_FILE_PATH_ERROR "/tmp/atc_controller_error_log.log"
using namespace std;

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
}


int mainloop_running = 0;
int make_move_mode = 0;   //TO DETERM FOR WHICH PURPOSE THE PLAYER_MAKE_MANUAL_MOVE_SCREEN WAS OPENED 0=NOT OPEN 1=TEST 2=RUNNING GAME MOVE
BackendConnector* gamebackend_logupload = nullptr;    //USED ONLY FOR UPLOADING THE LOGS

std::string get_interface_mac_address(const string& _ifname) {
	ifstream iface("/sys/class/net/" + _ifname + "/address");
	//READ FILE
	std::string str((istreambuf_iterator<char>(iface)), istreambuf_iterator<char>());
	//CHECK LENGTH
	if(str.length() > 0) {
		//REPLACE ILLEGAL CHARAKTERS
		std::string hex = regex_replace(str, std::regex("[,:.;/\\\n\t\r ]"), "");
		return hex;
	}
	else {
		return "1nv4l1dm4c";
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
	
	
	//SETUP LOGGER
	loguru::init(argc, argv);
	loguru::add_file(LOG_FILE_PATH, loguru::Truncate, loguru::Verbosity_MAX);
	loguru::add_file(LOG_FILE_PATH_ERROR, loguru::Truncate, loguru::Verbosity_WARNING);
	
	
	loguru::g_stderr_verbosity = 1;
	LOG_SCOPE_F(INFO, "ATC CONTROLLER STARTED");
	
	
	
	//READ CONFIG FILE
	LOG_SCOPE_F(INFO, "LOADING CONFIG FILE ./atccontrollerconfig.ini");
	
	//GENERATE A DEFAULT CONFIG FILE IN DEBUG MODE TO TEST THE CONFIG GENERATION
	if(cmdOptionExists(argv, argv + argc, "-writeconfig"))
	{
		LOG_F(WARNING, "ARGUMENT -writeconfig SET => CREATE NEW CONFIG FILE");
		//TODO
		ConfigParser::getInstance()->loadDefaults();
		ConfigParser::getInstance()->createConfigFile(CONFIG_FILE_PATH, false);
	}
	

	//OVERWRITE WITH EXISTSING CONFIG FILE SETTINGS
	if(!ConfigParser::getInstance()->loadConfigFile(CONFIG_FILE_PATH))
	{
		LOG_F(ERROR, "Failed to load atccontrollerconfig.ini");
		ConfigParser::getInstance()->loadDefaults();            //LOAD (PUPULATE) ALL CONFIG ENTRIES WITH THE DEFAULT CONFIG
		ConfigParser::getInstance()->createConfigFile(CONFIG_FILE_PATH, false);
		LOG_F(ERROR, "Failed to load atccontrollerconfig.ini");
		return 1;
	}
	LOG_F(INFO, "CONFIG FILE LOADED");	
	
	
	//------------- DETERM THE HW REV ------------ //
	//DETERM BY THE HWID => THERE IS ONLY ONE DK VERSION WITH DIFFERENT HARDWARE
	std::string hwid = get_interface_mac_address(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE));
	LOG_F(INFO, (const char*)hwid.c_str());

	//IF WE ARE ON THE DK HARDWARE THEN OVERRIDE THE DEFAULT CONFIG
	if(hwid == "b827ebad862f")
	{
		LOG_F(WARNING, "DETECTED HWID WITH DK HARDWARE PATCHING CONFIG TO DK");	
		//STORE ONLY IN DEBUG MODE PERSISTENT
		ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::HWARDWARE_REVISION, "DK", CONFIG_FILE_PATH);
		ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::MECHANIC_DISBABLE_COILSIWTCH_USE_COIL_A_ONLY, "0", CONFIG_FILE_PATH);
		
		
		
	}else
	{
		ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::HWARDWARE_REVISION, "PROD", CONFIG_FILE_PATH);
		ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::MECHANIC_DISBABLE_COILSIWTCH_USE_COIL_A_ONLY, "1", CONFIG_FILE_PATH);
		LOG_F(INFO, "DETECTED HWID WITH PROD");	
	}

	
	
	if (cmdOptionExists(argv, argv + argc, "-autoplayer"))
	{
		LOG_F(WARNING, "ARGUMENT -autoplayer SET");
		//TODO
		ConfigParser::getInstance()->set(ConfigParser::CFG_ENTRY::GENERAL_ENABLE_RANDOM_MOVE_MATCH, "1", CONFIG_FILE_PATH);
	}
	
	
	
	
	//INIT HARDWARE
	if(!HardwareInterface::getInstance()->check_hw_init_complete())
	{
		LOG_F(ERROR, "check_hw_init_complete failed");
		std::raise(SIGINT);
	}
	
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL_A, true);
	HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL_A, false);	
		
	//SARTING GUI COMMUNICATOR PROCESS
	LOG_F(INFO, "guicommunicator startig ipc thread");
	guicommunicator gui;
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
	//
	if(gui_wait_counter > GUI_WAIT_COUNTER_MAX)
	{
		LOG_F(WARNING, "guicommunicator check_guicommunicator_reachable check failed");
		std::raise(SIGINT);
	}
	
	gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_RESET, guicommunicator::GUI_VALUE_TYPE::ENABLED);
	gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
	//CHECK VERSION ON GUI SIDE
	if(gui.check_guicommunicator_version())
	{
		LOG_F(WARNING, "guicommunicator version check failed");
	}
	//DETERM THE HWID BY USING THE MAC ADDRESS OF THE OUTGOING INTERNFACE NAME	
	gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_HWID_LABEL, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, hwid);

	
	
	//DISPLAY FIRMARE VERSION NUMBER
	std::string fwver = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH);
	std::string hwrev = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_HWREV_FILE_PATH);
	std::string bootpart = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_BOOT_PARTION_INFO_FILE_PATH);
	gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_RANK_LABEL, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, fwver + "|" + hwrev + "|" + bootpart);
	LOG_F(INFO, (const char*)fwver.c_str());
	
	
	
	
	
	
	
	
	
	
	
	//INIT CHESSBOARD
	ChessBoard board;
	//INIT THE CHESS BOARD MECHANIC
	//=> HOME, SETUP COILS
	HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_WHITE_TURN);
	bool board_scan = true;
	//ASKS THE USER TO PLACE THE FIGURE CORRECTLY
	if(cmdOptionExists(argv, argv + argc, "-skipplacementdialog") || ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG))
	{
		board_scan = false;
		LOG_F(WARNING, "USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG IS SET skip board scan");
	}
	else
	{
		if (gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_A_OK_CANCEL, "CHESS FIGURES PLACED IN PARKING POSITIONS?", 10000) != guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_OK)
		{
			board_scan = false;
		}
		else if (gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_A_OK_CANCEL, "CHESS FIGURES PLACED IN PARKING POSITIONS?", 10000) != guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_CANCEL)
		{
			board_scan = true;
		}
	}
		
		
	
	
	
	
	HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING);
	gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
	while (board.initBoard(board_scan) != ChessBoard::BOARD_ERROR::INIT_COMPLETE)
	{
		if (gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_A_OK_CANCEL, "BOARD_INIT_FAILED RETRY?", 10000) != guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_OK) {
			break;
		}
	}
	

	
	//CREATE GAME BACKEND INSTANCE
	BackendConnector gamebackend(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL), ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE), hwid);
	//SET NEEDED SETTINGS
	int gamebackend_heartbeat_interval = 5;
	ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::NETWORK_HEARTBEAT_INTERVAL_SECS, gamebackend_heartbeat_interval);
	gamebackend.setHearbeatCallInterval(gamebackend_heartbeat_interval);
	
	//NOW TRY TO CONNECT TO THE SERVER
	std::string ALTERNATIVE_BACKEND_URL[] = { "http://192.168.178.24:3000", "http://atomicchess.de:3000", "http://marcelochsendorf.com:3000", "http://marcelochsendorf.com:3001", "http://marcelochsendorf.com:3002", "http://prodevmo.com:3001", "http://prodevmo.com:3002", "http://127.0.0.1:3000" };	
	//CHECK IF GAMESERVER IS REACHABLE ELSE USE A OTHER PREDEFINED URL
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
	
	

	
	
	//PERFORM A LOGOUT
	if(gamebackend.logout())
	{
		LOG_F(ERROR, "gamebackend - LOGIN");	
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
	TimePoint t1 = Clock::now();
	TimePoint t2 = Clock::now();
	int SYSTEM_TICK_DELAY = 0;
	if (!ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::GENERAL_SYSTEM_TICK_INTERVAL_MS, SYSTEM_TICK_DELAY)) {
		LOG_F(ERROR, "GENERAL_SYSTEM_TICK_INTERVAL_MS - config get entry  failed");
	}
	LOG_F(INFO, "SYSTEM_TICK_DELAY SET %i", SYSTEM_TICK_DELAY);
	
	//INIT SYSTEM STATE MACHINE
	StateMachine state_machiene;
	
	//DISCARD ALL GUI EVENTS
	gui.clearPreviousEvents();
	//ENTERING MIAN LOOP
	HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE);
	while (mainloop_running == 0)
	{
		
	
		t2 = Clock::now();
		//IF TIME DIFFERENCE IS BIG ENOUGHT => CALL THE TIMER EVENT
		if(CHRONO_DURATION_MS(t2, t1) > SYSTEM_TICK_DELAY)
		{
			t1 = Clock::now();
		

		
			//IF A VALID SESSION
			if(gamebackend.check_login_state())
			{
				//THE THE PLAYER STATUS
				//WHICH CONTAINS ALL INFORMATION ABOUT THE CURRENT STATE OF GAME / MATCHMAKING / GENERAL STATE OF THE CLIENT
				BackendConnector::PLAYER_STATUS current_player_state = gamebackend.get_player_state();
				
				
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
				
				//TODO STATE MACHINE NEU !!!! --------------------
			
				
				//FOR ALL OTHER EVENTS USE THE STATE MACHINE HANDLING CLASS
				//WHICH HANDLES THE GAME LOGIC STATES
				//THE CALL DETERMS THE CURRENT STATE
				//StateMachine::SM_STATE current_state = state_machiene.determ_state(current_player_state);
				//StateMachine::SM_STATE previous_state = state_machiene.get_prev_state();
				//IF STATE SWITCHED
				if(current_player_state.game_state.game_running)
				{
					
					//IN THIS SECIONT THE IMMEDIATES STATE WILL BE HANDLED
					//EG FROM NO_GAME_RUNNING TO GAME_RUNNING, THE SCREEN HAVE TO BE SWITCHED, ...
					
					//IF A GAME IS RUNNING UPDATE THE UI
					update_gamescreen_with_player_state(current_player_state, gui);
					
					//IF BOARD IS IN SYNC PHASE => SETUP BOARD BIECES
					if (current_player_state.game_state.is_syncing_phase)
					{
						
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
						
						
					}
					
					if (!current_player_state.game_state.is_my_turn) {
						make_move_mode = 0;
						
						if (current_player_state.game_state.im_white_player)
						{
							HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_BLACK_TURN);
						}
						else
						{
							HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_WHITE_TURN);
						}
						
						//SYNC BOARDS
						if(board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD) && !current_player_state.game_state.current_board_move.empty()) {
								if(board.syncRealWithTargetBoard(board.StringToMovePair(current_player_state.game_state.current_board_move))){
									
								}
						}
						
						
					}
					//IS MY TURN TRIGGER DIALOG
					if (current_player_state.game_state.is_my_turn) {
						if (current_player_state.game_state.im_white_player)
						{
							HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_WHITE_TURN);
						}
						else
						{
							HardwareInterface::getInstance()->setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_BLACK_TURN);
						}
						//SYNC BOARDS
						if (board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD) && board.syncRealWithTargetBoard()) {
								
						}
						
						//ENABLE AUTO PLAY FEATURE
						if(cmdOptionExists(argv, argv + argc, "-autoplayer") || ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_RANDOM_MOVE_MATCH)) {	
							if (current_player_state.game_state.legal_moves.size() > 0)
							{
								const int rnd_idnex = (rand() % (current_player_state.game_state.legal_moves.size() - 1));
								if(!gamebackend.set_make_move(current_player_state.game_state.legal_moves.at(rnd_idnex))){
									
								}
						
							}
							else
							{
								gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LEGAL_MOVES_EMPTY - CANCEL GAME", 4000);
								LOG_F(ERROR, "LEGAL_MOVES_EMPTY - CANCEL GAME");
							}
						
							//READ HUMAN PLAYER INPUT
						}else{
							//TODO SHOW MOVE UI
							if (make_move_mode != 2)
							{
								gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN);
								make_move_mode = 2;
							}
								
							
						}
					}
					
					//IS GAME OVER => ABORT GAME TO SERVER AND  GOTO MAIN MENU
					if(current_player_state.game_state.is_game_over) {
						gamebackend.set_abort_game();
						gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MAIN_MENU_SCREEN);	
					}
					
					//SYNC THE TWO BORDS
					board.boardFromFen(current_player_state.game_state.current_board_fen, ChessBoard::BOARD_TPYE::TARGET_BOARD);
					board.syncRealWithTargetBoard();

				
					
				
						
			

			
			}else
			{
			
			}
				
		}
			
	}
		
		
	//HANDLE UI EVENTS UI LOOP
	guicommunicator::GUI_EVENT ev = gui.get_gui_update_event();
	if (!ev.is_event_valid){continue;}
				
	gui.debug_event(ev, true);	

		
	//--------------------------------------------------------
	//----------------LOGIN BUTTON ---------------------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::BEGIN_BTN && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		//PERFORM A LOGIN AS HUMAN
		if(gamebackend.login(BackendConnector::PLAYER_TYPE::PT_HUMAN) && !gamebackend.get_session_id().empty())
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
				//PLACE THE GOT SESSION ID ON THE INFO SCREEN
				gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_SESSIONID_LABEL, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, gamebackend.get_session_id());
				//SHOW PLAYERNAME ON INFO SCREEN
				gui.createEvent(guicommunicator::GUI_ELEMENT::INFOSCREEN_VERSION, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, "Playername: " + gamebackend.getPlayerProfile().friendly_name + " | " + gamebackend.getPlayerProfile().elo_rank_readable);
			}else {
				gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOGIN_FAILED_HEARTBEAT", 4000);
				LOG_F(ERROR, "GOT LOGIN_FAILED_HEARTBEAT START THREAD");
				gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
			}
				
				
				
			//SET USER TO AUTO SEARCHING
			if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE))
			{
				gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_SEARCHING);	  	
			}

				


			
		}else {
			gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOGIN_FAILED", 4000);
			LOG_F(ERROR, "GOT LOGIN FAILED");
			gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
		}
	}
		
		
		
		
		
		
		
	//--------------------------------------------------------
	//----------------BOARD INIT BUTTON-----------------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::INIT_BTN && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
		if (board.initBoard(board_scan) != ChessBoard::BOARD_ERROR::INIT_COMPLETE)
		{
			gui.show_error_message_on_gui("board.initBoard() FAILED");
				
		}
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::SETTINGS_SCREEN);
	}
				
		
		
	//--------------------------------------------------------
	//----------------CALIBRATE BOARD BTN--------------------------
	//--------------------------------------------------------
	if((ev.event == guicommunicator::GUI_ELEMENT::SCAN_BOARD_BTN) && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
		if (board.calibrate_home_pos() == ChessBoard::BOARD_ERROR::NO_ERROR)
		{
			gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "TABLE REACHED HOME POSITION", 10000);
		}
		else
		{
			gui.show_error_message_on_gui("board.initBoard() FAILED");
		}
		HardwareInterface::getInstance()->setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::SETTINGS_SCREEN);
	}
					
		
		
	//--------------------------------------------------------
	//----------------DEBUG - LOAD CONFIG BUTTON--------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_B && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
		ConfigParser::getInstance()->createConfigFile(CONFIG_FILE_PATH, true);
		gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "LOADED DEFAULT CONFIG", 10000);
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::SETTINGS_SCREEN);
	}
		
		
	//--------------------------------------------------------
	//----------------DEBUG - LOAD CONFIG BUTTON--------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_C && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "RUN MAKE_MOVE FKT\nh1h5\ng2a2\npb1-pw10\ng8-pw10", 10000);
		std::string test_text = "";
		int test_id = 0;
		while (test_id >= 0) {
			board.test_make_move_func(test_text, test_id);
			gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, test_text, 10000);
		}
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::DEBUG_SCREEN);
	}
		
	//--------------------------------------------------------
	//----------------DEBUG - LOAD CONFIG BUTTON--------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_D && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		gui.show_message_box(guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK, "G5 -> A2", 10000);
		std::string test_text = "";
		board.test_make_move_static();
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::DEBUG_SCREEN);
	}
			
	//--------------------------------------------------------
	//----------------DEBUG - UPLOAD CONFIG BUTTON--------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_E && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		gamebackend.upload_config(ConfigParser::getInstance()); 
	}
        
	//--------------------------------------------------------
   //----------------DEBUG - UPLOAD LOG BUTTON--------------
   //--------------------------------------------------------
   if(ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_F && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		LOG_F(WARNING, "MANUAL LOG UPLOAD");
		LOG_F(WARNING, LOG_FILE_PATH_ERROR);
		loguru::flush();
		//IF GOT A SIGNAL READ LOGFILE AND UPLOAD THEM
		std::string log = read_file_to_string(LOG_FILE_PATH_ERROR);
		if (!log.empty())
		{
			gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
			gamebackend.upload_logfile(log);
			gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::DEBUG_SCREEN);
		} 
	}
        
		
		
	//--------------------------------------------------------
	//----------------DEBUG - DOWNLOAD CONFIG BUTTON--------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_H && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		LOG_F(WARNING, "DEBUG -SHOW MAKE MOVE SCREEN ");
		make_move_mode = 1;
	        
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN); 
	}



	//----------------- PLAYER MOVE RESPOMSE ---------------------- //
	if(ev.event == guicommunicator::GUI_ELEMENT::PLAYER_EMM_INPUT && ev.type == guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING && make_move_mode) {
			
		ChessBoard::MovePiar mvpair = board.StringToMovePair(ev.value);
		if (mvpair.is_valid) {
			//FOR TEST
			if(make_move_mode == 1) {
				board.makeMoveSync(mvpair, false, false, false);
				board.syncRealWithTargetBoard();
				gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::DEBUG_SCREEN);
				make_move_mode = 0;
			}
			else if(make_move_mode == 2) {
				//FOR RUNNING GAME
				gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::GAME_SCREEN);
			   if(gamebackend.set_make_move(ev.value))
				{
						
					make_move_mode = 0;
				}else
				{
					make_move_mode = 2;
					gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PLAYER_ENTER_MANUAL_MOVE_SCREEN); 
				}
			}

				 
		}
			
	}
		
	//--------------------------------------------------------
   //----------------DEBUG - DOWNLOAD CONFIG BUTTON--------------
   //--------------------------------------------------------
   if(ev.event == guicommunicator::GUI_ELEMENT::DEBUG_FUNCTION_G && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		LOG_F(WARNING, "DEBUG - DOWNLOAD CONFIG BUTTON TRIGGERED ");
		gamebackend.download_config(ConfigParser::getInstance(), true); 
	}
		
	//--------------------------------------------------------
	//----------------LOGOUT BUTTON --------------------------
	//--------------------------------------------------------	
	if(ev.event == guicommunicator::GUI_ELEMENT::LOGOUT_BTN && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
		//LOGOUT AND GOTO LOGIN SCREEN
		//if(gamebackend.stop_heartbeat_thread()) {
			gamebackend.logout();
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
		//}
	}
		
		
	//--------------------------------------------------------
	//----------------ENABLE MATCHMAKING BUTTON --------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::MAINMENU_START_AI_MATCH_BTN && ev.type == guicommunicator::GUI_VALUE_TYPE::ENABLED) {
		//SET PLAYERSTATE TO OPEN FO A MATCH
		if(!gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_SEARCHING)) {
			gui.show_error_message_on_gui("ENABLE MATCHMAKING FAILED");
			LOG_F(WARNING, "ENABLE MATCHMAKING FAILED TRIGGERED BY USER BUTTON");
		}
	}
	//--------------------------------------------------------
	//----------------DISBALE MATCHMAKING BUTTON --------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::MAINMENU_START_AI_MATCH_BTN && ev.type == guicommunicator::GUI_VALUE_TYPE::DISBALED) {
		//SET PLAYERSTATE TO OPEN FO A MATCH
		if(!gamebackend.set_player_state(BackendConnector::PLAYER_STATE::PS_IDLE)) {
			gui.show_error_message_on_gui("DISBALE MATCHMAKING FAILED");
			LOG_F(WARNING, "DISBALE MATCHMAKING FAILED TRIGGERED BY USER BUTTON");
		}
	}	
			
	//--------------------------------------------------------
	//----------------ABORT GAME BUTTON --------------
	//--------------------------------------------------------
	if(ev.event == guicommunicator::GUI_ELEMENT::GAMESCREEN_ABORT_GAME && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {
		//SET PLAYERSTATE TO OPEN FO A MATCH
		//if(gamebackend.set_abort_game()) {
			
		//}
		//gui.show_error_message_on_gui("GAME STOPPED");
		LOG_F(WARNING, "GAME STOPPED TRIGGERED BY USER BUTTON");
		gamebackend.logout();
		gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_RESET, guicommunicator::GUI_VALUE_TYPE::ENABLED);
		gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::LOGIN_SCREEN);
	}	
		
		
		
}
	
	
	
	
//UPLOAD LOGILES
loguru::flush();
gamebackend.upload_logfile(read_file_to_string(LOG_FILE_PATH_ERROR));
gamebackend.logout();
	
return 0;
}





