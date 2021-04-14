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
    LOG_F(INFO,"onfigParser::loadConfigFile WITH FILE %s", _file.c_str());
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
    LOG_F(INFO,"onfigParser::writeConfigfile save into %s", _file.c_str());
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



void ConfigParser::loadDefaults(std::string _type_str)
{

    LOG_F(INFO,"ConfigParser::loadDefaults FOR CONFIG %s", _type_str.c_str());

	if (_type_str == "DK")
	{
		config_store[ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL] = "http://atomicchess.de:3000";
		config_store[ConfigParser::CFG_ENTRY::NETWORK_HEARTBEAT_INTERVAL_SECS] = "5";

		config_store[ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE] = "eth0";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH] = "/VERSION";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_HWREV_FILE_PATH] = "/etc/hwrevision";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_BOOT_PARTION_INFO_FILE_PATH] = "/etc/swupdate/BOOTPART";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_SYSTEM_TICK_INTERVAL_MS] = "1000";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_EN_ATCGUI_COMMUNICATION] = "1";

		config_store[ConfigParser::CFG_ENTRY::MECHANIC_STEPS_PER_MM] = "1292"; //1292 ON TMC5160 DRIVER WITH MICROSTEPPING ENABLED
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_INVERT_COILS] = "1";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X] = "60";      //50 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y] = "10";      //0 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM] = "110";      //110 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_COIL_SWITCH_POSTION_TRIGGER] = "200";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X] = "45";      //50 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y] = "45";      //400 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE] = "5";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET] = "10";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE] = "370";     //350 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET] = "5";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET] = "30";	
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE] = "30";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_SIZE_550MM_WORKAROUND] = "1";     //1 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_MAKE_MOVE] = "0";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_WRITE_OFF] = "0";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_HOME_AFTER_MAKE_MOVE] = "1";      //1 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISBABLE_COILSIWTCH_USE_COIL_A_ONLY] = "0";      // 0 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_TRAVEL] = "-1"; //ON DK THIS OPTIONS ARE NOT SUPPORTED
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_MOVE] = "-1";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_SIWTHC_XY_AXIS] = "0";

		config_store[ConfigParser::CFG_ENTRY::BOARD_PRESET_START_POSITION_FEN] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

		config_store[ConfigParser::CFG_ENTRY::HWARDWARE_REVISION] = "DK";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_PORT] = "/dev/ttyACM0";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_BAUD] = "115200";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX] = "0";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_B_INDEX] = "1";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS] = "0";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS] = "115";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RESET_EEPROM_TO_DEFAULT_DURING_STARTUP] = "0";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RGBW_STRIP_SWITCH_GB] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_COOLING_FAN_DRIVER_INDEX] = "0";

        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_PORT] = "/dev/ttyUBC";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_BAUD] = "115200";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_NFC_READ_RETRY_COUNT] = "3";

        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_RANDOM_MOVE_MATCH] = "0";
		config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE] = "0";
		config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTOLOGIN] = "1";
		config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG] = "0";
		config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_MAKE_MOVE_MANUAL_TEST_DO_SYNC] = "0";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_USER_NFC_MOVE_SEARCH_AFTER_FIRST_CHANGE] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_AUTO_SCAN_BOARD_TIME_IF_USERS_TURN] = "-1";

	}else if (_type_str == "PROD_V1"){
		config_store[ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL] = "http://atomicchess.de:3000";
		config_store[ConfigParser::CFG_ENTRY::NETWORK_HEARTBEAT_INTERVAL_SECS] = "5";
	

		config_store[ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE] = "eth0";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH] = "/VERSION";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_HWREV_FILE_PATH] = "/etc/hwrevision";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_BOOT_PARTION_INFO_FILE_PATH] = "/etc/swupdate/BOOTPART";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_SYSTEM_TICK_INTERVAL_MS] = "1000";
		config_store[ConfigParser::CFG_ENTRY::GENERAL_EN_ATCGUI_COMMUNICATION] = "1";
	
	
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_STEPS_PER_MM] = "80"; //80 steps / mm for normal xy configuration
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_INVERT_COILS] = "1";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X] = "15";  //50 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y] = "40";  //0 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM] = "0";  //110 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_COIL_SWITCH_POSTION_TRIGGER] = "200";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X] = "56";  //50 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y] = "56";  //400 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE] = "5";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET] = "10";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE] = "520"; //350 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET] = "5";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET] = "30";	
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE] = "30";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_SIZE_550MM_WORKAROUND] = "1"; //1 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_MAKE_MOVE] = "0";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_WRITE_OFF] = "0";
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_HOME_AFTER_MAKE_MOVE] = "0";  //1 ON DK
		config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISBABLE_COILSIWTCH_USE_COIL_A_ONLY] = "1";  // 0 ON DK

        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_TRAVEL] = "8000";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_MOVE] = "5000";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_SIWTHC_XY_AXIS] = "0";

        config_store[ConfigParser::CFG_ENTRY::BOARD_PRESET_START_POSITION_FEN] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	
		config_store[ConfigParser::CFG_ENTRY::HWARDWARE_REVISION] = "PROD_V1";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_PORT] = "/dev/ttySKR";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_BAUD] = "115200";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX] = "0";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_B_INDEX] = "1";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS] = "0";
		config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS] = "115";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RESET_EEPROM_TO_DEFAULT_DURING_STARTUP] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RGBW_STRIP_SWITCH_GB] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_COOLING_FAN_DRIVER_INDEX] = "0";

        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_PORT] = "/dev/ttyUBC";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_BAUD] = "115200";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_NFC_READ_RETRY_COUNT] = "3";


        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_RANDOM_MOVE_MATCH] = "0";
		config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE] = "0";
		config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTOLOGIN] = "1";
		config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_MAKE_MOVE_MANUAL_TEST_DO_SYNC] = "0";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_USER_NFC_MOVE_SEARCH_AFTER_FIRST_CHANGE] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_AUTO_SCAN_BOARD_TIME_IF_USERS_TURN] = "10";

    }else if (_type_str == "VIRT"){
        config_store[ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL] = "http://atomicchess.de:3000";
        config_store[ConfigParser::CFG_ENTRY::NETWORK_HEARTBEAT_INTERVAL_SECS] = "5";


        config_store[ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE] = "eth0";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH] = "/tmp/VERSION";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_HWREV_FILE_PATH] = "/tmp/hwrevision";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_BOOT_PARTION_INFO_FILE_PATH] = "/tmp/BOOTPART";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_SYSTEM_TICK_INTERVAL_MS] = "1000";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_EN_ATCGUI_COMMUNICATION] = "1";


        config_store[ConfigParser::CFG_ENTRY::MECHANIC_STEPS_PER_MM] = "80"; //80 steps / mm for normal xy configuration
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_INVERT_COILS] = "1";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X] = "15";  //50 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y] = "40";  //0 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM] = "0";  //110 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_COIL_SWITCH_POSTION_TRIGGER] = "200";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X] = "56";  //50 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y] = "56";  //400 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE] = "5";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET] = "10";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE] = "520"; //350 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET] = "5";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET] = "30";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE] = "30";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_SIZE_550MM_WORKAROUND] = "1"; //1 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_MAKE_MOVE] = "0";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_WRITE_OFF] = "0";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_HOME_AFTER_MAKE_MOVE] = "0";  //1 ON DK
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISBABLE_COILSIWTCH_USE_COIL_A_ONLY] = "1";  // 0 ON DK

        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_TRAVEL] = "8000";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_MOVE] = "5000";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_SIWTHC_XY_AXIS] = "0";

        config_store[ConfigParser::CFG_ENTRY::BOARD_PRESET_START_POSITION_FEN] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        config_store[ConfigParser::CFG_ENTRY::HWARDWARE_REVISION] = "VIRT";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_PORT] = "/dev/ttySKR";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_BAUD] = "115200";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX] = "0";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_B_INDEX] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS] = "0";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS] = "115";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RESET_EEPROM_TO_DEFAULT_DURING_STARTUP] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RGBW_STRIP_SWITCH_GB] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_COOLING_FAN_DRIVER_INDEX] = "0";

        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_PORT] = "/dev/ttyUBC";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_BAUD] = "115200";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_NFC_READ_RETRY_COUNT] = "3";

        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_RANDOM_MOVE_MATCH] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTOLOGIN] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_MAKE_MOVE_MANUAL_TEST_DO_SYNC] = "0";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_USER_NFC_MOVE_SEARCH_AFTER_FIRST_CHANGE] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_AUTO_SCAN_BOARD_TIME_IF_USERS_TURN] = "-1";

    }
	else
	{
        config_store[ConfigParser::CFG_ENTRY::NETWORK_BACKEND_URL] = "http://atomicchess.de:3000";
        config_store[ConfigParser::CFG_ENTRY::NETWORK_HEARTBEAT_INTERVAL_SECS] = "5";

        config_store[ConfigParser::CFG_ENTRY::GENERAL_HWID_INTERFACE] = "eth0";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_VERSION_FILE_PATH] = "/VERSION";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_HWREV_FILE_PATH] = "/etc/hwrevision";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_BOOT_PARTION_INFO_FILE_PATH] = "/etc/swupdate/BOOTPART";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_SYSTEM_TICK_INTERVAL_MS] = "1000";
        config_store[ConfigParser::CFG_ENTRY::GENERAL_EN_ATCGUI_COMMUNICATION] = "1";

        config_store[ConfigParser::CFG_ENTRY::MECHANIC_STEPS_PER_MM] = "80";//80 steps / mm for CORE XY configuration
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_INVERT_COILS] = "1";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_X] = "50";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_H1_OFFSET_MM_Y] = "37";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISTANCE_COILS_MM] = "0";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_COIL_SWITCH_POSTION_TRIGGER] = "200";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_X] = "57";//55
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_CHESS_FIELD_WIDTH_Y] = "57";//440
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_X_LINE] = "0";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_BLACK_FIRST_Y_OFFSET] = "7";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_X_LINE] = "510";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_WHITE_FIRST_Y_OFFSET] = "5";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_BEFORE_OFFSET] = "30";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_PARK_POS_CELL_SIZE] = "30";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_SIZE_550MM_WORKAROUND] = "1";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_MAKE_MOVE] = "0";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_WRITE_COIL_STATE_ALWAYS_WRITE_OFF] = "0";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_BOARD_HOME_AFTER_MAKE_MOVE] = "0";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_DISBABLE_COILSIWTCH_USE_COIL_A_ONLY] = "1";

        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_TRAVEL] = "7000";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_MOVE] = "4000";
        config_store[ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_SIWTHC_XY_AXIS] = "0";


        config_store[ConfigParser::CFG_ENTRY::BOARD_PRESET_START_POSITION_FEN] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        config_store[ConfigParser::CFG_ENTRY::HWARDWARE_REVISION] = "PROD_V2";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_PORT] = "/dev/ttySKR";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_BAUD] = "115200";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX] = "0";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_B_INDEX] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS] = "180";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS] = "85";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RESET_EEPROM_TO_DEFAULT_DURING_STARTUP] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RGBW_STRIP_SWITCH_GB] = "1";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_COOLING_FAN_DRIVER_INDEX] = "0";

        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_PORT] = "/dev/ttyUBC";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_BAUD] = "115200";
        config_store[ConfigParser::CFG_ENTRY::HARDWARE_UBC_NFC_READ_RETRY_COUNT] = "3";

        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_RANDOM_MOVE_MATCH] = "0";
        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE] = "0";
        config_store[ConfigParser::CFG_ENTRY::USER_GENERAL_ENABLE_AUTOLOGIN] = "0";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_CHESS_PLACEMENT_DIALOG] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_MAKE_MOVE_MANUAL_TEST_DO_SYNC] = "0";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_SKIP_USER_NFC_MOVE_SEARCH_AFTER_FIRST_CHANGE] = "1";
        config_store[ConfigParser::CFG_ENTRY::USER_RESERVED_AUTO_SCAN_BOARD_TIME_IF_USERS_TURN] = "-1";

    }
}


void ConfigParser::set(ConfigParser::CFG_ENTRY _entry, std::string _value, std::string _conf_file_path)
{

    LOG_F(INFO,"ConfigParser::set UPDATE CONFIG ENTRY %s, %s", _value.c_str(), std::string(magic_enum::enum_name(_entry)).c_str());
	config_store[_entry] = _value;
	
	if (!_conf_file_path.empty())
	{
		writeConfigFile(_conf_file_path);
	}
}

void ConfigParser::setInt(ConfigParser::CFG_ENTRY _entry, int _value, std::string _conf_file_path)
{
	set(_entry,std::to_string(_value) , _conf_file_path);
}


bool ConfigParser::createConfigFile(std::string _file, bool _load_directly) {
		

	
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
        LOG_F(INFO,"ConfigParser::getBool_nocheck ERROR READ KEY FAILED%s", std::string(magic_enum::enum_name(_entry)).c_str());
		return false;
	}
		
	return tmp;
}

int ConfigParser::getInt_nocheck(ConfigParser::CFG_ENTRY _entry)
{
	int ret = 0;
	if (!getInt(_entry,ret)) {
        LOG_F(INFO,"ConfigParser::getInt_nocheck ERROR READ KEY FAILED%s", std::string(magic_enum::enum_name(_entry)).c_str());
		return 0;
	}
	return ret;
}

std::string ConfigParser::get(ConfigParser::CFG_ENTRY _entry)
{
	if (!cfg_loaded_success) {
		return "";
	}
	
	return config_store[_entry];
}

