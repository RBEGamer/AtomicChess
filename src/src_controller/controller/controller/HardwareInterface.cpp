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
	else if (hwrevstr == "PROD_V1")
	{
		hwrev = HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD;
	}else if (hwrevstr == "PROD_V2")
    {
        hwrev = HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2;
    }else if (hwrevstr == "PROD_V3")
    {
        hwrev = HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3;
    }else if (hwrevstr == "VIRT")
    {
        hwrev = HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_VIRT;
    }else
	{
		init_complete = false;
		return;
	}
	LOG_F(INFO, "HardwareInterface::HardwareInterface got hwrev ");
    LOG_F(INFO, hwrevstr.c_str());
	//AFTER DETERM OF THE HW REV THE SPECIFIC HARDWARE CAN BE SETUP
	init_complete = init_hardware(hwrev);
}


bool HardwareInterface::init_hardware(HardwareInterface::HI_HARDWARE_REVISION _hwrev)
{
    LOG_F(INFO, "HardwareInterface::init_hardware");
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2 )
	{
	    //INIT THE MARLIN GCODE SENDER INTERFACE
		if (gcode_interface == nullptr) {
			int baud = 115200;
			ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_BAUD, baud);
			gcode_interface = new GCodeSender(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_BOARD_SERIAL_PORT), baud);


			//REST EEPROM IF SET
			if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_RESET_EEPROM_TO_DEFAULT_DURING_STARTUP)){
                gcode_interface->reset_eeprom();
                LOG_F(INFO, "HardwareInterface::init_hardware RESET EEPROM TO DEFAULT");
			}


			gcode_interface->configure_marlin();  //CONFIGURE MARLIN FOR STARTUP
			setCoilState(HardwareInterface::HI_COIL::HI_COIL_A, false);
			setCoilState(HardwareInterface::HI_COIL::HI_COIL_B, false);
            //SET STEPS PER MM AFTER! INIT
            int spm = 0;
            ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::MECHANIC_STEPS_PER_MM, spm);
            if (spm > 0)
            {
                gcode_interface->set_steps_per_mm(spm,spm);
                LOG_F(INFO, "HardwareInterface::init_hardware set steps per mm setting to: %i" ,spm);
            }

		}


        //INIT USERBOARD CONTROLLER TO READ NFC TAGS USING THE ARDUINO SERIAL BASED APPROACH
		if(userboardcontroller_interface == nullptr){
            int baud = 115200;
            ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_BAUD, baud);
            userboardcontroller_interface = new UserBoardController(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_PORT), baud);
		}


    }else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){

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


        //INIT USERBOARD CONTROLLER TO READ NFC TAGS USING THE ARDUINO SERIAL BASED APPROACH
        if(userboardcontroller_interface == nullptr){
            int baud = 115200;
            ConfigParser::getInstance()->getInt(ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_BAUD, baud);
            userboardcontroller_interface = new UserBoardController(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::HARDWARE_UBC_SERIAL_PORT), baud);
        }


	}else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_VIRT){
        LOG_F(INFO, "HardwareInterface::init_hardware INIT VIRTUAL HARDWARE");
    }else{
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

    if (userboardcontroller_interface != nullptr)
    {
        delete userboardcontroller_interface;
    }

}

//LOAD CONFGI
bool HardwareInterface::check_hw_init_complete()
{
	return init_complete;
}

bool HardwareInterface::is_production_hardware()
{
	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool HardwareInterface::is_simulates_hardware()
{
    if (hwrev == HardwareInterface::HI_HWREV_VIRT)
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{

	    //USE THE SAME COLOR MODEL AS THE HWREV_DK VERSION
	    int hsv_col = 0; //0 is also a color on a 360 degree circle

        switch (_state) {
            case HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_OFF: hsv_col = 240;break; //SPECIAL CASE DIM THE OFF STATE
            case HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_IDLE: hsv_col = 180;break;
            case HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_WHITE_TURN: hsv_col = 140;break;
            case HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PLAYER_BLACK_TURN: hsv_col = 90;break;
            case HardwareInterface::HI_TURN_STATE_LIGHT::HI_TSL_PRECCESSING: hsv_col = 35;break;
            default:hsv_col=0;break;
        }



        if (gcode_interface != nullptr) {
            gcode_interface->set_led(hsv_col);
        }

    }else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if(userboardcontroller_interface != nullptr){
        userboardcontroller_interface->set_led((int)_state);
        }
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		//MORE LOGIC NEEDED HERE
		//IT DEPENDS ON THE ONFIGURATION OF MARLIN WHICH INDEX THE SERVOS HAVE TO WE NEED TO LOAD IT FROM THE CONFIG FIRST
        if (gcode_interface != nullptr)
        {
		if(_coil == HardwareInterface::HI_COIL::HI_COIL_A || _coil == HardwareInterface::HI_COIL::HI_COIL_B) {
            if (_state) {
                gcode_interface->setServo(ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX),
                                          ConfigParser::getInstance()->getInt_nocheck(
                                                  ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_UPPER_POS));
            } else {
                gcode_interface->setServo(ConfigParser::getInstance()->getInt_nocheck(
                        ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_A_INDEX),
                                          ConfigParser::getInstance()->getInt_nocheck(
                                                  ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_SERVO_COIL_BOTTOM_POS));
            }
        }
		}
	}
	return true;
}

ChessPiece::FIGURE HardwareInterface::ScanNFC()
{

	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (iocontroller != nullptr)
		{
            const int retry_count = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_UBC_NFC_READ_RETRY_COUNT) + 1;
            ChessPiece::FIGURE fig= iocontroller->ScanNFC(retry_count);
            ChessPiece::FigureDebugPrint(fig);
			return fig;
		}
	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
        if(userboardcontroller_interface != nullptr){
            return userboardcontroller_interface->read_chess_piece_nfc();
        }
    }else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_VIRT){
        LOG_F(INFO,"HardwareInterface::ScanNFC() return an invalid figure due HWREV_VIRT");



    }else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if(userboardcontroller_interface != nullptr) {
            return userboardcontroller_interface->read_chess_piece_nfc();
        }
	}else{
        LOG_F(ERROR,"HardwareInterface::ScanNFC() return an invalid figure due invalid HW CONFIG");
	}

	//ELSE RETURN A INVALID FIGURE
    ChessPiece::FIGURE fig;
	fig.color = ChessPiece::COLOR::COLOR_UNKNOWN;
	fig.type = ChessPiece::TYPE::TYPE_INVALID;
	fig.figure_number = -1;
	fig.unique_id = -1;
	fig.is_empty = true;
	fig.figure_read_failed = true;
	return fig;
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		//NOT NEEDED FOR REV 2 (GCODE) => MOTORS WILL AUTOMATICLY ACTIVATED AFTER A MOVEMENT COMMAND (G0, G1,...)
	}else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if (tmc5160_x != nullptr && tmc5160_y != nullptr)
        {
            tmc5160_x->enable_motor();
            tmc5160_y->enable_motor();
        }
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		if (gcode_interface != nullptr)
		{
			gcode_interface->disable_motors();
		}

    }else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if (tmc5160_x != nullptr && tmc5160_y != nullptr)
        {
            tmc5160_x->disable_motor();
            tmc5160_y->disable_motor();
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		if (gcode_interface != nullptr)
		{
			return gcode_interface->is_target_position_reached();
		}
    }else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_VIRT){
	    return true;

    }else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if (tmc5160_x != nullptr && tmc5160_y != nullptr)
        {
            if (tmc5160_x->is_target_position_reached() && tmc5160_y->is_target_position_reached())
            {
                return true;
            }
            return false;
        }
    }


	LOG_F(ERROR,"HardwareInterface::is_target_position_reached() return false due invalid HW CONFIG");
	return false;
}

void HardwareInterface::move_to_postion_mm_absolute(int _x, int _y, bool _blocking)
{
    //CHECK FOR SWITCH X Y AXIS
    if(ConfigParser::getInstance()->getBool_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_SIWTHC_XY_AXIS)){
        const int tmp = _x;
        _y = _x;
        _x = tmp;
    }

	if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
	{
		if (tmc5160_x != nullptr && tmc5160_y != nullptr)
		{
			tmc5160_x->move_to_postion_mm_absolute(_x, _blocking);
			tmc5160_y->move_to_postion_mm_absolute(_y, _blocking);
		}

	}
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		if (gcode_interface != nullptr)
		{
			gcode_interface->move_to_postion_mm_absolute(_x, _y, _blocking);
			gcode_interface->disable_motors();
		}

    }else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if (tmc5160_x != nullptr && tmc5160_y != nullptr)
        {
            tmc5160_x->move_to_postion_mm_absolute(_x, _blocking);
            tmc5160_y->move_to_postion_mm_absolute(_y, _blocking);
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		if (gcode_interface != nullptr)
		{
			gcode_interface->home_sync();
            gcode_interface->disable_motors();
		}


    }else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if (tmc5160_x != nullptr && tmc5160_y != nullptr)
        {
            tmc5160_x->atc_home_sync();
            tmc5160_y->atc_home_sync();
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
	else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
	{
		if (gcode_interface != nullptr)
		{
            //LOAD FEEDRATE FROM CONFIG
		    int fr_move = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_MOVE);
		    int fr_travel = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::MECHANIC_FEEDRATE_TRAVEL);
            //CHECK FOR FEEDRATE
		    if(fr_move <= 0 || fr_travel <= 0){
                fr_move = 100;
                fr_travel = 100;
                LOG_F(ERROR,"cant set speed_preset to fr_move %i or fr_travel %i USING DEFAULT OF 100 FOR BOTH",fr_move,fr_travel);
		    }
		    //WRITE FEEDRATE TO CONTROLLER MARLIN
			switch (_preset)
			{
			case HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_MOVE : gcode_interface->set_speed_preset(fr_move); break; //MM per MINUTE
			case HardwareInterface::HI_TRAVEL_SPEED_PRESET::HI_TSP_TRAVEL : gcode_interface->set_speed_preset(fr_travel); break;
			default:
				break;
			}

		}


    }else if(hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V3){
        if (tmc5160_x != nullptr && tmc5160_y != nullptr)
        {
            tmc5160_x->atc_set_speed_preset(static_cast<TMC5160::TRAVEL_SPEED_PRESET>(_preset));
            tmc5160_y->atc_set_speed_preset(static_cast<TMC5160::TRAVEL_SPEED_PRESET>(_preset));
        }
	}
}

void HardwareInterface::set_fan_state(HardwareInterface::HI_FAN _fan, bool _state){
    if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_DK)
    {

    }
    else if (hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD || hwrev == HardwareInterface::HI_HARDWARE_REVISION::HI_HWREV_PROD_V2)
    {
        int fan_index =-1 ;
        //LOAD FAN INDEX
        if(_fan == HI_FAN::HI_FAN_MOTOR_DRIVER){
            fan_index = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_MARLIN_COOLING_FAN_DRIVER_INDEX);
        }
        //SKIP IF DISBALED
        if(fan_index < 0){
            return;
        }
        //SET FAN STATE
        if (gcode_interface != nullptr)
        {
            gcode_interface->setFan(fan_index,((int)_state)*255);
        }
    }
};
