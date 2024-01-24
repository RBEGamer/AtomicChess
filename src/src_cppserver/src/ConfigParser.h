#pragma once
#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include <map>

///INCLUDE 3rd PARTY
#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/inih-master/INIReader.h"
#include "SHARED/magic_enum-master/include/magic_enum.hpp"
#include "SHARED/json11-master/json11.hpp"


#define USE_STD_LOG
#define INI_SETTINGS_CATEGORY_TOKEN "GLOBAL"


class ConfigParser
{
public:
	///PLEASE NOTE BY EDIT/ADDING ENTRIES PLEASE EDIT ConfigParser::createConfigFile, loadDefaults FUNCTIONS TOO
	enum class CFG_ENTRY
	{
        HTTP_HOST,
        HTTP_PORT,
        DB_FILEPATH,
        STATIC_HTML_PATH,
        INITIAL_BOARD_FEN
	};
	
	static ConfigParser* getInstance();
	bool loadConfigFile(std::string _file);
	bool createConfigFile(std::string _file, bool _load_directly); //GENERATES A CONFIG FILE WITH DEFAULTS
	bool configLoaded();
	std::string get(ConfigParser::CFG_ENTRY _entry);
	void set(ConfigParser::CFG_ENTRY _entry, std::string _value, std::string _conf_file_path); //SETS A CONFIG VALUE FOR ENTRY IF _conf_file_path IS NOT EMPTY THE NEW CONFIG WILL BE WRITTEN TO FILE
	void setInt(ConfigParser::CFG_ENTRY _entry, int _value, std::string _conf_file_path);  //SETS A CONFIG VALUE FOR ENTRY IF _conf_file_path IS NOT EMPTY THE NEW CONFIG WILL BE WRITTEN TO FILE
	bool writeConfigFile(std::string _file);
	bool getInt(ConfigParser::CFG_ENTRY _entry, int& _ret);
	int getInt_nocheck(ConfigParser::CFG_ENTRY _entry); //SAME AS getInt BUT WITH NO SUCCESS CHECK IF ENTRY FOUND; RETURN 0 IF ENTRY WAS NOT FOUND 
	bool getBool(ConfigParser::CFG_ENTRY _entry, bool& _ret);
	void loadDefaults(std::string _type_str);
	bool getBool_nocheck(ConfigParser::CFG_ENTRY _entry);//SAME AS getBool BUT WITH NO SUCCESS CHECK IF ENTRY FOUND; RETURN FALSE IF ENTRY WAS NOT FOUND 
	std::string toJson();
	bool loadFromJson(std::string _jsonstr, bool load_only_user_data); //loads config from json  load_only_user_data is TRUE = updates only the user data section with the new data from json
    bool loadFromJson(json11::Json::object _jsobj, bool load_only_user_data);

private:
    //USED IN toJson/loadFromJson
    struct KV_PAIR{
        std::string key = "";
        std::string value = "";
        KV_PAIR(std::string _k, std::string _v){
            key = _k;
            value = _v;
        }
        KV_PAIR(){
        }
    };

	static ConfigParser* instance;
	static std::mutex acces_lock_mutex;
	
	bool cfg_loaded_success = false;
	ConfigParser();
	~ConfigParser();
	//STORES THE ACUTAL CONFIG FILE
	std::map<ConfigParser::CFG_ENTRY, std::string> config_store;
	INIReader reader;
};
#endif
