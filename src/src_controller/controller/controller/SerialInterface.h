//
// Created by marcel on 21.02.23.
//

#ifndef CONTROLLER_SERIALINTERFACE_H
#define CONTROLLER_SERIALINTERFACE_H


#include <string>
//3rd PARTY INCLUDES
#include "SHARED/loguru-master/loguru.hpp"



#define TCP_PROTOCOL_IDENTIFIER "tcp:" //if serial port start with tcp:<IP>:<PORT> then init ser2net client instead of open a regular serial connection
//THE ORIGINAL LINUX VERSION IS ONLY COMPATIBLE WITH LINUX SYSTEMS
//THE NEWER BETA VERSION HAS A OPTION TO USE AN OTHER SERIAL PORT LIB BUT ITS NOT TESTED ON WIN SYSTEMS
//#define USE_ALTERNATIVE_SERIAL_LIB

#ifdef USE_ALTERNATIVE_SERIAL_LIB
#include "SerialPort.hpp"
#else
#include "SHARED/serialib-master/lib/serialib.h"
#endif

class SerialInterface {

public:

    enum class SERIAL_PORT_STATE{
        CLOSED,
        OPEN
    };

    SerialInterface(const std::string& _serial_port_file,int _baud_rate);
    ~SerialInterface();

    bool init();

    bool close_serial_port(); //CLOSES THE SERIAL PORT
    static bool check_baud_rate(int _baudrate_to_check); //CHECKS A GIVEN BAUDRATE TO A STANDART VALID ONE,0

    SERIAL_PORT_STATE get_state();
    void dummy_read();
    std::string read_line();
    std::string read_until(char _termination, unsigned int _max_wait);
    bool write_string(std::string _data);
    bool is_open();
private:

    bool init_serial_port(); //INIT (OR REINIT) THE SERIAL PORT AND OPENS IT
    bool init_tcp_port();

    std::string read_until_serial(char _termination, unsigned int _max_wait);
    std::string read_until_tcp(char _termination, unsigned int _max_wait);



    const int SERIAL_READ_DEFAULT_TIMEOUT = 500; //WAIT FOR x MS FOR AN ANSWER

#ifdef USE_ALTERNATIVE_SERIAL_LIB
    mn::CppLinuxSerial::SerialPort* port = nullptr;
    static mn::CppLinuxSerial::BaudRate convert_baud_rate(const int _baudrate_to_check);
#else
    serialib*  port = nullptr;
    bool connected = false;
#endif

    std::string serial_port_file;
    int baud_rate;
    bool is_network_port = false;

};


#endif //CONTROLLER_SERIALINTERFACE_H
