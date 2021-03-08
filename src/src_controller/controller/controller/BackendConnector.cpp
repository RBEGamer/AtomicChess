#include "BackendConnector.h"


BackendConnector::~BackendConnector()
{

}



//https://stackoverflow.com/questions/34221156/c-sanitize-string-function
void BackendConnector::sanitize(std::string &stringValue)
{
	// Add backslashes.
	for(auto i = stringValue.begin() ;  ;) {
		auto const pos = std::find_if(
		    i,
			stringValue.end(),
			[](char const c) { return '\\' == c || '\'' == c || '"' == c; });
		if (pos == stringValue.end()) {
			break;
		}
		i = std::next(stringValue.insert(pos, '\\'), 2);
	}

	// Removes others.
	stringValue.erase(
	    std::remove_if(
	        stringValue.begin(),
			stringValue.end(),
			[](char const c) {
				return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c;
			}),
		stringValue.end());
}


bool BackendConnector::upload_logfile(std::string _log_string)
{
	//SANTIZE LOG
	sanitize(_log_string);
	json11::Json::object root;
	root["log"] = _log_string;
	
	std::string json_str = json11::Json(root).dump();
	size_t count = json_str.size();
	request_result tmp = make_request_post(URL_UPLOAD_LOG + "?hwid=" + hwid + "&sid=" + session_id, json_str); //POST AS TEXT/PLAIN TO AVOID NODEJS PARING LIMITS WITH A POST REQUEST
	if (tmp.request_failed) {
		return false;
	}

	return true;
}
	
bool BackendConnector::upload_config(ConfigParser* __parser_instance){
    //CHECK CONFIG PARSED
    if(__parser_instance == nullptr){
        return false;
    }
    if(!__parser_instance->configLoaded()){
        return false;
    }
    std::string json_config = __parser_instance->toJson();
    request_result tmp = make_request_post(URL_UPLOAD_CONFIG+ "?hwid=" + hwid + "&sid=" + session_id,json_config);
    if(tmp.request_failed){
        return false;
    }

    return true;
}

bool BackendConnector::download_config(ConfigParser* __parser_instance, bool _userdata_only){
    if(__parser_instance == nullptr){
        return false;
    }
    //URL_DOWNLOAD_CONFIG
    request_result tmp = make_request(URL_DOWNLOAD_CONFIG+ "?hwid=" + hwid + "&sid=" + session_id);
    if(tmp.request_failed){
        return false;
    }
    if (!tmp.body.empty()) {

        std::string parse_err = "";
        json11::Json json_root = json11::Json::parse(tmp.body.c_str(), parse_err);
        json11::Json::object root_obj = json_root.object_items();
        //CHECK PARSE RESULT
        if(!parse_err.empty()){
            return false;
        }
	    bool f = json_root.is_null();
	    bool t = root_obj.count("cfg");
	    bool h = root_obj["cfg"].is_object();
       //CHECK FOR cfg OBJ
        if(root_obj.find("cfg") != root_obj.end() && !root_obj["cfg"].is_null() && root_obj["cfg"].is_object()) {
            return __parser_instance->loadFromJson(root_obj["cfg"].object_items(), _userdata_only);
        }else
	    {
		    return false;
	    }
    }


    return false;

}


BackendConnector::PLAYER_STATUS BackendConnector::get_player_state()
{
	PLAYER_STATUS status;
	status.requst_success = true;
	
	//DO THE REQUEST
	request_result tmp = make_request(URL_PLAYER_STATE + "?hwid=" + hwid + "&sid=" + session_id + "&simplified=1");
	
	if (tmp.request_failed)
	{
		last_error = "get_player_state - request failed";
		status.requst_success = false;
		return status;
	}
	std::string e = tmp.body;
	LOG_F(INFO, "%s", e.c_str());
	//PARSE THE JSON RESULT
	if(tmp.body.empty())
	{
		last_error = "get_player_state - result empty";
		status.requst_success = false;
		return status;
	}
	std::string jp_err = "";
	json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
	//CHECK JSON PARSE RESULT	
	if(!jp_err.empty())
	{
		last_error = "get_player_state - json parse failed";
		status.requst_success = false;
		status.err = "json_parse_failed";
		return status;
	}
	//PARSE ROOT OBJECT
	json11::Json::object  root = t.object_items();
	//err
	if(root.find("err") != root.end() && root["err"].is_string()) {
		status.err = root["err"].bool_value();
	}
	//status
	if(root.find("status") != root.end() && root["status"].is_string()) {
		status.err = root["status"].bool_value();
	}
	
	//MATCHMAKING ->waiting_for_game
	if(root["matchmaking_state"].is_object())
	{
		json11::Json::object  mm = root["matchmaking_state"].object_items();
		//WAITING_FOR_GAME_STATE
		if(mm.find("waiting_for_game") != mm.end() && mm["waiting_for_game"].is_bool()) {
			status.matchmaking_state.waiting_for_game = mm["waiting_for_game"].bool_value();
		}	
		//GET PLAYER VISIBLE STATE AND DO THE CASTING TO THE ENUM
		//WHICH IS INLY AVARIABLE IN THE DETAILED OBJECT
		//TODO REWORK THIS
		if(mm.find("detailed") != mm.end() && mm["detailed"].is_object()) {
			json11::Json::object  mm_detailed = root["detailed"].object_items();
			if (mm_detailed.find("player_state") != mm_detailed.end() && mm_detailed["player_state"].int_value()) {
				auto val = magic_enum::enum_cast<PLAYER_STATE>(mm_detailed["player_state"].int_value());
				if (val.has_value())
				{
					status.player_state = val.value();
				}
				else
				{
					status.player_state = BackendConnector::PLAYER_STATE::PS_INVALID;
				}
			}
		}
		
	}
	//GAME_STATE->game_running
	if(root["game_state"].is_object())
	{
		json11::Json::object  gs = root["game_state"].object_items();
		//GET GAME RUNNING FLAG
		if(gs.find("game_running") != gs.end() && gs["game_running"].is_bool()) {
			status.game_state.game_running = gs["game_running"].bool_value();
		}
		
		if (gs.find("simplified") != gs.end() && gs["simplified"].is_object()) {
			json11::Json::object  gs_simplified = gs["simplified"].object_items();
			
			if (gs_simplified.find("im_white_player") != gs_simplified.end() && gs_simplified["im_white_player"].is_bool()) {
				status.game_state.im_white_player = gs_simplified["im_white_player"].bool_value();
			}
			if (gs_simplified.find("is_my_turn") != gs_simplified.end() && gs_simplified["is_my_turn"].is_bool()) {
				status.game_state.is_my_turn = gs_simplified["is_my_turn"].bool_value();
			}
			if (gs_simplified.find("is_syncing_phase") != gs_simplified.end() && gs_simplified["is_syncing_phase"].is_bool()) {
				status.game_state.is_syncing_phase = gs_simplified["is_syncing_phase"].bool_value();
			}
			//PARSE CURRENT BOARD OBJECT
			if (gs_simplified.find("current_board") != gs_simplified.end() && gs_simplified["current_board"].is_object()) {
				json11::Json::object  gs_board = gs_simplified["current_board"].object_items();
				
				if (gs_board.find("fen") != gs_board.end() && gs_board["fen"].is_string())
				{
					status.game_state.current_board_fen = gs_board["fen"].string_value();
				}
				else
				{
					status.game_state.current_board_fen = "";
				}
				
				
				if (gs_board.find("initial_board") != gs_board.end() && gs_board["initial_board"].is_bool()) {
					status.game_state.is_initial_board = gs_board["initial_board"].bool_value();
				}
				if (gs_board.find("is_board_valid") != gs_board.end() && gs_board["is_board_valid"].is_bool()) {
					status.game_state.is_board_valid = gs_board["is_board_valid"].bool_value();
				}
				if (gs_board.find("is_game_over") != gs_board.end() && gs_board["is_game_over"].is_bool()) {
					status.game_state.is_game_over = gs_board["is_game_over"].bool_value();
				}
				if (gs_board.find("is_game_over") != gs_board.end() && gs_board["is_game_over"].is_bool()) {
					status.game_state.is_game_over = gs_board["is_game_over"].bool_value();
				}
				
				if (gs_board.find("move") != gs_board.end() && gs_board["move"].is_string())
				{
					status.game_state.current_board_move = gs_board["move"].string_value();
				}
				else
				{
					status.game_state.current_board_move = "";
				}
				
				if (gs_board.find("legal_moves") != gs_board.end() && gs_board["legal_moves"].is_array()) {
					for (int i = 0; i < gs_board["legal_moves"].array_items().size(); i++)
					{
						json11::Json ttobj = gs_board["legal_moves"].array_items()[i];
						
						if (ttobj.is_string()) {
							std::string tmp = ttobj.string_value();
							status.game_state.legal_moves.push_back(tmp);
						}		
						
					}
						
					  
				}
			}
			
		}	
	}
	return status;
}


void BackendConnector::set_https_client_certificate(std::string _path)
{
	ca_client_path = _path;
}

int BackendConnector::get_avariable_ai_player()
{
	request_result tmp = make_request(URL_GET_AI_PLAYERS_AVARIABLE + "?hwid=" + hwid + "&sid=" + session_id);
	if (tmp.request_failed)
	{
		last_error = "get_avariable_ai_player - request failed";
		return -1;
	}
	
	if (tmp.body.empty())
	{
		last_error = "get_avariable_ai_player - result empty";
		return -1;
	}
	
	//PARSE JSON RESULT
	std::string jp_err = "";
	json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
	if (jp_err.empty())
	{
		
		json11::Json::object  tt = t.object_items();
		if (tt.find("count") != tt.end()) {
			
			int count_str = tt["count"].int_value();
			if (count_str >= 0)
			{
				return count_str;
			}
		}
	}
	last_error = "get_avariable_ai_player - result format count invalid";
	return -1;
	
	
}

bool BackendConnector::set_player_setup_confirmation(PLAYER_SETUP_STATE _state)
{
	int state_index = magic_enum::enum_integer(_state);
	request_result tmp = make_request(URL_SET_PLAYER_SETUP_CONFIRMATION + "?hwid=" + hwid + "&sid=" + session_id + "&state=" + std::to_string(state_index));
	if (!tmp.body.empty())
	{
		//PARSE JSON
		std::string jp_err = "";
		json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
		if (jp_err.empty())
		{
			if (((json11::Json::object)t.object_items())["status"].string_value() == "ok")
			{
				return true;
			}	
		}
	}
	return false;
}
	
	

bool BackendConnector::set_make_move(std::string _move)
{
	request_result tmp = make_request(URL_SET_MAKE_MOVE + "?hwid=" + hwid + "&sid=" + session_id + "&move=" + _move);
	if (!tmp.body.empty())
	{
		//PARSE JSON
		std::string jp_err = "";
		json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
		if (jp_err.empty())
		{
			if (((json11::Json::object)t.object_items())["status"].string_value() == "ok")
			{
				return true;
			}	
		}
	}
	return false;
}


bool BackendConnector::set_abort_game()
{
	request_result tmp = make_request(URL_ABORT_GAME + "?hwid=" + hwid + "&sid=" + session_id);
	if (!tmp.body.empty())
	{
		//PARSE JSON
		std::string jp_err = "";
		json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
		if (jp_err.empty())
		{
			if (((json11::Json::object)t.object_items())["status"].string_value() == "ok")
			{
				return true;
			}	
		}
	}
	return false;
}


bool BackendConnector::set_player_state(PLAYER_STATE _ps)
{
	int ps_index = magic_enum::enum_integer(_ps);
	
	
	request_result tmp = make_request(URL_SET_PLAYER_VISIBLE_STATE + "?hwid=" + hwid + "&sid=" + session_id + "&ps=" + std::to_string(ps_index));
	if (!tmp.body.empty())
	{
		//PARSE JSON
		std::string jp_err = "";
		json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
		if (jp_err.empty())
		{
			if (((json11::Json::object)t.object_items())["status"].string_value() == "ok")
			{
				return true;
			}	
		}
	}
	return false;
}

BackendConnector::request_result BackendConnector::make_request(std::string _url_path)
{
	//PORT AND PROTOCOL IS ALREADY INCLUDED IN BASE URL LIKE http://127.0.0.1:3000
	httplib::Client cli(backend_base_url.c_str());
	cli.set_connection_timeout(10);
	cli.set_follow_location(true);   //AUTO REDIRECT IF GOT AN 300
	//SEND REQUEST OVER SELECTED INTERFACE
	if(!interface_name.empty())
	{
		cli.set_interface(interface_name.c_str());
	}
	//SET CLIENT CERTIFICATE FOR HTTP SERVER
	if(!ca_client_path.empty())
	{
		//cli.set_(ca_client_path.c_str());
	}
	
	request_result req_res;
	req_res.uri =  backend_base_url + _url_path;
	req_res.request_failed = false;
	//PERFORM REQUEST
	if(auto res = cli.Get(_url_path.c_str())) {
		req_res.status_code = res->status;
		//CHECK STATUS CODE 200 IS VALID
			req_res.body = res->body;
	}
	else {
		req_res.error = res.error();
		req_res.request_failed = true;
	}
	
	return req_res;
}


BackendConnector::request_result BackendConnector::make_request_post(std::string _url_path, std::string _post_body_json_data)
{
    //PORT AND PROTOCOL IS ALREADY INCLUDED IN BASE URL LIKE http://127.0.0.1:3000
    httplib::Client cli(backend_base_url.c_str());
    cli.set_connection_timeout(10);
    cli.set_follow_location(true);   //AUTO REDIRECT IF GOT AN 300
    //SEND REQUEST OVER SELECTED INTERFACE
    if(!interface_name.empty())
    {
        cli.set_interface(interface_name.c_str());
    }
    //SET CLIENT CERTIFICATE FOR HTTP SERVER
    if(!ca_client_path.empty())
    {
        //cli.set_(ca_client_path.c_str());
    }

    request_result req_res;
    req_res.uri =  backend_base_url + _url_path;
    req_res.request_failed = false;
    //PERFORM REQUEST
    if(auto res = cli.Post(_url_path.c_str(),_post_body_json_data,"application/json")) {
        req_res.status_code = res->status;
        //CHECK STATUS CODE 200 IS VALID
        req_res.body = res->body;
    }
    else {
        req_res.error = res.error();
        req_res.request_failed = true;
    }

    return req_res;
}

BackendConnector::request_result BackendConnector::make_request_post_raw(std::string _url_path, std::string _post_body_json_data)
{
	//PORT AND PROTOCOL IS ALREADY INCLUDED IN BASE URL LIKE http://127.0.0.1:3000
	httplib::Client cli(backend_base_url.c_str());
	cli.set_connection_timeout(10);
	cli.set_follow_location(true);    //AUTO REDIRECT IF GOT AN 300
	//SEND REQUEST OVER SELECTED INTERFACE
	if(!interface_name.empty())
	{
		cli.set_interface(interface_name.c_str());
	}
	//SET CLIENT CERTIFICATE FOR HTTP SERVER
	if(!ca_client_path.empty())
	{
		//cli.set_(ca_client_path.c_str());
	}

	request_result req_res;
	req_res.uri =  backend_base_url + _url_path;
	req_res.request_failed = false;
	//PERFORM REQUEST
	if(auto res = cli.Post(_url_path.c_str(), _post_body_json_data, "text/plain")) {
		req_res.status_code = res->status;
		//CHECK STATUS CODE 200 IS VALID
		req_res.body = res->body;
	}
	else {
		req_res.error = res.error();
		req_res.request_failed = true;
	}

	return req_res;
}
void BackendConnector::set_backend_base_url(std::string _url)
{
	backend_base_url = _url;
}

std::string BackendConnector::get_backend_base_url()
{
	return backend_base_url;
}


BackendConnector::PLAYER_PROFILE BackendConnector::getPlayerProfile()
{
	return player_profile;
}
//CHECKS IF BACKEND REACHABLE
bool BackendConnector::check_connection()
{
	request_result tmp = make_request(URL_CONNECTION_CHECK);
	if (!tmp.body.empty())
	{
		//PARSE JSON
		std::string jp_err = "";
		json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
		if (jp_err.empty())
		{
			if (((json11::Json::object)t.object_items())["status"].string_value() == "ok")
			{
				return true;
			}	
		}
	}
	return false;
}

bool BackendConnector::login(PLAYER_TYPE _pt)
{
	//SET PLAYER TYPE PARAMETER
	std::string pt = "0";
	if (_pt == BackendConnector::PLAYER_TYPE::PT_AI) {
		pt = "1";	
	}
	else if (_pt == BackendConnector::PLAYER_TYPE::PT_HUMAN) {
		pt = "0";	
	}
	//DO THE REQUEST
	request_result tmp = make_request(URL_LOGIN + "?hwid=" + hwid + "&playertype=" + pt);
	
	if (tmp.request_failed)
	{
		last_error = "login - request failed";
		return false;
	}
	//PARSE THE JSON RESULT
	if(!tmp.body.empty())
	{
		//PARSE JSON
		std::string jp_err = "";
		json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
		
		if (jp_err.empty())
		{
			json11::Json::object  tt = t.object_items();
			std::string status = tt["status"].string_value();
			if (status != "ok")
			{
				last_error = tt["status"].string_value();
				return false;
			}
			
			//TRY TO GET PROFILE INFORMATION
			if(tt["profile"].is_object())
			{
				json11::Json::object  po = tt["profile"].object_items();
				//FRIENDLY NAME
				if(po.find("friendly_name") != po.end()) {
					player_profile.friendly_name = po["friendly_name"].string_value();
				}
			
				
				//VIRTUAL PLAYER ID
				if(po.find("virtual_player_id") != po.end()) {
					player_profile.virtual_player_id = po["virtual_player_id"].string_value();
				}
				//ELO_RANK_READABLE
				if(po.find("elo_rank_readable") != po.end()) {
					player_profile.elo_rank_readable = po["elo_rank_readable"].string_value();
				
					//RANK
					if(po.find("rank") != po.end()) {
						player_profile.rank = po["rank"].int_value();
					}
				}
				//FINALLY GET THE SESSION ID
					std::string sid = tt["sid"].string_value();
				if (!sid.empty())
				{
					session_id = sid;
					return true;
				}	
			}
		}
	}
	//PARSE ID
	//SAVE SESSION ID
	//SAVE IMPORTANT PROFILE DATA
	last_error = "login - body empty";
	return false;
}
		
bool BackendConnector::logout()
{
	request_result tmp = make_request(URL_LOGOUT + "?hwid=" + hwid);
	
	if (tmp.request_failed)
	{
		last_error = "login - request failed";
		return false;
	}
	//DESTROY SESSION
	player_profile.rank = -1;
	player_profile.elo_rank_readable = "";
	player_profile.virtual_player_id  = "";
	player_profile.friendly_name = "";
	session_id = "";
	return true;
}

BackendConnector::BackendConnector(std::string _backend_base_url, std::string _interface_name, std::string _hwid)
	: backend_base_url()
{
	backend_base_url = _backend_base_url;
	hwid = _hwid;
	
}


bool BackendConnector::check_login_state()
{
	if (!get_session_id().empty())
	{
		return true;
	}
	return false;
}

std::string BackendConnector::get_session_id()
{
	return session_id;
}

std::string BackendConnector::get_interface_name()
{
	return interface_name;
}

std::string BackendConnector::get_last_error()
{
	return last_error;
}


void BackendConnector::setHearbeatCallInterval(int _int)
{

	if (_int <= 0) {
		_int = 1;      //MINUMUM INTERVAL
	}
	heartbeat_call_interval = _int;
}
	

	

bool BackendConnector::start_heartbeat_thread()
{
	if (heartbeat_thread_running)
	{
		stop_heartbeat_thread();
	}
	if (heartbeat_call_interval <= 0) {
		heartbeat_call_interval = 1;        //MINUMUM INTERVAL
	}
	
	//SET THREAD DATA
	heartbeat_thread_data.hwid = hwid;
	heartbeat_thread_data.sid = session_id;
	heartbeat_thread_data.heartbeat_interval = heartbeat_call_interval;
	//FINALLY START THREAD WITH THE THREAD FUNCTION
	heartbeat_thread_running = true;
	std::thread t1(BackendConnector::heartbeat_thread_function, this);
	t1.detach();
	heartbeat_thread = &t1;
	return true;
}

bool BackendConnector::stop_heartbeat_thread()
{
	if (!heartbeat_thread_running)
	{
		return false;
	}
	heartbeat_thread_running = false;
	//while (!heartbeat_thread->joinable()) {
	//	std::cout << "." << std::endl;
	//}
	//heartbeat_thread->join();
	return true;
}


bool BackendConnector::get_heartbeat(BackendConnector::HB_THREAD_DATA _data)
{
	if (_data.sid.empty()) {
		return false;
	}
		
	request_result tmp = make_request(URL_HEARTBEAT + "?hwid=" + _data.hwid + "&sid=" + _data.sid);
	if (!tmp.body.empty())
	{
		//PARSE JSON
		std::string jp_err = "";
		json11::Json t = json11::Json::parse(tmp.body.c_str(), jp_err);
		if (jp_err.empty())
		{
			if (((json11::Json::object)t.object_items())["status"].is_null())
			{
				return true;
			}	
		}
	}
	return false;
}
	
bool BackendConnector::get_heartbeat()
{
	HB_THREAD_DATA tmp;
	tmp.hwid = hwid;
	tmp.sid = session_id;
	
	return get_heartbeat(tmp);
}
	

void BackendConnector::heartbeat_thread_function(BackendConnector* _this)
{
	//https://stackoverflow.com/questions/158585/how-do-you-add-a-timed-delay-to-a-c-program
	using namespace std::this_thread;    // sleep_for, sleep_until
	using namespace std::chrono;    // nanoseconds, system_clock, seconds
	
	//MAKE A
	HB_THREAD_DATA thread_data = _this->heartbeat_thread_data;
	
	while (_this->heartbeat_thread_running) {
		
		bool res = _this->get_heartbeat(_this->heartbeat_thread_data);
		
	
		sleep_until(system_clock::now() + seconds(thread_data.heartbeat_interval));
		
	}
	
}
