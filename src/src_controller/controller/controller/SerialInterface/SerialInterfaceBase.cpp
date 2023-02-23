//
// Created by marcel on 23.02.23.
//

#include <string>
#include <cstring>
#include <vector>

#include "../SHARED/loguru-master/loguru.hpp"

#include "SerialInterfaceBase.h"
#include "SerialInterfaceSerial.h"


// TODO MOVE TO HELPER FUNCTION TOGEHTER WITH SAME FKT IN USBER BOARD CONTROLLER
void SerialInterfaceBase::tokenize(std::string const &str, const char *delim, std::vector<std::string> &out)
{
    char *token = strtok(const_cast<char *>(str.c_str()), delim);
    while (token != nullptr)
    {
        out.emplace_back(token);
        token = strtok(nullptr, delim);
    }
}

SerialInterfaceBase *SerialInterfaceBase::get_interface_instance(const std::string &_serial_port_file, int _baud_rate) {
    if (_serial_port_file.empty())
    {
        LOG_F(ERROR, "serial port _serialport_file is empty");
        return nullptr;
    }

    // CHECK FOR SERIAL PORT TYPE
    // THIS CLASS WAS ADDED TO SUPPORT SER2NET TCP SOCKETS TOO SO WE NEED TO CHECK FOR THAT
    // tcp://127.0.0.1:7001 for ser2net
    if (_serial_port_file.find(":")  != std::string::npos)
    {
        LOG_F(INFO, "serial is a tcp ser2net port ");

        std::vector<std::string> out;
        tokenize(_serial_port_file, ":", out);
        if (out.size() == 3)
        {
            if(out.at(0) != "tcp"){
                LOG_F(ERROR, "serial git invalid network protocol %s only tcp is supported", out.at(0).c_str());
                return nullptr;
            }
            // return new SerialInterfaceTCP(out.at(1), std::stoi(out.at(2)));
        }
        else
        {
            LOG_F(ERROR, "serial tcp format invlaid, got: %s - please use format tcp:<IP>:<PORT>", _serial_port_file.c_str());
            return nullptr;
        }
    }
    else
    {
        return new SerialInterfaceSerial(_serial_port_file, _baud_rate);
    }
    return nullptr;
}
