//
// Created by prodevmo on 12.06.21.
//

#ifndef CONTROLLER_DGT3000INTERFACE_H
#define CONTROLLER_DGT3000INTERFACE_H

#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/cpp-httplib-master/httplib.h"



// ACCORDING THE src_dgt3000chessclockextention/ATC_DGT3000_EXT/atcdgt3000ext.ini ---- //
#define BTN_CHANGE_ROUTE "/btn_changed"
#define GET_TEXT_URL "/get_text"
#define VERSION_URL "/version"
#define DGT_WEBSERVER_PORT 5000
#define DGT_WEBSERVER_BIND_ADDR "0.0.0.0"
struct DGT3000Buttons{
    bool back_btn;
    bool minus_btn;
    bool play_btn;
    bool plus_btn;
    bool forward_btn;
    bool on_btn;
    bool lever_right_down;
    bool is_event_valid;
};


class DGT3000Interface {

public:
    DGT3000Interface(const std::string _ver);
    void enable_service(bool _en); //START STOP WEBSERVER THREAD

    bool set_dgt3000_text(const std::string _text);
    DGT3000Buttons get_dgt3000_buttons_state();


private:
    std::string version = "V0.0.0";
    std::string dgt_current_set_text = "";
    std::thread* dgt_update_thread = nullptr;
    std::mutex update_thread_mutex; //get button events
    std::mutex update_thread_mutex_set_text;
    bool thread_running = false;
    httplib::Server svr;
    std::queue<DGT3000Buttons> dgt3000_button_event_queue;
    static void recieve_thread_function_dgt(DGT3000Interface* _this);
};


#endif //CONTROLLER_DGT3000INTERFACE_H
