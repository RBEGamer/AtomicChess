//
// Created by prodevmo on 28.03.21.
//

#ifndef CONTROLLER_USERBOARDCONTROLLER_H
#define CONTROLLER_USERBOARDCONTROLLER_H

#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>      // std::stringstream, std::stringbuf
#include <filesystem>
#include <cstddef>         // std::size_t

//3rd PARTY INCLUDES
#include "SHARED/loguru-master/loguru.hpp"


#include "ChessPiece.h"
#include "ConfigParser.h"
#include "SerialInterface/SerialInterfaceBase.h"
//THIS COMMANDS MATCHES THE MICROCONTROLLER FIRMWARE





class UserBoardController {
public:

    UserBoardController(std::string _serialport_file,int _baud);
    ~UserBoardController();


    ChessPiece::FIGURE read_chess_piece_nfc();
    void set_led(int _color);
    void set_led_hsv(int _color);
    void set_servo(int _pos);

private:

    std::string send_command(std::string _cmd,const bool _blocking);
    static std::vector<std::string> split(const std::string& _input, const char _char);
    static std::string get_value(const std::string data,const char separator,const int index);
    static void tokenize(std::string const &str, const char *delim, std::vector<std::string> &out);

    const std::string UBC_COMMAND_VERSION = "version";
    const std::string  UBC_COMMAND_READNFC = "readnfc";
    const std::string  UBC_COMMAND_STATE = "state";
    const char UBC_CMD_SEPERATOR = '_';
    const std::string  UBC_COMMAND_LED = "led";
    const std::string  UBC_COMMAND_SERVO = "servo";
    const std::string  UBC_COMMAND_HSV = "hsv";
    const int GENERAL_UBC_COMMAND_RESPONSE_RETRY = 3; //RETRY X TIME TO SUCC SEND A COMMAND
    const int UBC_SERIAL_BAUD_RATE = 115200;
    const unsigned int SERIAL_READ_DEFAULT_TIMEOUT = 500; //WAIT FOR 2000 aMS FOR DATA READ -1 IS INFITIE TIMEOUT

    SerialInterfaceBase* serialport = nullptr;

};


#endif //CONTROLLER_USERBOARDCONTROLLER_H
