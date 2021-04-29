//
// Created by prodevmo on 29.04.21.
//

#include "VoiceRecognitionExtention.h"



VoiceRecognitionExtention::VoiceRecognitionExtention(std::string _hwid, std::string _interface_name){
    service_enabled = ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::USER_RESERVED_EXTENTION_VOICE_RECOGNITION_ENABLED);
    base_url = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::USER_RESERVED_EXTENTION_VOICE_RECOGNITION_URL);
    interface_name = _interface_name;
    if(service_enabled && base_url.empty()){
        service_enabled = false;
        LOG_F(ERROR,"VoiceRecognitionExtention BASE URL IS EMPTY BUT SERVICE ENABLED");
    }

    hwid = _hwid;
    if(service_enabled && hwid.empty()){
        service_enabled = false;
        LOG_F(ERROR,"VoiceRecognitionExtention HWID IS EMPTY BUT SERVICE ENABLED");
    }

    enable_service(service_enabled);
}

VoiceRecognitionExtention::VR_MOVE VoiceRecognitionExtention::get_move(){
    VoiceRecognitionExtention::VR_MOVE tmp;
    tmp.valid = false;

    if(!service_enabled){
    return tmp;
    }


    VoiceRecognitionExtention::request_result res = make_request(base_url + URL_GET_MOVE + "?hwid=" + hwid);

    if(res.request_failed){
        tmp.valid = false;
        return tmp;
    }


    std::string jp_err = "";
    json11::Json t = json11::Json::parse(res.body.c_str(), jp_err);
    if (jp_err.empty())
    {
        const std::string figure_str = ((json11::Json::object)t.object_items())["figure"].string_value();
        const std::string field_str = ((json11::Json::object)t.object_items())["to_field"].string_value();

        if(field_str.empty() || figure_str.empty()){
            return tmp;
        }
        tmp.figure = figure_str;
        tmp.to = field_str;
        tmp.valid = true;
    }
        return tmp;
}


bool VoiceRecognitionExtention::enable_service(bool _en){
    if(!service_enabled){
        return false;
    }
    VoiceRecognitionExtention::request_result res = make_request(base_url + URL_ENABLE_SERVICE + "?hwid=" + hwid + "&enabled=" + std::to_string((int)_en));
    return !res.request_failed;
}


bool VoiceRecognitionExtention::reset_move(){
    if(!service_enabled){
     return false;
    }
    VoiceRecognitionExtention::request_result res = make_request(base_url + URL_RESET_MOVE + "?hwid=" + hwid);
    return !res.request_failed;
}



///GET REQUEST
VoiceRecognitionExtention::request_result VoiceRecognitionExtention::make_request(std::string _url_path)
{
    //PORT AND PROTOCOL IS ALREADY INCLUDED IN BASE URL LIKE http://127.0.0.1:3000
    httplib::Client cli(base_url.c_str());
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
    req_res.uri =  base_url + _url_path;
    req_res.request_failed = false;
    //PERFORM REQUEST
    httplib::Result res = cli.Get(_url_path.c_str());
    if(res && res->status >= 200 && res->status < 300) {
        req_res.status_code = res->status;
        //CHECK STATUS CODE 200 IS VALID
        req_res.body = res->body;
        req_res.request_failed = false;
    }
    else {
        req_res.error = res.error();
        req_res.request_failed = true;
        req_res.body = "";
    }

    return req_res;
}