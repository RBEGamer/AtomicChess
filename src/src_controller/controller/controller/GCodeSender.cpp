#include "GCodeSender.h"

GCodeSender::GCodeSender(std::string _serialport_file, int _baud)
{
	LOG_SCOPE_F(INFO, "GCodeSender::GCodeSender");
	// OPEN UP SERIAL PORT
	if (serialport)
	{
		delete serialport;
	}
	serialport = new SerialInterface(_serialport_file, _baud);
    // OPEN PORT
    if(!serialport->init()){
        LOG_F(ERROR, "cant create init_serial_port SerialInterface");
        return;
    }
	// READ MAY EXISTING BUFFER => IGNORING MARLIN CONNECTION INFO LIKE Printer Online,...
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	// READ BUFFER EMPTY
	if (serialport->is_open())
	{
		serialport->dummy_read();
	}
}

GCodeSender::~GCodeSender()
{
	if (serialport)
	{
		delete serialport;
	}
}

bool GCodeSender::wait_for_ack()
{
	// check port state
	if (serialport == nullptr)
	{
		LOG_F(ERROR, "serialpor is null");
		return false;
	}

	int wait_counter = 0;
	std::string resp;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		resp += serialport->read_line();

		if (resp.rfind("ok") != std::string::npos)
		{
			break;
		}
		else if (resp.rfind("echo:Unknown") != std::string::npos)
		{
			break;
		}
		else if (resp.rfind("Error:") != std::string::npos)
		{
			break;
			//}else if (resp.rfind("echo:busy: processing") != std::string::npos) {
		}
		else if (resp.rfind("echo:busy:") != std::string::npos)
		{
			wait_counter = 0;
			std::cout << "wait_for_ack: busy_processing" << std::endl;
		}
		else
		{
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

bool GCodeSender::write_gcode(std::string _gcode_line, bool _ack_check)
{
	// check port state
	if (serialport == nullptr)
	{
		LOG_F(ERROR, "serialpor is null");
		return false;
	}
	// check line
	if (_gcode_line.empty())
	{
		return false;
	}
	std::cout << "write_gcode: -" << _gcode_line.c_str() << "-" << std::endl;
	// APPEND NEW LINE CHARAKTER IF NEEDED
	if (_gcode_line.rfind('\n') == std::string::npos)
	{
		_gcode_line += '\n';
	}
	// SEND OVER PORT
	serialport->write_string(_gcode_line);

	if (!_ack_check)
	{
		return true;
	}
	// WAIT FOR ACK OK OR ERROR MESSAGE FROM BOARD
	if (!wait_for_ack())
	{
		LOG_F(ERROR, _gcode_line.c_str(), " ACK FAILED");
		return false;
	}
	return true;
}

void GCodeSender::set_speed_preset(int _feedrate)
{
	if (_feedrate < 0)
	{
		_feedrate = 0;
	}
	write_gcode("M203 X" + std::to_string(_feedrate) + " Y" + std::to_string(_feedrate));
	write_gcode("G0 F" + std::to_string(_feedrate));
}

void GCodeSender::configure_marlin()
{
	write_gcode("G21"); // SET UNIT TO MM
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	write_gcode("G90"); // ABSOLUTE MODE
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	disable_motors(); // DISABLE MOTORD DIRECTLY
}

void GCodeSender::home_sync()
{
	write_gcode("G28 X Y"); // HOME AXIS
}

bool GCodeSender::setServo(int _index, int _pos)
{
	return write_gcode("M280 P" + std::to_string(_index) + " S" + std::to_string(_pos), false); // MOVE SERVO
}

bool GCodeSender::setFan(int _index, int _speed)
{
	return write_gcode("M106 P" + std::to_string(_index) + " S" + std::to_string(_speed), false); // SET FAN STATE
}

void GCodeSender::disable_motors()
{
	write_gcode("M84"); // DISBBLE MOTOR
}

void GCodeSender::reset_eeprom()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	write_gcode("M502"); // RESET DEFAULTS
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	write_gcode("M500"); // STORE NEW VALUES
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

bool GCodeSender::set_steps_per_mm(int _x, int _y)
{
	// M92 X100 Y100
	if (_x < 0)
	{
		_x = 0;
	}
	if (_y < 0)
	{
		_y = 0;
	}
	// MARLIN M92 COMMAND TO SET STEPS PER MM DYNAMICLY
	return write_gcode("M92 X" + std::to_string(_x) + " Y" + std::to_string(_y)); // MOVE SERVO
}

bool GCodeSender::write_gcode(std::string _gcode_line)
{
	return write_gcode(_gcode_line, true);
}

void GCodeSender::move_to_postion_mm_absolute(int _x, int _y)
{
	move_to_postion_mm_absolute(_x, _y, true);
}

void GCodeSender::move_to_postion_mm_absolute(int _x, int _y, bool _blocking)
{
	write_gcode("G0 X" + std::to_string(_x) + " Y" + std::to_string(_y)); // HOME AXIS
	current_pos_x = _x;
	current_pos_y = _y;
	// WAIT UNTIL M400 RETURNS ok => POSITION REACHED
	if (_blocking)
	{
		is_target_position_reached();
	}
	disable_motors();
}

bool GCodeSender::is_target_position_reached()
{
	// IN THIS CASE WE DONT READ THE CURRENT POS (gcode M114) ONLY WAITING FOR FINISHING THE MOVE
	// MORE IS NOT NEEDED FOR THIS APPLICATION
	return write_gcode("M400", true);
}

bool GCodeSender::set_led(int _hsv)
{
	int r, g, b, tmp = 0;
	// CONVERT HSV TO RGB
	if (_hsv < 85)
	{
		r = _hsv * 3;
		g = 255 - _hsv * 3;
		b = 0;
	}
	else if (_hsv < 170)
	{
		_hsv -= 85;
		r = _hsv * 3;
		g = 255 - _hsv * 3;
		b = 0;
	}
	else
	{
		_hsv -= 170;
		b = _hsv * 3;
		g = 255 - _hsv * 3;
		r = 0;
	}
	// SWITCH G AND B COMPONENT DEPENDING ON THE USED WS2812 STRIP
	if (ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RGBW_STRIP_SWITCH_GB))
	{
		tmp = g;
		g = b;
		b = tmp;
	}
	return GCodeSender::set_led(r, g, b, 255);
}

bool GCodeSender::set_led(int _r, int _g, int _b, int _intensity)
{
	// M150 P255 U255 R255 B255
	// p<intensity> u<green_intensity> r<red_intensity> b<blue_intensity>
	if (_r > 255)
	{
		_r = 255;
	}
	else if (_r < 0)
	{
		_r = 0;
	}
	if (_g > 255)
	{
		_g = 255;
	}
	else if (_g < 0)
	{
		_g = 0;
	}
	if (_b > 255)
	{
		_b = 255;
	}
	else if (_b < 0)
	{
		_b = 0;
	}
	if (_intensity > 255)
	{
		_intensity = 255;
	}
	else if (_intensity < 0)
	{
		_intensity = 0;
	}
	// SET LED BUT DONT WAIT FOR ACK => IF COMMAND ISNT IMPLEMENTED
	return write_gcode("M150 P" + std::to_string(_intensity) + " U" + std::to_string(_g) + " R" + std::to_string(_r) + " B" + std::to_string(_b), false);
}