//
// Created by marcel on 21.02.23.
//

#include <thread>
#include <filesystem>
#include "SerialInterface.h"


SerialInterface::SerialInterface(const std::string& _serial_port_file, const int _baud_rate) {
    if (_serial_port_file.empty()) {
        LOG_F(ERROR, "serial port _serialport_file is empty");
        return;
    }

    serial_port_file = _serial_port_file;
    LOG_F(INFO, "serial port set to %s", serial_port_file.c_str());
    //CHECK FOR SERIAL PORT TYPE
    //THIS CLASS WAS ADDED TO SUPPORT SER2NET TCP SOCKETS TOO SO WE NEED TO CHECK FOR THAT
    //tcp://127.0.0.1:7001 for ser2net
    if (_serial_port_file.rfind(TCP_PROTOCOL_IDENTIFIER, 0) == 0) {
        LOG_F(INFO, "serial is a tcp ser2net port ");
        is_network_port = true;
        // s starts with prefix
    } else {

        //A BAUDRATE IS NEEDED HERE
        baud_rate = _baud_rate;
        LOG_F(INFO, "serial baud set to %i", baud_rate);
        is_network_port = false;
    }
}


SerialInterface::~SerialInterface() {
    if (is_network_port) {

    } else {
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        if (port) {
            port->Close();
        }
#else
        port->closeDevice();
        connected =false;
#endif
    }
}


SerialInterface::SERIAL_PORT_STATE SerialInterface::get_state() {

    if (is_network_port) {

    } else {
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        if (!port) {
            return SERIAL_PORT_STATE::CLOSED;
        } else if (port->GetState() == mn::CppLinuxSerial::State::OPEN) {
            return SERIAL_PORT_STATE::OPEN;
        } else {
            return SERIAL_PORT_STATE::CLOSED;
        }
#else
        if(connected){
            return SERIAL_PORT_STATE::OPEN;
        }else{
            return SERIAL_PORT_STATE::CLOSED;
        }
#endif
    }

    return SerialInterface::SERIAL_PORT_STATE::CLOSED;
}

bool SerialInterface::init() {
    if (is_network_port) {
        return init_tcp_port();
    } else {
        return init_serial_port();
    }
}

bool SerialInterface::init_tcp_port() {

}

bool SerialInterface::init_serial_port() {

#ifndef USE_ALTERNATIVE_SERIAL_LIB
    if (port == nullptr){
        port = new serialib();
    }
#endif
    if (baud_rate <= 0 || !check_baud_rate(baud_rate)) {
        LOG_F(ERROR, "serial port baudrate is invalid %i", baud_rate);
        return false;
    }
    //configure serialport though 3rd party lib
    close_serial_port();     //CLOSE PORT IF OPEN

    if (std::filesystem::is_symlink(serial_port_file)) {
        LOG_F(INFO, "filesystem::is_symlink(_port) %s", serial_port_file.c_str());
        //GET PREFIX EG /dev/
        std::string prefix;
        const size_t slash_index = serial_port_file.find_last_of("/\\");
        if (slash_index) {
            prefix = serial_port_file.substr(0, slash_index) + '/';
        }
        //ADD SYMLINK DESTINATION ttyTEST =>prefix + ttyUSB0 = /dev/ttyUSB0
        serial_port_file = prefix + std::filesystem::read_symlink(serial_port_file).string();
        LOG_F(INFO, "filesystem::read_symlink(_port) %s", serial_port_file.c_str());
    }

    //TRY TO CONNECT
    bool con_succ = false;
    for (int i = 0; i < 5; i++) {

#ifdef USE_ALTERNATIVE_SERIAL_LIB
        const mn::CppLinuxSerial::BaudRate baud = convert_baud_rate(baud_rate);
        //IF PORT IS ALREADY CREATED => ONLY SET NEW VALUES
        if (port) {
            port->Close();
            port->SetDevice(serial_port_file);
            port->SetNumDataBits(mn::CppLinuxSerial::NumDataBits::EIGHT);
            port->SetNumStopBits(mn::CppLinuxSerial::NumStopBits::ONE);
            port->SetParity(mn::CppLinuxSerial::Parity::NONE);
            port->SetBaudRate(baud);
            port->SetTimeout(SERIAL_READ_DEFAULT_TIMEOUT);
        } else {
            port = new mn::CppLinuxSerial::SerialPort(serial_port_file, baud, mn::CppLinuxSerial::NumDataBits::EIGHT,
                                                      mn::CppLinuxSerial::Parity::NONE,
                                                      mn::CppLinuxSerial::NumStopBits::ONE,
                                                      mn::CppLinuxSerial::HardwareFlowControl::ON,
                                                      mn::CppLinuxSerial::SoftwareFlowControl::ON);
            port->SetTimeout(SERIAL_READ_DEFAULT_TIMEOUT);
        }
        //TRY TO OPEN
        port->Open();
        //CHECK STATE
        if (port->GetState() == mn::CppLinuxSerial::State::OPEN) {
            con_succ = true;
            break;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
#else
        if(port->openDevice(serial_port_file.c_str(), baud_rate) != 1) {
            LOG_F(INFO, "(loop %i ) serial port open failed %s WITH BAUD %i", i, serial_port_file.c_str(),baud_rate);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }else{
            con_succ = true;
            break;
        }
#endif
    }

    if (!con_succ) {
        LOG_F(ERROR, "serial port open failed %s WITH BAUD %i", serial_port_file.c_str(), baud_rate);
        return false;
    }

#ifndef USE_ALTERNATIVE_SERIAL_LIB
    //ENABLE RESET
    port->DTR(false);
    port->RTS(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    port->DTR(true);
    port->RTS(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    port->DTR(true);
    port->RTS(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    port->DTR(false);
    port->RTS(false);
    connected = true;
#endif
    return true;
}

bool SerialInterface::close_serial_port() {
    if (port != nullptr) {
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        if (port->GetState() == mn::CppLinuxSerial::State::OPEN) {
            port->Close();
        }
#else
        port->closeDevice();
#endif
    }
    return true;
}

void SerialInterface::dummy_read() {
    char c = 128;
    std::string complete;
    int timeout = 0;
    while (true) {
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        std::string readData;
        if (!port) {
            LOG_SCOPE_F(ERROR, "SerialInterface::port is null => call init()");
            return;
        }
        port->Read(readData);//port->readChar(&c, SERIAL_READ_DEFAULT_TIMEOUT);
        if (readData.empty()) {
            break;
        } else {
            timeout++;
            if (timeout > 3) {
                break;
            }
        }
#else
        const char resp = port->readChar(&c, SERIAL_READ_DEFAULT_TIMEOUT);
        if (resp == 1){
            complete += c;
        }
        else if (resp == -2) //ERROR
        {
            break;
        }else{
            timeout++;
            if (timeout > 3){
                break;
            }
        }
#endif
    }
}

std::string SerialInterface::read_line() {
    return read_until('\n', SERIAL_READ_DEFAULT_TIMEOUT);
}

std::string SerialInterface::read_until(char _termination, unsigned int _max_wait) {
    if (is_network_port) {
        return read_until_tcp(_termination, _max_wait);
    } else {
        return read_until_serial(_termination, _max_wait);
    }
}

std::string SerialInterface::read_until_tcp(char _termination, unsigned int _max_wait) {

}

std::string SerialInterface::read_until_serial(char _termination, unsigned int _max_wait) {
    int wait_counter = 0;
    std::string complete = "";

    char charr[1024] = {0};

    while (true) {
        //READ CHARS FORM SERIAL
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        if (!port) {
            LOG_SCOPE_F(ERROR, "SerialInterface::port is null => call init()");
            return "";
        }
        port->Read(complete);
#else
        const int chars_read = port->readString(charr, _termination, 1024, _max_wait);
        if (chars_read > 0) {
            for (int i = 0; i < chars_read; i++)
            {
                complete += charr[i];
            }
        }
#endif
        //CHECK READ RESULT
        if (!complete.empty()) {
            break;
        } else {
            wait_counter++;
        }
        if (wait_counter > 10) {
            break;
        }
    }


    if(!complete.empty()){
        std::cout << "GCODE-RS: -" << complete.c_str() << "-" << std::endl;
    }

    return complete;
}

bool SerialInterface::write_string(const std::string _data) {
    //WRITE COMMAND TO SERIAL LINE
    if (!port) {
        LOG_SCOPE_F(ERROR, "SerialInterface::port is null => call init()");
        return false;
    }
#ifdef USE_ALTERNATIVE_SERIAL_LIB
    port->Write(_data);
#else
    port->flushReceiver();
    const char pwriteret = port->writeString(_data.c_str());
    if(pwriteret){
        return true;
    }
#endif
        return false;
}


#ifdef USE_ALTERNATIVE_SERIAL_LIB
mn::CppLinuxSerial::BaudRate SerialInterface::convert_baud_rate(const int _baudrate_to_check) {
    switch (_baudrate_to_check) {
        case 9600:
            return mn::CppLinuxSerial::BaudRate::B_9600;
        case 19200:
            return mn::CppLinuxSerial::BaudRate::B_19200;
        case 38400:
            return mn::CppLinuxSerial::BaudRate::B_38400;
        case 57600:
            return mn::CppLinuxSerial::BaudRate::B_57600;
        case 115200:
            return mn::CppLinuxSerial::BaudRate::B_115200;
        case 230400:
            return mn::CppLinuxSerial::BaudRate::B_230400;
        case 460800:
            return mn::CppLinuxSerial::BaudRate::B_460800;
        default:
            return mn::CppLinuxSerial::BaudRate::B_0;
    }
}

#endif

bool SerialInterface::check_baud_rate(const int _baudrate_to_check) {
#ifdef __MACH__
    return true;
#endif

#ifdef USE_ALTERNATIVE_SERIAL_LIB
    return convert_baud_rate(_baudrate_to_check) != mn::CppLinuxSerial::BaudRate::B_0;
#else
    switch (_baudrate_to_check) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        default:
            return 0;
    }
#endif
}

bool SerialInterface::is_open() {
    if (get_state() >= SERIAL_PORT_STATE::OPEN) {
        return true;
    } else {
        return false;
    }
}


