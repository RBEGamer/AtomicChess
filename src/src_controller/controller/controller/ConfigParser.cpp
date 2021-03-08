#include "ConfigParser.h"

//STATIC INIT
ConfigParser* ConfigParser::instance = nullptr;
std::mutex ConfigParser::acces_lock_mutex;


std::string ConfigParser::toJson(){

    //CREATE JSON ROOT ELEMENT
    json11::Json::object root;
    json11::Json::object settings;
    json11::Json::object settings_user;
    constexpr auto& entries = magic_enum::enum_values<ConfigParser::CFG_ENTRY>();
    //FOR EACH CONFIG ENTRY ADD ENTRY TO ROOT ELEMENT
    for (size_t i = 0; i < entries.size(); i++)
    {
        //CAST ENUM KEY TO STRING
        std::string_view cfg_name = magic_enum::enum_name(entries.at(i));
        //SKIP EMPTY NAMES
        if (std::string(cfg_name).empty()) {
            continue;
        }
        //FINALLY ADD ELEMENT
        if (std::string(cfg_name).rfind(USER_DATA_CONFIG_ENTRY_PREFIX,0)==0) {
            settings_user[std::string(cfg_name)] = config_store[entries.at(i)];
        }else{
            settings[std::string(cfg_name)] = config_store[entries.at(i)];
        }

    }


    //SUMP TO JSON
    root[INI_SETTINGS_CATEGORY_TOKEN] = settings;
    root[INI_USER_DATA_CATEGORY_TOKEN] = settings_user;

    std::string json_str = json11::Json(root).dump();

    return json_str;
}



bool ConfigParser::loadFromJson(json11::Json::object _jsobj, bool load_only_user_data){
    //NOW READ ENTRIES
    constexpr auto& entries = magic_enum::enum_values<ConfigParser::CFG_ENTRY>();

    if(!load_only_user_data){
        //CHECK
        if(!_jsobj[INI_SETTINGS_CATEGORY_TOKEN].is_null() && _jsobj.find(INI_SETTINGS_CATEGORY_TOKEN) != _jsobj.end() && _jsobj[INI_SETTINGS_CATEGORY_TOKEN].is_object()) {
            json11::Json::object settings_obj = _jsobj[INI_SETTINGS_CATEGORY_TOKEN].object_items();
            //GET SINGLE ITEMS
            for (size_t i = 0; i < entries.size(); i++)
            {
                std::string_view cfg_name = magic_enum::enum_name(entries.at(i));
                if (std::string(cfg_name).empty()) {
                    continue;
                }
                //GET OBJECT VALUE STORE IT IN CONFIG_STORE
                if(settings_obj.find(std::string(cfg_name)) != settings_obj.end() && settings_obj[std::string(cfg_name)].is_string() && !settings_obj[std::string(cfg_name)].is_null()) {
                    config_store[entries.at(i)] = settings_obj[std::string(cfg_name)].string_value();
                }
            }
        }
    }

    //FOR KEY USER_DATA
    if(!_jsobj[INI_USER_DATA_CATEGORY_TOKEN].is_null() && _jsobj.find(INI_USER_DATA_CATEGORY_TOKEN) != _jsobj.end() && _jsobj[INI_USER_DATA_CATEGORY_TOKEN].is_object()) {
        json11::Json::object settings_obj = _jsobj[INI_USER_DATA_CATEGORY_TOKEN].object_items();
        //GET SINGLE ITEMS
        for (size_t i = 0; i < entries.size(); i++)
        {
            std::string_view cfg_name = magic_enum::enum_name(entries.at(i));
            if (std::string(cfg_name).empty()) {
                continue;
            }
            //GET OBJECT VALUE STORE IT IN CONFIG_STORE
            if(settings_obj.find(std::string(cfg_name)) != settings_obj.end() && settings_obj[std::string(cfg_name)].is_string()) {
                config_store[entries.at(i)] = settings_obj[std::string(cfg_name)].string_value();
            }
        }
    }

    cfg_loaded_success = true;
	return cfg_loaded_success;
}
bool ConfigParser::loadFromJson(std::string _jsonstr, bool load_only_user_data){
    if(_jsonstr.empty()){
        return false;
    }
    //PARSE JSON STRING
    std::string parse_err = "";
    json11::Json json_root = json11::Json::parse(_jsonstr.c_str(), parse_err);
    json11::Json::object root_obj = json_root.object_items();
    //CHECK PARSE RESULT
    if(!parse_err.empty()){
        return false;
    }

    return loadFromJson(root_obj,load_only_user_data);
}

ConfigParser::ConfigParser()
{
	reader  = INIReader();
}
	
ConfigParser::~ConfigParser()
{
}

ConfigParser* ConfigParser::getInstance()
{
	std::lock_guard<std::mutex> lck(ConfigParser::acces_lock_mutex);
	if (ConfigParser::instance == nullptr)
	{
		ConfigParser::instance = new ConfigParser();
	}
	return ConfigParser::instance;
}
	
bool ConfigParser::loadConfigFile(std::string _file)
{
	//PARSE GIVEN INI FILE
	reader = INIReader(_file);
	if (reader.ParseError() != 0)
	{
		return false;
	}
	
	//LOAD VALUES TO MAP
	constexpr auto& entries = magic_enum::enum_values<ConfigParser::CFG_ENTRY>();
	
	for (size_t i = 0; i < entries.size(); i++)
	{
		std::string_view cfg_name = magic_enum::enum_name(entries.at(i));
		if (std::string(cfg_name).empty()) {
			continue;
		}

        if (std::string(cfg_name).rfind(USER_DATA_CONFIG_ENTRY_PREFIX,0)==0) {
            config_store[entries.at(i)] = reader.Get(INI_USER_DATA_CATEGORY_TOKEN, std::string(cfg_name), "");
        }else{
            config_store[entries.at(i)] = reader.Get(INI_SETTINGS_CATEGORY_TOKEN, std::string(cfg_name), "");
        }

	}
	
	
	cfg_loaded_success = true;
	return true;
}

bool ConfigParser::writeConfigFile(std::string _file)
{
	//OPEN TEXT FILE
	std::ofstream myfile;
	myfile.open(_file.c_str());
	if (!myfile.is_open()) {
		return false;
	}
	//WRITE INI FILE CATEGORY
	myfile << "[" << INI_SETTINGS_CATEGORY_TOKEN << "]\n";
	//FOR EACH ENTRY IN ENUM
	constexpr auto& entries = magic_enum::enum_values<ConfigParser::CFG_ENTRY>();
	//WRITE NORMAL CONFIG
	for (size_t i = 0; i < entries.size(); i++)
	{
		std::string_view cfg_name = magic_enum::enum_name(entries.at(i));
		if (std::string(cfg_name).empty()) {
			continue;
		}
        if (std::string(cfg_name).rfind(USER_DATA_CONFIG_ENTRY_PREFIX,0)==0) {
            continue;
        }
		myfile << std::string(cfg_name) << "=" << config_store[entries.at(i)] << "\n";
	}
	//WRITE USER DATA CONFIG
    myfile << "[" << INI_USER_DATA_CATEGORY_TOKEN << "]\n";
    for (size_t i = 0; i < entries.size(); i++)
    {
        std::string_view cfg_name = magic_enum::enum_name(entries.at(i));
        if (std::string(cfg_name).empty()) {
            continue;
        }
        if (!(std::string(cfg_name).rfind(USER_DATA_CONFIG_ENTRY_PREFIX,0)==0)) {
            continue;
        }
        myfile << std::string(cfg_name) << "=" << config_store[entries.at(i)] << "\n";
    }


	//FINALLY WRITE FILE
	myfile.close();
	return true;
}

void ConfigParser::loadDefaults() {
	//DEFAULT CONFIG HARD CODED :)
//	config_store[ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL] = "http://192.168.178.24:3000";
	config_store[ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL] = "http://atomicchess.de:3000";
	
	config_store[ConfigParser::CFG_ENTRY::NETWORK_HEARTBEAT_INTERVAL_SECS] = "5";
	
	
	
	config_store[ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE] = "eth0";
	config_store[ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH] = "/VERSION";
	config_store[ConfigParser::CFG_ENTRY::GENERAL_HWREV_FILE_PATH] = "/etc/hwrevision";
	config_store[ConfigParser::CFG_ENTRY::GENERAL_BOOT_PARTION_INFO_FILE_PATH] = "/etc/swupdate/BOOTPART";
	config_store[ConfigParser::CFG_ENTRY::GENERAL_SYSTEM_TICK_INTERVAL_MS] = "1000";
	
	config_store[ConfigParser::CFG_ENTRY::GENERAL_EN_ATCGUI_COMMUNICATION] = "1";
	
	
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_STEPS_PER_MM] = "1292";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_INVERT_COILS] = "1";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X] = "-15"; //50 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y] = "40"; //0 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM] = "0"; //110 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_COIL_SWITCH_POSTION_TRIGGER] = "200";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH] = "55"; //50 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_BOARD_WIDTH] = "440"; //400 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE] = "5";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET] = "10";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE] = "520";//350 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET] = "5";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET] = "30";	
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE] = "30";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_SIZE_550MM_WORKAROUND] = "1";//1 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_MAKE_MOVE] = "0";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_WRITE_OFF] = "0";
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_HOME_AFTER_MAKE_MOVE] = "0"; //1 ON DK
	config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISBABLE_COILSIWTCH_USE_COIL_A_ONLY] = "1"; // 0 ON DK
	
	
	
		
	config_store[ConfigParser::CFG_ENTRY::BOARD_PRESET_START_POSITION_FEN] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	
	
	config_store[ConfigParser::CFG_ENTRY::HWARDWARE_REVISION] = "PROD";
	config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_PORT] = "/dev/ttyACM0";
	config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_BAUD] = "115200";
	config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX] = "0";
	config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_B_INDEX] = "1";
	config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS] = "0";
	config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS] = "115";
	
	
	config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_RANDOM_MOVE_MATCH] = "0";
	config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE] = "1";
	config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTOLOGIN] = "1";
	config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG] = "1";
	

}

void ConfigParser::set(ConfigParser::CFG_ENTRY _entry, std::string _value, std::string _conf_file_path)
{
	config_store[_entry] = _value;
	
	if (!_conf_file_path.empty())
	{
		writeConfigFile(_conf_file_path);
	}
}

bool ConfigParser::createConfigFile(std::string _file, bool _load_directly) {
		
	//LOAD DEFAULTS
	loadDefaults();

	
	//WRITE SETTINGS TO FILE
	writeConfigFile(_file);
	//LOAD FILE IF FLAG SET
	if(_load_directly)
	{
		return loadConfigFile(_file) ;
		
	}
	return true ;
}

bool ConfigParser::configLoaded()
{
	return cfg_loaded_success;
}

bool ConfigParser::getInt(ConfigParser::CFG_ENTRY _entry, int& _ret)
{
	
	if (!ConfigParser::getInstance()->get(_entry).empty()) {
		_ret = std::atoi(get(_entry).c_str());
		return true;
	}
	return false;
	
}

bool ConfigParser::getBool(ConfigParser::CFG_ENTRY _entry, bool& _ret)
{
	
	if (!ConfigParser::getInstance()->get(_entry).empty()) {
		
		if (get(_entry) == "true" || get(_entry) == "TRUE" || get(_entry) == "True") {
			_ret = true;
			return true;
		}else if(get(_entry) == "false" || get(_entry) == "FALSE" || get(_entry) == "False"){
			_ret = false;
			return true;
		}
		
		
		int ret = atoi(get(_entry).c_str());
		if (ret > 0)
		{
			_ret = true;
			return true;
		}
		else
		{
			_ret = false;
			return true;
		}
	}
	return false;
	
}

bool ConfigParser::getBool_nocheck(ConfigParser::CFG_ENTRY _entry)
{
	bool tmp = false;
	if (!getBool(_entry, tmp))
	{
		return false;
	}
		
	return tmp;
}

int ConfigParser::getInt_nocheck(ConfigParser::CFG_ENTRY _entry)
{
	int ret = 0;
	if (getInt(_entry,ret)) {
		return ret;
	}
	return 0;
}

std::string ConfigParser::get(ConfigParser::CFG_ENTRY _entry)
{
	if (!cfg_loaded_success) {
		return "";
	}
	
	return config_store[_entry];
}

