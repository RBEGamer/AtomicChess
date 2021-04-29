//
// Created by prodevmo on 29.04.21.
//

#ifndef CONTROLLER_VOICERECOGNITIONEXTENTION_H
#define CONTROLLER_VOICERECOGNITIONEXTENTION_H

#include <string>
#include <list>
#include <vector>
//THREAD STUFF
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>


#include "SHARED/cpp-httplib-master/httplib.h"
#include "SHARED/json11-master/json11.hpp"
#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/magic_enum-master/include/magic_enum.hpp"
//INCLUDE LOCAL DEPENDENCIES
#include "ConfigParser.h"


class VoiceRecognitionExtention {


public:

    struct VR_MOVE{
        std::string figure;
        std::string to;
        bool valid;

    };

    VoiceRecognitionExtention(std::string _hwid, std::string _interface_name);

    VR_MOVE get_move();
    bool reset_move();
    bool enable_service(bool _en);

private:
    std::string ca_client_path = "./";
    std::string hwid;
    bool service_enabled;
    std::string base_url;
    std::string interface_name;
    const std::string URL_RESET_MOVE = "/reset_move"; // GET hwid query paramter
    const std::string URL_GET_MOVE = "/get_move"; //// GET hwid query paramter
    const std::string URL_ENABLE_SERVICE = "/enable_service_for_table"; //// GET hwid query paramter

    struct request_result
    {
        bool request_failed;
        std::string body;
        httplib::Error error;
        int status_code;
        std::string uri;
    };

    request_result make_request(std::string _url_path);
    void sanitize(std::string& stringValue);

};


#endif //CONTROLLER_VOICERECOGNITIONEXTENTION_H
