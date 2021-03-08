#include "HardwareInterface.h"

HardwareInterface* HardwareInterface::instance;
std::mutex HardwareInterface::acces_lock_mutex;

HardwareInterface* HardwareInterface::getInstance()
{
	if (instance == nullptr)
	{
		instance = new HardwareInterface();
	}
	return instance;
}


HardwareInterface::HardwareInterface()
{
	//FIRST CHECH WHICH HARDWARE REVISION WE ARE RUNNING ON
	hwrev = HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_UNKNOWN;
	
	std::string hwrevstr = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::HWARDWARE_REVISION);
	if (hwrevstr == "DK")
	{ 
		hwrev = HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK;
	}
	else if (hwrevstr == "PROD")
	{ 
		hwrev = HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD;
	}
	else
	{
		init_complete = false;
		return;
	}
	LOG_F(INFO, "HardwareInterface::HardwareInterface got hwrev ", hwrevstr.c_str());
	//AFTER DETERM OF THE HW REV THE SPECIFIC HARDWARE CAN BE SETUP
	init_complete = init_hardware(hwrev);
}


bool HardwareInterface::init_hardware(HardwareInterface::HI_HARDWARE_REVISION _hwrev)
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		//MOTOR SETUP
		if(tmc5160_x == nullptr)
		{
			tmc5160_x = new TMC5160(TMC5160::MOTOR_ID::MOTOR_0);
		}
		if (tmc5160_y == nullptr)
		{
			tmc5160_y = new TMC5160(TMC5160::MOTOR_ID::MOTOR_1);
		}
		//LOAD DEFAULT SETINGS
		tmc5160_x->default_settings();
		tmc5160_y->default_settings();
		///OVERRIDE STEPS PER MM IF CONFIG EXISTS
		int spm = 0;
		ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_STEPS_PER_MM, spm);
		if (spm > 0)
		{
			tmc5160_x->steps_per_mm(spm);
			tmc5160_y->steps_per_mm(spm);

			LOG_F(INFO, "HardwareInterface::init_hardware set steps per mm setting to: %i" ,spm);
		}	
		tmc5160_x->disable_motor();
		tmc5160_y->disable_motor();
		//IO CONTROLLER SETUP
		if(iocontroller == nullptr)
		{
			iocontroller = new IOController();
			
			if (!iocontroller->isInitialized())
			{
				LOG_F(ERROR, "HardwareInterface::init_hardware iocontroller->isInitialized()");
			}
			//DISBALE COILS
            setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
			setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
			///SET STATUS LED
			iocontroller->setStatusLed(IOController::STAUS_LED_A, true);
			///SET TURN STATE LIGHT
			iocontroller->setTurnStateLight(IOController::TSL_IDLE);
			//OVERRIDE POLARITY SETTING OF TH COILS
			int ivcoils = 0;
			ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_INVERT_COILS, ivcoils);
			if (ivcoils > 0)
			{
				iocontroller->invertCoilPolarity(IOController::COIL::COIL_A, true);
				iocontroller->invertCoilPolarity(IOController::COIL::COIL_B, true);
			}
		}
		
		
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		if (gcode_interface == nullptr) {
			int baud = 115200;
			ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_BAUD, baud);
			gcode_interface = new GCodeSender(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_PORT), baud);
			gcode_interface->configure_marlin();  //CONFIGURE MARLIN FOR STARTUP
			setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
			setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
		}

	}
	else
	{
		return false;
	}
	
	return true;
}


HardwareInterface::~HardwareInterface()
{
	if (tmc5160_x != nullptr)
	{
		delete tmc5160_x;
	}
	if (tmc5160_y != nullptr)
	{
		delete tmc5160_y;
	}
	if (iocontroller != nullptr)
	{
		delete iocontroller;
	}
	
	if (gcode_interface != nullptr)
	{
		delete gcode_interface;
	}
	
}

//LOAD CONFGI
bool HardwareInterface::check_hw_init_complete()
{
	return init_complete;
}

bool HardwareInterface::is_production_hardware()
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool HardwareInterface::setTurnStateLight(HardwareInterface::HI_TURN_STATE_LIGHT _state)
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (iocontroller != nullptr)
		{
			iocontroller->setTurnStateLight(static_cast<IOController::TURN_STATE_LIGHT>(_state));
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		//TODO
	}
	return true;
}

bool HardwareInterface::setCoilState(HI_COIL _coil, bool _state)
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (iocontroller != nullptr)
		{
			iocontroller->setCoilState(static_cast<IOController::COIL>(_coil), _state);
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		//MORE LOGIC NEEDED HERE
		//IT DEPENDS ON THE ONFIGURATION OF MARLIN WHICH INDEX THE SERVOS HAVE TO WE NEED TO LOAD IT FROM THE CONFIG FIRST
		if(_coil == HardwareInterface::HI_COIL::HI_COIL_A)
		{
			if (_state)
			{
				gcode_interface->setServo(ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX), ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS));	
			}
			else
			{
				gcode_interface->setServo(ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX), ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS));	
			}
		}else if(_coil == HardwareInterface::HI_COIL::HI_COIL_B) {
			if (_state)
			{
				gcode_interface->setServo(ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_B_INDEX), ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS));	
			}
			else
			{
				gcode_interface->setServo(ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_B_INDEX), ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS));	
			}
		}
	}
	return true;
}

ChessPiece::FIGURE HardwareInterface::ScanNFC(int _retry_count)
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (iocontroller != nullptr)
		{
			return iocontroller->ScanNFC(_retry_count);
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		//TODO
	}
}
	
	

	
	

void HardwareInterface::enable_motors()
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (tmc5160_x != nullptr && tmc5160_y != nullptr)
		{
			tmc5160_x->enable_motor();
			tmc5160_y->enable_motor();
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		//NOT NEEDED FOR REV 2 (GCODE) => MOTORS WILL AUTOMATICLY ACTIVATED AFTER A MOVEMENT COMMAND (G0, G1)
	}
}

void HardwareInterface::disable_motors()
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (tmc5160_x != nullptr && tmc5160_y != nullptr)
		{
			tmc5160_x->disable_motor();
			tmc5160_y->disable_motor();
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		if (gcode_interface != nullptr)
		{
			gcode_interface->disable_motors();
		}
	}
}
	
bool HardwareInterface::is_target_position_reached()
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (tmc5160_x != nullptr && tmc5160_y != nullptr)
		{
			if (tmc5160_x->is_target_position_reached() && tmc5160_y->is_target_position_reached())
			{
				return true;
			}
			return false;
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		if (gcode_interface != nullptr)
		{
			return gcode_interface->is_target_position_reached();
		}
	}
}
	
void HardwareInterface::move_to_postion_mm_absolute(int _x, int _y, bool _blocking)
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (tmc5160_x != nullptr && tmc5160_y != nullptr)
		{
			tmc5160_x->move_to_postion_mm_absolute(_x, _blocking);
			tmc5160_y->move_to_postion_mm_absolute(_y, _blocking);	
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		if (gcode_interface != nullptr)
		{
			gcode_interface->move_to_postion_mm_absolute(_x, _y, _blocking);
		}
	}
}
	
void HardwareInterface::home_sync()
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (tmc5160_x != nullptr && tmc5160_y != nullptr)
		{
			tmc5160_x->atc_home_sync();
			tmc5160_y->atc_home_sync();
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		if (gcode_interface != nullptr)
		{
			gcode_interface->home_sync();
		}
	}
}
	
void HardwareInterface::set_speed_preset(HardwareInterface::HI_TRAVEL_SPEED_PRESET _preset)
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (tmc5160_x != nullptr && tmc5160_y != nullptr)
		{
			tmc5160_x->atc_set_speed_preset(static_cast<TMC5160::TRAVEL_SPEED_PRESET>(_preset));
			tmc5160_y->atc_set_speed_preset(static_cast<TMC5160::TRAVEL_SPEED_PRESET>(_preset));
		}
			
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD)
	{
		if (gcode_interface != nullptr)
		{
			switch (_preset)
			{
			case HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE : gcode_interface->set_speed_preset(8000); break; //MM per MINUTE
			case HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_TRAVEL : gcode_interface->set_speed_preset(15000); break;
			default:
				break;
			}
			
		}
	}
}