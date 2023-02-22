#ifndef __GCODESENDER_H__
#define __GCODESENDER_H__


#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <vector>
#include <filesystem>
#include <cstddef>         // std::size_t
//3rd PARTY INCLUDES
#include "SHARED/loguru-master/loguru.hpp"




#include "ConfigParser.h"
#include "SerialInterface.h"

class GCodeSender
{
	
public:

	GCodeSender(const std::string _serialport_file, const int _baud);
	~GCodeSender();
	//THREAD ZUM SENDEN


	void configure_marlin();  //SENDS INIT GACODE TO MARLIN TO CONFIGURE TO THE ATC SYSTEM; ABSOLUTE MODE;,.
	//FINALLY WRITE MOTOR HOME/ MOVE TO / SET LIGHT ((I2C))
    void disable_motors();
	bool is_target_position_reached();
	void move_to_postion_mm_absolute(const int _x, const int _y,const bool _blocking);
	void move_to_postion_mm_absolute(const int _x, const int _y); //JUST FOR CONVENIENCE 	CALLS => void move_to_postion_mm_absolute(int _x, int _y, bool _blocking);
	void reset_eeprom();//RESETS THE INTERNAL EEPROM TO FIRMWARE DEFAULTS //IS A BUGFIX OF UNCONTROLLED BEHAVIOR OG THE MARLIN CONTROLLER
	void home_sync();
	void set_speed_preset(const int _feedrate);


	bool setServo(const int _index, const int _pos);
	bool setFan(const int _index,const int _speed);
	bool set_steps_per_mm(const int _x,const int _y);

	bool set_led(const int _r, const int _g, const int _b, const int _intensity); //SET THE RGW STRIP COLOR M150 OPTION NEEDED
    bool set_led(const int _hsv);

	bool write_gcode(const std::string _gcode);
	bool write_gcode(const std::string _gcode, const bool _blocking);
	bool wait_for_ack();
private:

	const int MARLIN_SERIAL_BAUD_RATE = 9600;
	int current_pos_x;
	int current_pos_y;

	SerialInterface* serialport = nullptr;
	
	
	
	

};

#endif


