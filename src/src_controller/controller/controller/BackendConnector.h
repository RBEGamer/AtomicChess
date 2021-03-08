#pragma once
#ifndef __BackendConnector__
#define __BackendConnector__

#include <string>
#include <list>
#include <vector>
//THREAD STUFF
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>
//INCLUDE SHARED DEPENDENCIES


//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "SHARED/cpp-httplib-master/httplib.h"


#include "SHARED/json11-master/json11.hpp"
#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/magic_enum-master/include/magic_enum.hpp"

//INCLUDE LOCAL DEPENDENCIES
#include "ConfigParser.h"

//THREAD FOR SESSOON
class BackendConnector
{
	
public:
	struct PLAYER_AVARIABLE{
		std::string visibale_name;
		int rank;
		std::string elo_rank;
		std::string virtual_player_id;
	};
	
	
	enum class GAME_STATE_STATE
	{
		GSS_UNKNOWN = 0
	};

		
	struct MATCHMAKING_STATE
	{
		bool waiting_for_game; //IF THE PLAYER HAS MATCHMAKING ENABLED
	};
	
	struct GAME_STATE
	{
		bool game_running;
		
		GAME_STATE_STATE game_state;
		bool im_white_player;
		bool is_my_turn;
		bool is_syncing_phase;
		
		
		std::string current_board_fen; //CURRENT BOARD FEN STRING
		std::string current_board_move;
		bool  is_initial_board;
		bool is_board_valid;
		bool is_game_over;
		std::vector<std::string> legal_moves;
		
	};
		
	struct PLAYER_PROFILE
	{
		std::string friendly_name;
		std::string virtual_player_id;
		std::string elo_rank_readable;
		int rank;
	};

	enum class PLAYER_STATE{
		PS_INVALID=0,
		PS_IDLE=1,
		PS_SEARCHING=4,
		PS_SEARCHING_MANUAL =5,
		PS_PREPARING_INGAME = 6,
		PS_INGAME = 7
	};
	enum class PLAYER_TYPE{
		PT_HUMAN = 0,
		PT_AI = 1
	};
	
	enum class PLAYER_SETUP_STATE {
		PSP_NO_READY = 0,
		PSP_READY = 1
	};
	
	struct PLAYER_STATUS
	{
		PLAYER_STATE player_state;
		PLAYER_TYPE player_type;
		
		std::string status;
		std::string err;
		
		
		MATCHMAKING_STATE matchmaking_state;
		GAME_STATE game_state;
		bool requst_success;
		
	};
	
	
	
	BackendConnector(std::string _backend_base_url, std::string _interface_name, std::string _hwid);
	~BackendConnector();
	
	bool login(PLAYER_TYPE _pt);
	bool update_session();
	bool check_connection();
	bool check_login_state();
	bool logout();
	
	std::list<std::string> get_avariable_player();
	

	int get_avariable_ai_player();
	bool get_heartbeat();
	
	void set_backend_base_url(std::string _url);
	std::string get_backend_base_url();
	std::string get_session_id();
	std::string get_interface_name();
	
	PLAYER_STATUS get_player_state();
	
	bool set_player_state(PLAYER_STATE _ps);
	bool set_player_setup_confirmation(PLAYER_SETUP_STATE _state);
	bool start_match_with_player(std::string _virtual_player_id);
	
	bool set_abort_game();
	bool set_make_move(std::string _move);
	std::list<PLAYER_AVARIABLE> get_players_avariable();
	//DATA STRUCT SEND TO THE HEARTBEAT THREAD FUNCTION
	
	PLAYER_PROFILE getPlayerProfile(); //GETTER FOR private plaqyer_profile
	
	
    
    
    //USED TO LOAD/REPLACE GENERAL CONFIG FROM SERVER
    bool upload_config(ConfigParser* __parser_instance);
    bool download_config(ConfigParser* __parser_instance, bool _userdata_only);
    
	//LOG UPLOAD
	bool upload_logfile(std::string _log_string);
    
    //HEARTBEAT THREAD
	bool start_heartbeat_thread();   //CHECK IF RUNNING
	bool stop_heartbeat_thread();
	void setHearbeatCallInterval(int _int);
    
    
    //MISC CALLS
	void set_https_client_certificate(std::string _path);
	std::string get_last_error();
	
	
	
private:
	
	//STRUCT FOR HOLDING IMPORTANT INFORMATION ABOUT A REQUEST RESPONSE USED AS RETURN IN THE make_request FUNCTION
	struct request_result
	{
		bool request_failed;
		std::string body;
		httplib::Error error;
		int status_code;
		std::string uri;
	};
	
	std::string backend_base_url; //BASE URL OF THE BACKEND (http:// + URL + : + PORT ) SET BY CONSTRUCTOR
	std::string hwid; //HWID OF THE USER (SET BY CONTSTRUCTOR) USED FOR ALL REQUESTS
	std::string session_id; //THE CURRENT USER SESSION ID SET AFTER LOGIN(); cleared after logout()
	std::string interface_name; //NAME OF THE OUTGOING TRAFIIC INTERFACE NAME (eth0)
	
	PLAYER_PROFILE player_profile; //HOLDS THE CURRENT USERPROFILE AFTER LOGIN (FILLED WITH login() call)
	
	
	const std::string URL_CONNECTION_CHECK = "/rest/client_status";
	const std::string URL_LOGIN = "/rest/login";
	const std::string URL_LOGOUT = "/rest/logout";
	const std::string URL_HEARTBEAT = "/rest/heartbeat";
	const std::string URL_PLAYER_STATE = "/rest/get_player_state";
	const std::string URL_GET_PLAYERS_AVARIABLE = "/rest/get_players_avariable";
	const std::string URL_GET_AI_PLAYERS_AVARIABLE = "/rest/get_avariable_ai_players";
	const std::string URL_SET_PLAYER_VISIBLE_STATE = "/rest/set_player_state";
	const std::string URL_SET_PLAYER_SETUP_CONFIRMATION = "/rest/player_setup_confirmation";
	const std::string URL_SET_MAKE_MOVE = "/rest/make_move";
	const std::string URL_UPLOAD_CONFIG = "/rest/set_user_config";
    const std::string URL_DOWNLOAD_CONFIG = "/rest/get_user_config";
	
	const std::string URL_UPLOAD_LOG = "/rest/store_user_log";
	const std::string URL_ABORT_GAME = "/rest/abort_game";
	
	std::string ca_client_path = "";
	
	std::string last_error = "";
	
	
	//--------------------- THREAD VARIABLES ---------------------- //
	//------------------ USED FOR THE HEARTBEAT ------------------ //
	struct HB_THREAD_DATA {
		std::string hwid;
		std::string sid;
		int heartbeat_interval;
	};
	int heartbeat_call_interval = 5;
	bool heartbeat_thread_running = false; //IS THE THREAD RUNNING
	HB_THREAD_DATA heartbeat_thread_data; //THREAD READ ALL VARS IN THIS STRUCT; SET BY START_HEARBEAT THREAD FUNCTION
	std::thread* heartbeat_thread = nullptr;
	std::mutex heartbeat_thread_mutex;
	bool get_heartbeat(HB_THREAD_DATA _data);  //FUNCTION VARIANT TO USE WITH A THREAD
	//-------------------- FUNCTIONS ----------------------------- //
	
	request_result make_request(std::string _url_path);
    request_result make_request_post(std::string _url_path, std::string _post_body_json_data);
	request_result make_request_post_raw(std::string _url_path, std::string _post_body_json_data);
    static void heartbeat_thread_function(BackendConnector* _this);
	
	void sanitize(std::string& stringValue);
	
	
};
#endif

