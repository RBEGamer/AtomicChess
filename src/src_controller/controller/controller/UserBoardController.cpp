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
#ifndef USE_ALTERNATIVE_SERIAL_LIB
    if (port == nullptr)
    {
        port = new serialib();
    }
#endif
    init_serial_port(_serialport_file, _baud);
}

UserBoardController::UserBoardController(std::string _serialport_file) {
    LOG_SCOPE_F(INFO, "UserBoardController::UserBoardController");
    if (_serialport_file.empty()) {
        LOG_F(ERROR, "serial port _serialport_file is empty");
        return;
    }
    //OPEN SERIAL PORT
#ifndef USE_ALTERNATIVE_SERIAL_LIB
    if (port == nullptr)
    {
        port = new serialib();
    }
#endif
    init_serial_port(_serialport_file, UBC_SERIAL_BAUD_RATE);
}



bool UserBoardController::close_serial_port() {
    if (port != nullptr)
    {
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        if(port->GetState() == mn::CppLinuxSerial::State::OPEN){
            port->Close();
        }
#else
        port->closeDevice();
#endif
    }
    return true;
}


bool UserBoardController::init_serial_port(std::string _serial_port_file, int _baud_rate) {
    LOG_SCOPE_F(INFO, "UserBoardController::init_serial_port");
#ifndef USE_ALTERNATIVE_SERIAL_LIB
    if (port == nullptr)
	{
		LOG_F(ERROR, "serial port instance is null");
		return false;
	}
#endif
    if (_baud_rate <= 0 || !check_baud_rate(_baud_rate))
    {
        LOG_F(ERROR, "serial port baudrate is invalid %i", _baud_rate);
        return false;
    }
    //configure serialport though 3rd party lib
    close_serial_port();     //CLOSE PORT IF OPEN

    if(std::filesystem::is_symlink(_serial_port_file)){
        LOG_F(INFO, "filesystem::is_symlink(_port) %s", _serial_port_file.c_str());
        //GET PREFIX EG /dev/
        std::string prefix = "";
        const size_t slash_index = _serial_port_file.find_last_of("/\\");
        if(slash_index){
            prefix = _serial_port_file.substr(0,slash_index) + '/';
        }
        //ADD SYMLINK DESTINATION ttyTEST =>prefix + ttyUSB0 = /dev/ttyUSB0
        _serial_port_file = prefix + std::filesystem::read_symlink(_serial_port_file).string();
        LOG_F(INFO, "filesystem::read_symlink(_port) %s", _serial_port_file.c_str());
    }

    //TRY TO CONNECT
    bool con_succ = false;
    for(int i = 0; i < 5; i++){

#ifdef USE_ALTERNATIVE_SERIAL_LIB
        const mn::CppLinuxSerial::BaudRate baud = convert_baud_rate(_baud_rate);
        //IF PORT IS ALREADY CREATED => ONLY SET NEW VALUES
        if(port){
            port->Close();
            port->SetDevice(_serial_port_file.c_str());
            port->SetNumDataBits(mn::CppLinuxSerial::NumDataBits::EIGHT);
            port->SetNumStopBits(mn::CppLinuxSerial::NumStopBits::ONE);
            port->SetParity(mn::CppLinuxSerial::Parity::NONE);
            port->SetBaudRate(baud);
            port->SetTimeout(SERIAL_READ_DEFAULT_TIMEOUT);

        }else{
            port = new mn::CppLinuxSerial::SerialPort(_serial_port_file.c_str(), baud, mn::CppLinuxSerial::NumDataBits::EIGHT, mn::CppLinuxSerial::Parity::NONE, mn::CppLinuxSerial::NumStopBits::ONE, mn::CppLinuxSerial::HardwareFlowControl::ON, mn::CppLinuxSerial::SoftwareFlowControl::ON);
            port->SetTimeout(SERIAL_READ_DEFAULT_TIMEOUT);
        }

        //TRY TO OPEN
        port->Open();
        //CHECK STATE
        if(port->GetState() == mn::CppLinuxSerial::State::OPEN){
            con_succ = true;
            break;
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
#else
        if(port->openDevice(_serial_port_file.c_str(), _baud_rate) != 1) {
			LOG_F(INFO, "(loop %i ) serial port open failed %s WITH BAUD %i", i, _serial_port_file.c_str(),_baud_rate);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}else{
			break;
		}
#endif
    }

    if(!con_succ){
        LOG_F(ERROR, "serial port open failed %s WITH BAUD %i",_serial_port_file.c_str(),_baud_rate);
        return false;
    }

#ifdef USE_ALTERNATIVE_SERIAL_LIB
#else
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
#endif

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //READ MAY EXISTING BUFFER
    dummy_read();
    return true;
}


void UserBoardController::dummy_read()
    {
        LOG_F(INFO, "UserBoardController::dummy_read() ENTER");
        char c = 128;
        std::string complete = "";
        int timeout = 0;
        while (c != 0)
        {
#ifdef USE_ALTERNATIVE_SERIAL_LIB
            std::string readData;
            port->Read(readData);
            if(readData.empty()){
                break;
            }else{
                timeout++;
                if (timeout > 3)
                {
                    break;
                }
            }
#else
        const char resp = port->readChar(&c, SERIAL_READ_DEFAULT_TIMEOUT);
		if (resp == 1)
		{
			complete += c;
		}
		else if (resp == -2) //ERROR
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
#endif
        }
        std::cout << complete.c_str() << std::endl;
        LOG_F(INFO, "UserBoardController::dummy_read() EXIT");
    }

UserBoardController::~UserBoardController() {
        if (port != nullptr)
        {
#ifdef USE_ALTERNATIVE_SERIAL_LIB
            port->Close();
#else
            port->closeDevice();
#endif
            delete port;
        }
}

std::string UserBoardController::read_string_from_serial() {
    int wait_counter = 0;
    std::string complete;
    char charr[1024] = { 0 };
    while (true){
        //READ CHARS FORM SERIAL
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        port->Read(complete);
#else
        const int chars_read = port->readString(charr, '\n', 1024, SERIAL_READ_DEFAULT_TIMEOUT);
		if (chars_read > 0) {
			for (int i = 0; i < chars_read; i++)
			{
				complete += charr[i];
			}

		}
#endif
        //CHECK READ RESULT
        if (!complete.empty())
        {
            break;
        }
        else
        {
            wait_counter++;
        }
        if (wait_counter > 10)
        {
            break;
        }
    }
    std::cout << "UserBoardController-RS: -" << complete.c_str() << "-" <<std::endl;
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

std::string UserBoardController::send_command(std::string _cmd, bool _blocking) {

    if (port == nullptr) {
        return "";
    }
    if (_cmd.empty()) {
        return "";
    }

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
#ifdef USE_ALTERNATIVE_SERIAL_LIB
    port->Write(_cmd_send.c_str());
#else
    port->flushReceiver();
    const char pwriteret = port->writeString(_cmd_send.c_str());
    if(!_blocking){
        return "_state_res_ok_";
    }
#endif

    //NOW READ RECEIVE BUFFER
    int wait_counter = 0;
    while (true) {
        const std::string resp = read_string_from_serial();
        //CHECK FOR RESPONSE
        if (resp.rfind(_cmd + "res_") != std::string::npos) {
            return resp;
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


void UserBoardController::set_led(int _color){
    //const std::string readres = send_command("_"+ UBC_COMMAND_LED + "_" + std::to_string(_color) + "_", false);

    for(int i =0; i <GENERAL_UBC_COMMAND_RESPONSE_RETRY;i++){
        const std::string readres = send_command("_"+ UBC_COMMAND_LED + "_" + std::to_string(_color) + "_", false);
        if(readres.empty()){
            continue;
        }
        //SPLIT REPSONSE
        const std::vector<std::string> re = split(readres,UBC_CMD_SEPERATOR);
        //CHECK SPLIT LENGTH
        if(re.size() != 4){
            break;
        }
        //READ RESULT
        const std::string& errorcode = re.at(3);
        //READ STATUS CODE IF READOUT IS VALID / NO TAG PRESENT OR READ FAILED
        if(errorcode == "ok"){
            break;
        }else{
            LOG_F(WARNING, "UBC_COMMAND_LED ERRORCODE %s", errorcode.c_str());
        }
    }

}

void UserBoardController::set_servo(int _pos){
    for(int i =0; i <GENERAL_UBC_COMMAND_RESPONSE_RETRY;i++){
        const std::string readres = send_command("_" + UBC_COMMAND_SERVO + "_" + std::to_string(_pos) + "_", false);
        if(readres.empty()){
            continue;
        }
        //SPLIT REPSONSE
        const std::vector<std::string> re = split(readres,UBC_CMD_SEPERATOR);
        //CHECK SPLIT LENGTH
        if(re.size() != 4){
            continue;
        }
        //READ RESULT
        const std::string& errorcode = re.at(3);
        //READ STATUS CODE IF READOUT IS VALID / NO TAG PRESENT OR READ FAILED
        if(errorcode == "ok"){
            break;
        }else{
            LOG_F(WARNING, "UBC_COMMAND_LED ERRORCODE %s", errorcode.c_str());
        }
    }
}


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
        std::string readres = send_command(UBC_COMMAND_READNFC, true);
        if(readres.empty()){
            continue;
        }
        //SPLIT REPSONSE
        const std::vector<std::string> re = split(readres,UBC_CMD_SEPERATOR);
        //CHECK SPLIT LENGTH
        if(re.size() != 6){
            continue;
        }
        //READ RESULT
        const std::string& figure = re.at(3);
        const std::string& errorcode = re.at(4);
        //READ STATUS CODE IF READOUT IS VALID / NO TAG PRESENT OR READ FAILED
        if(errorcode == "ok"){
            if(figure.empty()){
                break;
            }
            const char figure_charakter = figure.at(0);
            //const char figure_id = figure_at(1);
            fig = ChessPiece::getFigureByCharakter(figure_charakter);
            break;
        }else if(errorcode == "notagpresent"){
            break;
        }else{
            LOG_F(WARNING, "NFC READ RESPONSE ERRORCODE %s", errorcode.c_str());
        }
    }
return fig;
}


#ifdef USE_ALTERNATIVE_SERIAL_LIB
mn::CppLinuxSerial::BaudRate UserBoardController::convert_baud_rate(const int _baudrate_to_check) {
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
        default:
            return 0;
    }
#endif
}