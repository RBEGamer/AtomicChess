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


//#define USE_ALTERNATIVE_SERIAL_LIB

#ifdef USE_ALTERNATIVE_SERIAL_LIB
#include "SerialPort.hpp"
#else
#include "SHARED/serialib-master/lib/serialib.h"
#endif


#include "ChessPiece.h"
#include "ConfigParser.h"

//THIS COMMANDS MATCHES THE MICROCONTROLLER FIRMWARE





class UserBoardController {
public:

    UserBoardController(std::string _serialport_file);
    UserBoardController(std::string _serialport_file,int _baud);
    ~UserBoardController();


    ChessPiece::FIGURE read_chess_piece_nfc();
    void set_led(int _color);
    void set_servo(int _pos);

private:
    const std::string UBC_COMMAND_VERSION = "version";
    const std::string  UBC_COMMAND_READNFC = "readnfc";
    const std::string  UBC_COMMAND_STATE = "state";
    const char UBC_CMD_SEPERATOR = '_';
    const std::string  UBC_COMMAND_LED = "led";
    const std::string  UBC_COMMAND_SERVO = "servo";
    const int GENERAL_UBC_COMMAND_RESPONSE_RETRY = 3; //RETRY X TIME TO SUCC SEND A COMMAND
    const int UBC_SERIAL_BAUD_RATE = 115200;
    const unsigned int SERIAL_READ_DEFAULT_TIMEOUT = 500; //WAIT FOR 2000 aMS FOR DATA READ -1 IS INFITIE TIMEOUT

#ifdef USE_ALTERNATIVE_SERIAL_LIB
    mn::CppLinuxSerial::SerialPort* port = nullptr;
#else
    serialib*  port = nullptr;
#endif

    std::string read_string_from_serial();
    std::string send_command(std::string _cmd, bool _blocking);

    bool init_serial_port(std::string _serial_port_file, int _baud_rate); //INIT (OR REINIT) THE SERIAL PORT AND OPENS IT
    bool close_serial_port(); //CLOSES THE SERIAL PORT
    bool check_baud_rate(int _baudrate_to_check); //CHECKS A GIVEN BAUDRATE TO A STANDART VALID ONE
    void dummy_read();
    static std::string get_value(std::string data, char separator, int index);
    static std::vector<std::string> split(std::string _input, char _char);

#ifdef USE_ALTERNATIVE_SERIAL_LIB
    mn::CppLinuxSerial::BaudRate convert_baud_rate(const int _baudrate_to_check);
#endif

};


#endif //CONTROLLER_USERBOARDCONTROLLER_H
