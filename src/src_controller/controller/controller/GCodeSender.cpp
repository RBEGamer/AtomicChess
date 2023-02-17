#include "GCodeSender.h"


GCodeSender::GCodeSender(std::string _serialport_file, int _baud)
{
	LOG_SCOPE_F(INFO, "GCodeSender::GCodeSender");
	if (_serialport_file.empty())
	{
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

GCodeSender::GCodeSender(std::string _serialport_file)
{
	LOG_SCOPE_F(INFO, "GCodeSender::GCodeSender");
	if (_serialport_file.empty())
	{
		LOG_F(ERROR, "serial port _serialport_file is empty");
		return;
	}
#ifndef USE_ALTERNATIVE_SERIAL_LIB
    if (port == nullptr)
    {
        port = new serialib();
    }
#endif
		init_serial_port(_serialport_file, MARLIN_SERIAL_BAUD_RATE);
}

GCodeSender::~GCodeSender()
{
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

bool GCodeSender::close_serial_port()
{
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

bool GCodeSender::init_serial_port(std::string _serial_port_file, int _baud_rate)
{	
	LOG_SCOPE_F(INFO, "GCodeSender::init_serial_port");
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
	//READ MAY EXISTING BUFFER => IGNORING MARLIN CONNECTION INFO LIKE Printer Online,...
	dummy_read();
	return true;
}

void GCodeSender::dummy_read()
{
	LOG_F(INFO, "dummy_read() ENTER");
	char c = 128;
	std::string complete = "";
	int timeout = 0;
	while (c != 0)
	{
#ifdef USE_ALTERNATIVE_SERIAL_LIB
        std::string readData;

        port->Read(readData);//port->readChar(&c, SERIAL_READ_DEFAULT_TIMEOUT);
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
	LOG_F(INFO, "dummy_read() EXIT");
}

std::string GCodeSender::read_string_from_serial()
{
	int wait_counter = 0;
	std::string complete = "";
	
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
	std::cout << "GCODE-RS: -" << complete.c_str() << "-" <<std::endl;
	return complete;
}
	
bool GCodeSender::wait_for_ack() {
	int wait_counter = 0;
    std::string resp;
	while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
		resp += read_string_from_serial();

		if (resp.rfind("ok") != std::string::npos)
		{
			break;
		}else if(resp.rfind("echo:Unknown") != std::string::npos) {
			break;
		}else if(resp.rfind("Error:") != std::string::npos) {
			break;			
		//}else if (resp.rfind("echo:busy: processing") != std::string::npos) {
		}else if (resp.rfind("echo:busy:") != std::string::npos) {
			wait_counter = 0;
			std::cout << "wait_for_ack: busy_processing" << std::endl;
		}else {
			wait_counter++;
			std::cout << "wait_for_ack" << resp << std::endl;
			if (wait_counter > 3)
			{
				break;
			}
		}	
	}
	return true;
}

bool GCodeSender::write_gcode(std::string _gcode_line, bool _ack_check) {
	//check port state
	if(port == nullptr) {
		return false;
	}
	//check line
	if(_gcode_line.empty()) {
		return false;
	}
	std::cout << "write_gcode: -" << _gcode_line.c_str() << "-" << std::endl;
	//APPEND NEW LINE CHARAKTER IF NEEDED
	if (_gcode_line.rfind('\n') == std::string::npos)
	{
		_gcode_line += '\n';
	}
	//WRITE COMMAND TO SERIAL LINE
#ifdef USE_ALTERNATIVE_SERIAL_LIB
    port->Write(_gcode_line.c_str());
#else
    port->flushReceiver();
    const char pwriteret = port->writeString(_gcode_line.c_str());
#endif

	if (!_ack_check)
	{
		return true;
	}
	//WAIT FOR ACK OK OR ERROR MESSAGE FROM BOARD
	if(!wait_for_ack())
	{
		LOG_F(ERROR, _gcode_line.c_str(), " ACK FAILED");
		return false;
	}
	return true;
}

void GCodeSender::set_speed_preset(int _feedrate) {
	if (_feedrate < 0){_feedrate = 0; }
	write_gcode("M203 X" + std::to_string(_feedrate)+" Y"+ std::to_string(_feedrate));
    write_gcode("G0 F" + std::to_string(_feedrate));
}

void GCodeSender::configure_marlin() {
	write_gcode("G21");      //SET UNIT TO MM
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	write_gcode("G90");      //ABSOLUTE MODE
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	disable_motors();		//DISABLE MOTORD DIRECTLY
}

void GCodeSender::home_sync() {
	write_gcode("G28 X Y");     //HOME AXIS
}

bool GCodeSender::setServo(int _index, int _pos) {
	const bool r = write_gcode("M280 P" + std::to_string(_index) + " S" + std::to_string(_pos), false);     //MOVE SERVO
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return r;
}

bool GCodeSender::setFan(int _index, int _speed){
    //const bool r = write_gcode("M106 P" + std::to_string(_index) + " S" + std::to_string(_speed), false);     //SET FAN STATE
	//return r;
	return true;
}

void GCodeSender::disable_motors() {
	write_gcode("M84");     //DISBBLE MOTOR
}

void GCodeSender::reset_eeprom(){
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
   // write_gcode("M502"); // RESET DEFAULTS
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //write_gcode("M500"); // STORE NEW VALUES
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

bool GCodeSender::set_steps_per_mm(int _x, int _y){
    //M92 X100 Y100
    if(_x < 0){
        _x = 0;
    }
    if(_y < 0){
        _y = 0;
    }
    //MARLIN M92 COMMAND TO SET STEPS PER MM DYNAMICLY
    return write_gcode("M92 X" + std::to_string(_x) + " Y" + std::to_string(_y));     //MOVE SERVO
}

bool GCodeSender::write_gcode(std::string _gcode_line)
{
	return write_gcode(_gcode_line, true);
}

void GCodeSender::move_to_postion_mm_absolute(int _x, int _y) {
	move_to_postion_mm_absolute(_x, _y, true);
}
	
void GCodeSender::move_to_postion_mm_absolute(int _x, int _y, bool _blocking) {
	write_gcode("G0 X" + std::to_string(_x) + " Y" + std::to_string(_y));     //HOME AXIS
	current_pos_x = _x;
	current_pos_y = _y;

	if (_blocking) {
		write_gcode("M400");      //WAIT FOR PREV MOVE COMMAND FINISHED
		//is_target_position_reached();
	}
	
	disable_motors();
}

bool GCodeSender::is_target_position_reached() {
	if (port == nullptr) {
		return false;
	}
	//IN THIS CASE WE DONT READ THE CURRENT POS (gcode M114) ONLY WAITING FOR FINISHING THE MOVE
	//MORE IS NOT NEEDED FOR THIS APPLICATION
	return write_gcode("M400", true); 		
}

bool GCodeSender::set_led(int _hsv) {
    int r,g,b,tmp = 0;
    //CONVERT HSV TO RGB
    if (_hsv < 85) {
        r = _hsv * 3;
        g = 255 - _hsv * 3;
        b = 0;
    } else if (_hsv < 170) {
        _hsv -= 85;
        r = _hsv * 3;
        g = 255 - _hsv * 3;
        b = 0;
    } else {
        _hsv -= 170;
        b = _hsv * 3;
        g = 255 - _hsv * 3;
        r= 0;
    }
    //SWITCH G AND B COMPONENT DEPENDING ON THE USED WS2812 STRIP
    if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RGBW_STRIP_SWITCH_GB)){
        tmp = g;
        g = b;
        b = tmp;
    }
    return GCodeSender::set_led(r,g,b, 255);
}

bool GCodeSender::set_led(int _r, int _g, int _b, int _intensity){
//M150 P255 U255 R255 B255
    //p<intensity> u<green_intensity> r<red_intensity> b<blue_intensity>
    if(_r > 255){_r = 255;}else if(_r < 0){_r = 0;}
    if(_g > 255){_g = 255;}else if(_g < 0){_g = 0;}
    if(_b > 255){_b = 255;}else if(_b < 0){_b = 0;}
    if(_intensity > 255){_intensity = 255;}else if(_intensity < 0){_intensity = 0;}
    // SET LED BUT DONT WAIT FOR ACK => IF COMMAND ISNT IMPLEMENTED
    return write_gcode("M150 P" + std::to_string(_intensity) + " U" + std::to_string(_g)+ " R" + std::to_string(_r)+ " B" + std::to_string(_b), false);
}


#ifdef USE_ALTERNATIVE_SERIAL_LIB
mn::CppLinuxSerial::BaudRate GCodeSender::convert_baud_rate(const int _baudrate_to_check) {
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

bool GCodeSender::check_baud_rate(int _baudrate_to_check) {
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