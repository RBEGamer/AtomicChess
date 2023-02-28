//
// Created by marcel on 21.02.23.
//


#include "SerialInterfaceSerial.h"






SerialInterfaceSerial::SerialInterfaceSerial(const std::string &_serial_port_file, int _baud_rate) {
    LOG_SCOPE_F(INFO, "SerialInterfaceSerial::SerialInterfaceSerial");
    if (_serial_port_file.empty())
    {
        LOG_F(ERROR, "serial port _serialport_file is empty %s", _serial_port_file.c_str());
        return;
    }
    //OPEN SERIAL PORT
    if (port == nullptr)
    {
        port = new serialib();
    }
    serial_port_file = _serial_port_file;
    baud_rate = _baud_rate;
}

SerialInterfaceSerial::~SerialInterfaceSerial() {
    SerialInterfaceSerial::close();
    if (port != nullptr)
    {
        delete port;
    }
}

bool SerialInterfaceSerial::init() {
    LOG_SCOPE_F(INFO, "GCodeSender::init_serial_port");
    if (port == nullptr)
    {
        LOG_F(ERROR, "serial port instance is null");
        return false;
    }
    if (baud_rate <= 0 || !check_baud_rate(baud_rate))
    {
        LOG_F(ERROR, "serial port baudrate is invalid %i", baud_rate);
        return false;
    }
    //configure serialport though 3rd party lib
    SerialInterfaceSerial::close();     //CLOSE PORT IF OPEN

    if(!std::filesystem::exists(serial_port_file)){
        LOG_F(ERROR, "serial port %s does not exists", serial_port_file.c_str());
        return false;
    }
    if(std::filesystem::is_symlink(serial_port_file)){
        LOG_F(INFO, "filesystem::is_symlink(_port) %s", serial_port_file.c_str());
        //GET PREFIX EG /dev/
        std::string prefix = "";
        const size_t slash_index = serial_port_file.find_last_of("/\\");
        if(slash_index){
            prefix = serial_port_file.substr(0,slash_index) + '/';
        }
        //ADD SYMLINK DESTINATION ttyTEST =>prefix + ttyUSB0 = /dev/ttyUSB0
        serial_port_file_path = prefix + std::filesystem::read_symlink(serial_port_file).string();
        LOG_F(INFO, "filesystem::read_symlink(_port) %s", serial_port_file_path.c_str());
    }else{
        serial_port_file_path = serial_port_file;
    }

    if(port->openDevice(serial_port_file_path.c_str(), baud_rate) != 1) {
        LOG_F(ERROR, "serial port open failed %s WITH BAUD %i",serial_port_file_path.c_str(), baud_rate);
        return false;
    }

    //RESET CONNECTED DEVICE
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

    //READ MAY EXISITNG BUFFER => IGNORING MARLIN CONNECTION INFO LIKE Printer Online,...
    dummy_read();
    connected = true;
    return true;

}

bool SerialInterfaceSerial::close() {
    if (port != nullptr)
    {
        port->closeDevice();
        connected = false;
    }
    return true;
}

SerialInterfaceBase::SERIAL_PORT_STATE SerialInterfaceSerial::get_state(){
    if (port == nullptr)
    {
        return SERIAL_PORT_STATE::CLOSED;
    }


    if(fcntl(port->get_fd(), F_GETFL) != -1 || errno != EBADF){
        return SERIAL_PORT_STATE::OPEN;
    }

    if(connected){
        return SERIAL_PORT_STATE::OPEN;
    }

    return SERIAL_PORT_STATE::CLOSED;
}

void SerialInterfaceSerial::dummy_read() {
    if(!port){
        LOG_F(INFO, "port is nullptr please use init() first");
        return;
    }
    port->flushReceiver();
    char c = 128;
    std::string complete;
    int timeout = 0;
    while (c != 0)
    {
        char resp = port->readChar(&c, SERIAL_READ_DEFAULT_TIMEOUT);
        if (resp == 1)
        {
            complete += c;
        }
        else if (resp == -2) //ERROR
        {
            break;
        }else if (resp == 0) //TIMEOUT REACHED //ADDED ------------------------------------
        {
            break;
        }
        else
        {
            timeout++;
            if (timeout > 3)
            {
                break;
            }
        }
    }
    if(complete.length()){
        std::cout << complete.c_str() << std::endl;
    }
}

std::string SerialInterfaceSerial::read_line() {
    return read_until('\n',SERIAL_READ_DEFAULT_TIMEOUT);
}

std::string SerialInterfaceSerial::read_until(char _termination, unsigned int _max_wait) {
    if(!port){
        LOG_F(INFO, "port is nullptr please use init() first");
        return "";
    }
    int wait_counter = 0;
    std::string complete;


    char charr[128] = { 0 };
    while (true){
        //READ CHARS FORM SERIAL
        int chars_read = port->readString(charr, _termination, 128, _max_wait);
        if (chars_read > 0) {
            //complete.append(charr, chars_read);
            for (int i = 0; i < chars_read; i++) {
                const char c = charr[i];
                if (c > 0) {
                    complete += c;
                }
                if (c == '\r' || c == '\n') {
                    //port->flushReceiver();
                    break;
                }
            }

        //}else if (chars_read == 0){ //TIMEOUT REACHED //ADDED ------------------------------------
         //       break;

        }else{
            wait_counter++;
        }
        // CHECK TIMEOUT
        if (wait_counter > 10)
        {
            break;
        }
    }

    if(complete.size()>0){
        std::cout << complete.c_str() << std::endl;
    }
    return complete;
}

bool SerialInterfaceSerial::write_string(std::string _data) {
    if(!port){
        LOG_F(INFO, "port is nullptr please use init() first");
        return "";
    }
    if(_data.size() <= 0){
        LOG_F(WARNING, "nothing to send");
        return true;
    }
    port->flushReceiver();
    port->writeString(_data.c_str());
    return true;
}

bool SerialInterfaceSerial::check_baud_rate(int _baudrate_to_check) {
#ifdef __MACH__
    return true;
#endif
    switch (_baudrate_to_check)
    {
        case 9600:
            //return B9600;
            return true;
        case 19200:
            //return B19200;
            return true;
        case 38400:
            //return B38400;
            return true;
        case 57600:
            //return B57600;
            return true;
        case 115200:
            //return B115200;
            return true;
        case 230400:
            //return B230400;
            return true;
        case 460800:
            //return B460800;
            return true;
        default:
            return false;
    }
}

bool SerialInterfaceSerial::is_open() {
    return get_state() == SERIAL_PORT_STATE::OPEN;
}








