//
// Created by prodevmo on 12.06.21.
//


#include "DGT3000Interface.h"

DGT3000Interface::DGT3000Interface(const std::string _ver){
    version = _ver;
    dgt3000_button_event_queue = std::queue<DGT3000Buttons>();
}

void DGT3000Interface::enable_service(bool _en){

    if(_en){
        std::thread t1(recieve_thread_function_dgt, this);
        t1.detach();
        dgt_update_thread = &t1;
        thread_running = true;
        LOG_F(INFO,"DGT3000Interface start thread");
    }else{
        if (dgt_update_thread)
        {
            thread_running = false;
            dgt_update_thread->join();
            LOG_F(INFO,"DGT3000Interface stop thread");
        }
    }
}

bool DGT3000Interface::DGT3000Interface::set_dgt3000_text(const std::string _text){
    update_thread_mutex_set_text.lock();
    dgt_current_set_text  = _text;
    update_thread_mutex_set_text.unlock();
}


DGT3000Buttons DGT3000Interface::get_dgt3000_buttons_state(){
    DGT3000Buttons tmp;
    tmp.is_event_valid = false;
    update_thread_mutex.lock();
    if(!dgt3000_button_event_queue.empty()) {
        tmp = dgt3000_button_event_queue.front();
        dgt3000_button_event_queue.pop();
    }
    update_thread_mutex.unlock();
    return tmp;
}



void DGT3000Interface::recieve_thread_function_dgt(DGT3000Interface* _this){

    using namespace httplib;
    //REGISTER WEBSERVER EVENTS
    _this->svr.Get("/",
                   [](const Request& req, Response& res) {
                       res.set_redirect(VERSION_URL, 302);
                   });

    _this->svr.Get(VERSION_URL,
                   [_this](const Request& req, Response& res) {
                       res.set_content(_this->version, "text/html");
                   });

    _this->svr.Get(GET_TEXT_URL,
                   [_this](const Request& req, Response& res) {
        _this->update_thread_mutex_set_text.lock();
                        const  std::string cont = _this->dgt_current_set_text;
                        _this->update_thread_mutex_set_text.unlock();
                       res.set_content(cont, "text/html");
                   });


    _this->svr.Get(BTN_CHANGE_ROUTE,
                   [_this](const Request& req, Response& res) {
                        if(req.params.size() > 0){
                            DGT3000Buttons tmp;
                            tmp.is_event_valid =false;
                            auto it = req.params.find("raw");
                            if (it != req.params.end() && !it->second.empty()){
                                int curr_btn_state = curr_btn_state = std::stoi(it->second);
                                tmp.is_event_valid =true;
                                tmp.minus_btn = curr_btn_state & 0x02;
                                tmp.back_btn = curr_btn_state & 0x01;
                                tmp.play_btn = curr_btn_state & 0x04;
                                tmp.plus_btn = curr_btn_state & 0x08;
                                tmp.forward_btn = curr_btn_state & 0x10;
                                tmp.on_btn = curr_btn_state & 0x20;
                                tmp.lever_right_down = curr_btn_state & 0x40;
                                //ENQUEUE EVENT
                                _this->update_thread_mutex.lock();
                                _this->dgt3000_button_event_queue.push(tmp);
                                _this->update_thread_mutex.unlock();

                            }else{
                                //CONVERT MANUALLY MAYBE USEFUL FOR OTHER APPLICATIONS
                                int succ_counter = 0;
                                it = req.params.find("back_btn");
                                if(it != req.params.end() && !it->second.empty()){tmp.back_btn = std::stoi(it->second) & 0x02;succ_counter++;}
                                it = req.params.find("minus_btn");
                                if(it != req.params.end() && !it->second.empty()){tmp.minus_btn = std::stoi(it->second) & 0x02;succ_counter++;}
                                it = req.params.find("play_btn");
                                if(it != req.params.end() && !it->second.empty()){tmp.play_btn = std::stoi(it->second) & 0x02;succ_counter++;}
                                it = req.params.find("plus_btn");
                                if(it != req.params.end() && !it->second.empty()){tmp.plus_btn = std::stoi(it->second) & 0x02;succ_counter++;}
                                it = req.params.find("forward_btn");
                                if(it != req.params.end() && !it->second.empty()){tmp.forward_btn = std::stoi(it->second) & 0x02;succ_counter++;}
                                it = req.params.find("on_btn");
                                if(it != req.params.end() && !it->second.empty()){tmp.on_btn = std::stoi(it->second) & 0x02;succ_counter++;}
                                it = req.params.find("lever_right_down");
                                if(it != req.params.end() && !it->second.empty()){tmp.lever_right_down = std::stoi(it->second) & 0x02;succ_counter++;}
                                if(succ_counter == 7){
                                    tmp.is_event_valid =true;
                                    //ENQUEUE EVENT
                                    _this->update_thread_mutex.lock();
                                    _this->dgt3000_button_event_queue.push(tmp);
                                    _this->update_thread_mutex.unlock();
                                }
                            }
                        }
                       res.set_content("ok", "text/html");
                   });



    //START WEBSERVER
    if(!_this->svr.listen(DGT_WEBSERVER_BIND_ADDR, DGT_WEBSERVER_PORT)){
        LOG_F(ERROR,"DGT3000 WEBSERVER BIND FAILED");
    }
    while (_this->thread_running) {

    }
    _this->svr.stop();


}
