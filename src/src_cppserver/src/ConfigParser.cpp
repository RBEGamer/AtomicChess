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
        settings[std::string(cfg_name)] = config_store[entries.at(i)];
    }


    //SUMP TO JSON
    root[INI_SETTINGS_CATEGORY_TOKEN] = settings;

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
        config_store[entries.at(i)] = reader.Get(INI_SETTINGS_CATEGORY_TOKEN, std::string(cfg_name), "");
	}
	cfg_loaded_success = true;
	return true;
}

bool ConfigParser::writeConfigFile(std::string _file)
{
    LOG_F(INFO,"ConfigParser::writeConfigfile save into %s", _file.c_str());
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

		myfile << std::string(cfg_name) << "=" << config_store[entries.at(i)] << "\n";
	}
	//FINALLY WRITE FILE
	myfile.close();
	return true;
}



void ConfigParser::loadDefaults(std::string _type_str)
{
    LOG_F(INFO,"ConfigParser::loadDefaults FOR CONFIG %s", _type_str.c_str());
    config_store[ConfigParser::CFG_ENTRY::HTTP_HOST] = "0.0.0.0";
    config_store[ConfigParser::CFG_ENTRY::HTTP_PORT] = "9999";
    config_store[ConfigParser::CFG_ENTRY::DB_FILEPATH] = "atcserver.sqlite";
    config_store[ConfigParser::CFG_ENTRY::STATIC_HTML_PATH] = "html_static/public";
    config_store[ConfigParser::CFG_ENTRY::INITIAL_BOARD_FEN] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    config_store[ConfigParser::CFG_ENTRY::AI_ALGORITHM] = "RNG";
    config_store[ConfigParser::CFG_ENTRY::UCI_ENGINE_FILEPATH] = "";
    config_store[ConfigParser::CFG_ENTRY::RESET_GAME_DATABASE_ON_START] = "1";


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

