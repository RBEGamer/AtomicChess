//
// Created by marcel on 21.02.23.
//

#ifndef CONTROLLER_SERIALINTERFACE_H
#define CONTROLLER_SERIALINTERFACE_H


#include <string>
#include <vector>
#include <iostream>

#include <sys/socket.h>
#include <arpa/inet.h> // This contains inet_addr
#include <unistd.h> // This contains close

//3rd PARTY INCLUDES
#include "SHARED/loguru-master/loguru.hpp"

#include "simple_cpp_sockets.h"


// THIS LIB ALSO ALLOWS TO CONNECT TO A SER2NET SERVER USING THE FOLLOWING SPECIFIER AS SERIAL PORT STRING INSTEAD OF THE SERIAL PORT PATH
#define TCP_PROTOCOL_IDENTIFIER "tcp:" //if serial port start with tcp:<IP>:<PORT> then init ser2net client instead of open a regular serial connection
#define TCP_INVALID_SOCKET (SOCKET)(~0)
#define TCP_SOCKET_ERROR (-1)
#define TCP_CONNECTION_ERROR 5
#define TCP_MESSAGE_SEND_ERROR 6
#define TCP_RECIEVED_ERROR 7
typedef int SOCKET;

// THE ORIGINAL LIB CAUSES CONNECTION ERRORS IF RUNNING ON THE BUILDROOT SYSTEM
// SO AN OTHER LIB USAGE WAS IMPLEMENTED BUT NOT FULLY TESTED BUT SINGLE TESTS SUCCEEDED
//#define USE_ALTERNATIVE_SERIAL_LIB

#ifdef USE_ALTERNATIVE_SERIAL_LIB
//#include "SerialPort.hpp"
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

    bool close_port(); //CLOSES THE SERIAL OR TCP PORT
    static bool check_baud_rate(int _baudrate_to_check); //CHECKS A GIVEN BAUDRATE TO A STANDART VALID ONE,0

    SERIAL_PORT_STATE get_state() const;
    void dummy_read();
    std::string read_line();
    std::string read_until(char _termination, unsigned int _max_wait);
    bool write_string(std::string _data);
    bool write_string_tcp(const std::string _data);
    bool write_string_serial(const std::string _data);
    bool is_open() const;
private:

    bool init_serial_port(); //INIT (OR REINIT) THE SERIAL PORT AND OPENS IT
    bool init_tcp_port();
    bool connect_tcp_socket();
    std::string read_until_serial(char _termination, unsigned int _max_wait);
    std::string read_until_tcp(char _termination, unsigned int _max_wait);

    static void tokenize(std::string const &str, const char *delim, std::vector<std::string> &out);


    const int SERIAL_READ_DEFAULT_TIMEOUT = 500; //WAIT FOR x MS FOR AN ANSWER

#ifdef USE_ALTERNATIVE_SERIAL_LIB
    mn::CppLinuxSerial::SerialPort* port = nullptr;
    static mn::CppLinuxSerial::BaudRate convert_baud_rate(const int _baudrate_to_check);
#else
    serialib*  port = nullptr;
    bool connected = false;
#endif

    std::string serial_port_file; //or TCP IP ADDRESS
    int baud_rate;


    int tcp_port;
    bool is_network_port = false;
    SOCKET socket_fd{};
    sockaddr_in socket_addr{};
};


#endif //CONTROLLER_SERIALINTERFACE_H
