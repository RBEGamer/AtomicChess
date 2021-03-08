#include "GCodeSender.h"


GCodeSender::GCodeSender(std::string _serialport_file, int _baud)
{
	LOG_SCOPE_F(INFO, "GCodeSender::GCodeSender");
	if (_serialport_file.empty())
	{
		
		LOG_F(ERROR, "serial port _serialport_file is empty");
		return;
	}
		
	if (port == nullptr)
	{
		
		port = new serialib();
		init_serial_port(_serialport_file, _baud);
		
		
	}
}


GCodeSender::GCodeSender(std::string _serialport_file)
{
	LOG_SCOPE_F(INFO, "GCodeSender::GCodeSender");
	if (_serialport_file.empty())
	{
		
		LOG_F(ERROR, "serial port _serialport_file is empty");
		return;
	}
		
	if (port == nullptr)
	{
		
		port = new serialib();
		init_serial_port(_serialport_file, MARLIN_SERIAL_BAUD_RATE);
		
		
	}
		
}

bool GCodeSender::check_baud_rate(int _baudrate_to_check) {
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
}

bool GCodeSender::close_serial_port()
{
	if (port == nullptr)
	{
		LOG_F(ERROR, "close_serial_port serial port instance is null");
		return false;
	}
	
		port->closeDevice();
		return true;
}

bool GCodeSender::init_serial_port(std::string _serial_port_file, int _baud_rate)
{	
	LOG_SCOPE_F(INFO, "GCodeSender::init_serial_port");
	if (port == nullptr)
	{
		LOG_F(ERROR, "serial port instance is null");
		return false;
	}
	if (_baud_rate <= 0 || !check_baud_rate(_baud_rate))
	{
		LOG_F(ERROR, "serial port baudrate is invalid");
		return false;
	}
	//configure serialport though 3rd party lib
	close_serial_port();     //CLOSE PORT IF OPEN
	if(port->openDevice(_serial_port_file.c_str(), _baud_rate) != 1) {
		LOG_F(ERROR, "serial port open failed");
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
	
	
	dummy_read();

	
	return true;
}

void GCodeSender::dummy_read()
{
	char c = 128;
	std::string complete = "";
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
	
	std::cout << complete.c_str() << std::endl;
}

GCodeSender::~GCodeSender()
{
	if (port != nullptr)
	{
		port->closeDevice();
		delete port;
	}	
}

std::string GCodeSender::read_string_from_serial()
{
	int wait_counter = 0;
	std::string complete = "";
	
	char charr[1024] = { 0 };
	
	while (true){
		
	
		
		//READ CHARS FORM SERIAL
		int chars_read = port->readString(charr, '\n', 1024, SERIAL_READ_DEFAULT_TIMEOUT);
		if (chars_read > 0) { 
			for (int i = 0; i < chars_read; i++)
			{
				complete += charr[i];
			}
					
		}
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
	//std::cout << complete.c_str() << std::endl;
	return complete;
}
	
bool GCodeSender::wait_for_ack() {
	
	
	int wait_counter = 0;
	
	while (true) {
		std::string resp = read_string_from_serial();
		
		
		
		if (resp.rfind("ok") != std::string::npos)
		{
			break;
		}else if(resp.rfind("echo:Unknown") != std::string::npos) {
			break;
		}else if(resp.rfind("Error:") != std::string::npos) {
			break;			
		}else if (resp.rfind("echo:busy: processing") != std::string::npos) {
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


	//FLUSH INPUT BUFFER
	port->flushReceiver();
	//APPEND NEW LINE CHARAKTER IF NEEDED
	if (_gcode_line.rfind('\n') == std::string::npos)
	{
		_gcode_line += '\n';
	}
	//WRITE COMMAND TO SERIAL LINE
	port->writeString(_gcode_line.c_str());
	
	
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
	write_gcode("G0 F" + std::to_string(_feedrate));
}

void GCodeSender::configure_marlin() {
	write_gcode("G21");      //SET UNIT TO MM
    write_gcode("G90");      //ABSOLUTE MODE
	disable_motors();		//DISABLE MOTORD DIRECTLY
}

void GCodeSender::home_sync() {
	write_gcode("G28 X Y");     //HOME AXIS
}

bool GCodeSender::setServo(int _index, int _pos) {
	return write_gcode("M280 P" + std::to_string(_index) + " S" + std::to_string(_pos));     //MOVE SERVO
}

void GCodeSender::disable_motors() {
	write_gcode("M84");     //DISBBLE MOTOR
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
		write_gcode("M400");      //HOME AXIS
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