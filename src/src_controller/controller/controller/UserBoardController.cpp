//
// Created by prodevmo on 28.03.21.
//

#include "UserBoardController.h"

UserBoardController::UserBoardController(std::string _serialport_file, int _baud) {
    LOG_SCOPE_F(INFO, "UserBoardController::UserBoardController");
    if (_serialport_file.empty()) {
        LOG_F(ERROR, "serial port _serialport_file is empty %s", _serialport_file.c_str());
        return;
    }
//OPEN SERIAL PORT
    if (port == nullptr) {
        port = new serialib();
        init_serial_port(_serialport_file, _baud);
    }
}


UserBoardController::UserBoardController(std::string _serialport_file) {
    LOG_SCOPE_F(INFO, "UserBoardController::UserBoardController");
    if (_serialport_file.empty()) {

        LOG_F(ERROR, "serial port _serialport_file is empty");
        return;
    }

    if (port == nullptr) {
        port = new serialib();
        init_serial_port(_serialport_file, UBC_SERIAL_BAUD_RATE);
    }
}

bool UserBoardController::check_baud_rate(int _baudrate_to_check) {
#ifdef __MACH__
    return true;
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
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            return 0;
    }
#endif

}

bool UserBoardController::close_serial_port() {
    if (port == nullptr) {
        LOG_F(ERROR, "UserBoardController::close_serial_port close_serial_port serial port instance is null");
        return false;
    }
    port->closeDevice();
    return true;
}


bool UserBoardController::init_serial_port(std::string _serial_port_file, int _baud_rate) {
    LOG_SCOPE_F(INFO, "UserBoardController::init_serial_port");
    if (port == nullptr) {
        LOG_F(ERROR, "serial port instance is null");
        return false;
    }
    if (_baud_rate <= 0 || !check_baud_rate(_baud_rate)) {
        LOG_F(ERROR, "serial port baudrate is invalid %i", _baud_rate);
        return false;
    }
    //configure serialport though 3rd party lib
    close_serial_port();     //CLOSE PORT IF OPEN
    if (port->openDevice(_serial_port_file.c_str(), _baud_rate) != 1) {
        LOG_F(ERROR, "serial port open failed %s WITH BAUD %i", _serial_port_file.c_str(), _baud_rate);
        return false;
    }

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
    //READ MAY EXISITNG BUFFER => IGNORE STARTUP MESSAGES AFTER RESET
    dummy_read();


    return true;
}


void UserBoardController::dummy_read() {
    char c = 128;
    std::string complete = "";
    int timeout = 0;
    while (c != 0) {
        char resp = port->readChar(&c, SERIAL_READ_DEFAULT_TIMEOUT);
        if (resp == 1) {
            complete += c;
        } else if (resp == -2) //ERROR
        {
            break;
        } else {
            timeout++;
            if (timeout > 3) {
                break;
            }
        }
    }

    std::cout << complete.c_str() << std::endl;
}


UserBoardController::~UserBoardController() {
    if (port != nullptr) {
        port->closeDevice();
        delete port;
    }
}


std::string UserBoardController::read_string_from_serial() {
    int wait_counter = 0;
    std::string complete = "";

    char charr[1024] = {0};

    while (true) {
        //READ CHARS FORM SERIAL
        int chars_read = port->readString(charr, '\n', 1024, SERIAL_READ_DEFAULT_TIMEOUT);
        if (chars_read > 0) {
            for (int i = 0; i < chars_read; i++) {
                complete += charr[i];
            }

        }
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
    //std::cout << complete.c_str() << std::endl;
    return complete;
}

std::string UserBoardController::get_value(std::string data, char separator, int index){
        int found = 0;
        int strIndex[] = {
                0,
                -1
        };
        int maxIndex = data.length() - 1;
        for (int i = 0; i <= maxIndex && found <= index; i++) {
            if (data.at(i) == separator || i == maxIndex) {
                found++;
                strIndex[0] = strIndex[1] + 1;
                strIndex[1] = (i == maxIndex) ? i + 1 : i;
            }
        }
        return found > index ? data.substr(strIndex[0], strIndex[1]) : "";

}

//https://stackoverflow.com/questions/10058606/splitting-a-string-by-a-character/10058756
std::vector<std::string> UserBoardController::split(std::string _input, char _char){
    std::stringstream test;
    test.str(_input); //LOAD STRING

    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(test, segment, _char))
    {
        seglist.push_back(segment);
    }

    return seglist;
}

std::string UserBoardController::send_command_blocking(std::string _cmd) {

    if (port == nullptr) {
        return "";
    }
    if (_cmd.empty()) {
        return "";
    }

    port->flushReceiver();
    //APPEND _ CHARAKTER IF NEEDED ACCORDING THE PROTOCOL
    // version => _version_
    // readnfc => _readnfc_
    char t = _cmd.at(0);
    if (_cmd.at(0) != '_') {
        _cmd =  "_"+_cmd;
        _cmd += "_";
    }
    //APPEND NEW LINE CHARAKTER IF NEEDED
    std::string _cmd_send = _cmd;
    if (_cmd_send.rfind('\n') == std::string::npos) {
        _cmd_send += '\n';
    }
    //WRITE COMMAND TO SERIAL LINE
    port->writeString(_cmd_send.c_str());

    //NOW READ RECEIVE BUFFER

    int wait_counter = 0;

    while (true) {
        std::string resp = read_string_from_serial();


        //CHECK FOR RESPONSE
        if (resp.rfind(_cmd + "res_") != std::string::npos) {
        std::string value = get_value(resp,UBC_CMD_SEPERATOR,3);
        std::vector<std::string> re = split(value,UBC_CMD_SEPERATOR);
        return value;
           //TODO TEST
        } else {
            wait_counter++;
            if (wait_counter > 3) {
                break;
            }
        }
    }

    return "";
}
//HARDWARE_UBC_NFC_READ_RETRY_COUNT



ChessPiece::FIGURE UserBoardController::read_chess_piece_nfc(){

    ChessPiece::FIGURE fig;
    fig.color = ChessPiece::COLOR::COLOR_UNKNOWN;
    fig.type = ChessPiece::TYPE::TYPE_INVALID;
    fig.figure_number = -1;
    fig.unique_id = -1;
    fig.is_empty = true;
    fig.figure_read_failed = true;


    //NOW TRY TO READ THE
    const int retry_count = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_UBC_NFC_READ_RETRY_COUNT) + 1;
    for(int i =0; i <retry_count;i++){
        std::string readres = send_command_blocking(UBC_COMMAND_READNFC);
        if(readres.empty()){
            continue;
        }
        //TODO PARSE RESULT + ERROR CODE
        //0 => TAG
        //1 => ERROR
        //IF COMMAND
        //TODO MODIFY PARSE RESULT
        //MAKE A FIGURE
    }


return fig;
}
