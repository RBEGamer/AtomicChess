//
// Created by marcel on 21.02.23.
//

#ifndef CONTROLLER_SERIALINTERFACE_SERIAL_H
#define CONTROLLER_SERIALINTERFACE_SERIAL_H


#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <filesystem>


//3rd PARTY INCLUDES
#include "../SHARED/loguru-master/loguru.hpp"



#include "serialib.h"
#include "SerialInterfaceBase.h"


class SerialInterfaceSerial: public SerialInterfaceBase {

public:

    SerialInterfaceSerial(const std::string& _serial_port_file,int _baud_rate);
    ~SerialInterfaceSerial();

    bool init();
    bool close(); //CLOSES THE SERIAL OR TCP PORT
    SerialInterfaceBase::SERIAL_PORT_STATE get_state() override;
    void dummy_read();
    std::string read_line();
    std::string read_until(char _termination, unsigned int _max_wait); 
    bool write_string(std::string _data);
    bool is_open() override;
    
    private:
    static bool check_baud_rate(int _baudrate_to_check); //CHECKS A GIVEN BAUDRATE TO A STANDART VALID ONE,0

    std::string serial_port_file; //ORIGINAL => MAYBE SYMLINK
    std::string serial_port_file_path;
    int baud_rate;

    serialib*  port = nullptr;
    bool connected = false;

    const int SERIAL_READ_DEFAULT_TIMEOUT = 200; //WAIT FOR x MS FOR AN ANSWER

  
};


#endif //CONTROLLER_SERIALINTERFACE_H
