#ifndef __GCODESENDER_H__
#define __GCODESENDER_H__


#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <vector>
//3rd PARTY INCLUDES
#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/serialib-master/lib/serialib.h"


#include "ConfigParser.h"

class GCodeSender
{
	
public:
	
	GCodeSender(std::string _serialport_file);
	GCodeSender(std::string _serialport_file,int _baud);
	~GCodeSender();
	//THREAD ZUM SENDEN


	void configure_marlin();  //SENDS INIT GACODE TO MARLIN TO CONFIGURE TO THE ATC SYSTEM; ABSOLUTE MODE;,.
	//FINALLY WRITE MOTOR HOME/ MOVE TO / SET LIGHT ((I2C))
    void disable_motors();
	bool is_target_position_reached();
	void move_to_postion_mm_absolute(int _x, int _y, bool _blocking);
	void move_to_postion_mm_absolute(int _x, int _y); //JUST FOR CONVENIENCE 	CALLS => void move_to_postion_mm_absolute(int _x, int _y, bool _blocking);
	void home_sync();
	void set_speed_preset(int _feedrate);

	bool setTurnStateLight(int _state);
	bool setServo(int _index, int _pos);
	
	
	
private:
	serialib*  port = nullptr;
	const int MARLIN_SERIAL_BAUD_RATE = 115200;
	const unsigned int SERIAL_READ_DEFAULT_TIMEOUT = 500; //WAIT FOR 2000 aMS FOR DATA READ -1 IS INFITIE TIMEOUT
	bool init_serial_port(std::string _serial_port_file, int _baud_rate); //INIT (OR REINIT) THE SERIAL PORT AND OPENS IT
	bool close_serial_port(); //CLOSES THE SERIAL PORT
	bool check_baud_rate(int _baudrate_to_check); //CHECKS A GIVEN BAUDRATE TO A STANDART VALID ONE
	

	int current_pos_x;
	int current_pos_y;
	//---------- MARLIN GCODE SENDER FUNCTIONS ---------//a

	bool write_gcode(std::string _gcode_line, bool _ack_check); //WRITE GCODE AND WAIT FOR ACK X TIMES ; _ack_check = true waits for the ack repsonse from the board
	bool write_gcode(std::string _gcode_line);
    bool wait_for_ack(); //WAITS FOR A OK OR UNKNOWN KOMMENT ACK
	void dummy_read();
	std::string read_string_from_serial();
	//TODO PREAMBE GCODE => ABS POS
	//TODO WRITE SEND CODE
	//WRITE SEND CODE WAIT FOR ACK (TIME)
	
	
	
	

};

#endif


