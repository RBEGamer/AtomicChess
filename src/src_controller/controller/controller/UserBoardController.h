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
//3rd PARTY INCLUDES
#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/serialib-master/lib/serialib.h"


#include "ChessPiece.h"
#include "ConfigParser.h"

//THIS COMMANDS MATCHES THE MICROCONTROLLER FIRMWARE

#define UBC_COMMAND_VERSION "version"
#define UBC_COMMAND_READNFC "readnfc"
#define UBC_COMMAND_STATE "state"
#define UBC_CMD_SEPERATOR '_'



class UserBoardController {
public:

    UserBoardController(std::string _serialport_file);
    UserBoardController(std::string _serialport_file,int _baud);
    ~UserBoardController();


    ChessPiece::FIGURE read_chess_piece_nfc();


private:
    const int UBC_SERIAL_BAUD_RATE = 115200;
    const unsigned int SERIAL_READ_DEFAULT_TIMEOUT = 500; //WAIT FOR 2000 aMS FOR DATA READ -1 IS INFITIE TIMEOUT
    serialib*  port = nullptr;
    std::string read_string_from_serial();
    std::string send_command_blocking(std::string _cmd);

    bool init_serial_port(std::string _serial_port_file, int _baud_rate); //INIT (OR REINIT) THE SERIAL PORT AND OPENS IT
    bool close_serial_port(); //CLOSES THE SERIAL PORT
    bool check_baud_rate(int _baudrate_to_check); //CHECKS A GIVEN BAUDRATE TO A STANDART VALID ONE
    void dummy_read();
    std::string get_value(std::string data, char separator, int index);
    std::vector<std::string> split(std::string _input, char _char);
};


#endif //CONTROLLER_USERBOARDCONTROLLER_H
