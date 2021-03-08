//
// Created by Marcel Ochsendorf on 17.10.20.
//

#ifndef UNTITLED1_TMC5160_H
#define UNTITLED1_TMC5160_H

#define USE_STD_LOG

#ifdef USE_STD_LOG
#include <iostream>
#endif

#include <cstdint>
#include <chrono>
#include <thread>
#include <math.h>
#include <iostream>

#include "SPICommunication.h"

#include "SHARED/WiringPi-master/wiringPi/wiringPi.h"

#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1

/* x=target variable, y=mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK_ALL(x,y) (!(~(x) & (y)))
#define BITMASK_CHECK_ANY(x,y) ((x) & (y))


class TMC5160 {


public:
	~TMC5160();
	
	const int DEFAULT_STEPS_PER_MM = 1140;
	enum MOTOR_ID
	{
		MOTOR_0,
		MOTOR_1
	};
	TMC5160(MOTOR_ID _id);

	enum DIRECTION {
		LEFT  = 1,
		RIGHT = 2
	};
	
	enum VELOCITY_DIRECTION
	{
		POSITIVE =1,
		NEGATIVE=2,
	};
	
	enum REF_SWITCH
	{
	         REF_L = 0,
			REF_R = 1
	};
	
	enum TRAVEL_SPEED_PRESET {
		MOVE = 0,
		TRAVEL = 1
	};
	
	struct RAMP_STATUS {
		bool status_signal;
		bool seconds_move;
		bool zero_wait_active;
		bool vzero;
		bool position_reached;
		bool velocity_reached;
		bool event_pos_reached;
		bool stop_signal;
		bool event_stop_r;
		bool event_stop_l;
		bool status_latch_r;
		bool status_latch_l;
		bool status_stop_r;
		bool status_stop_l;
	};
	
	TMC5160::RAMP_STATUS get_ramp_stauts();
	
	void enable_motor(); //ENABLES THE DRV_EN INPUT OF THE MOTOR DRIVER
	void disable_motor();//SIABLED THE DRV_EN INPUT OF THE MOTOR DRIVER
	void default_settings(); //RESTORES AND WRITES THE DEFAULT RAMP PARAMETERS AND SET THE DRIVER TO POSITION MODE
	void set_VSTART(int _value);
	void set_A1(int _value);
	void set_V1(int _value);
	void set_AMAX(int _value);
	void set_VMAX(int _value);
	void set_DMAX(int _value);
	void set_D1(int _value);
	void set_VSTOP(int _value);
	void write_ramp_params(); //WRITES THE RAMP PARAMETERS TO THE MOTOR DRIVER
	void reset_ramp_defaults(); // RESET THE RAMP PARAMETERS
	void enable_switch(TMC5160::REF_SWITCH _direction, bool _enable, bool _automatic_stop, bool _invert_button_level);  //ENABLE THE REF_L / REF_R INPUTS AND CONFIGURE THEM TO BE A HARD ENDSTP
	int get_latched_position(); //GET THE LAST MOTOR POSITION; WHEN A SWITCH WAS TRIGGRED
	int get_velocity(); //GET CURRENT MOTOR VELOCITY
	void go_to(int _position); //MOVE MOTOR TO STEPS POSTION //ABSOLUTE
	void move_velocity(TMC5160::VELOCITY_DIRECTION _dir, int  _v_max, int _a_max);  //ENABLE CONTONOUS MOTOR ROATION WITH A GIVEN SPEED; USED FOR  HOMING
	void move_velocity(TMC5160::VELOCITY_DIRECTION _dir, TMC5160::TRAVEL_SPEED_PRESET _preset);
	void stop_motor(); //STOP MOTOR
	void hold_mode(); //STOP MOTOR; LET MOTOR DRIVER ACITAVED
	void position_mode(); //SET MOTOR DRIVER INTO POSITION MODE (SEE GOTO FUNCTION)

	int read(int _address);
	int write(int _address, int _data);
	
	void debugprint_ramstatus(TMC5160::RAMP_STATUS _rmp);
	
	bool is_target_position_reached();
	void wait_for_target_position_reached();
	int get_position();

	int steps_per_mm();
	void steps_per_mm(int _steps);
	
	void set_postion_offset(int _offset);
	
	void move_to_postion_mm_relative(int _rpos, bool _blocking);

	void move_to_postion_mm_absolute(int _apos, bool _blocking);
	
	
	
	
	//FUNCTIONS ESPACIALLY CREATED FOR THE ATC PROJECT WITH THE ATC HARDWARE
	void atc_home_sync();
	
	bool atc_home_async();
	void atc_home_async_reset();
	
	void atc_set_speed_preset(TMC5160::TRAVEL_SPEED_PRESET _preset);
private:
	int two_complement(int _value, int _bits = 32);
	int steps_to_mm(int _steps); ///SET THE STEPS PER MM MANUALLY
	TMC5160::MOTOR_ID _motor_id;
	bool atc_home_process_status = 0; ///SET TO ZERO AND CALL atc_home_async; this variable is used for the statemachine
	//ATC HOME SETTINGS
	const int HOME_SPEED_VELOCITY = 150000;
	
	//RESOLUTION SETTINGS
	int spm = 1140; ///THIS IS THE STEPS PER MM FOR THE GT2 20 TEETH 6mm pulley
	int current_postion = 0;
	int position_offset = 0; ///postion offset used by the postion of the endstop

	//GPIO SETTINGS
	const int CS_GPIO_NUMBER_MOTOR_0 = 0;  //USING GPIO 10 = CE_0
	const int DRVEN_GPIO_NUMBER_MOTOR_0 = 4;

	const int CS_GPIO_NUMBER_MOTOR_1 = 2;  //USING GPIO 10 = CE_0
	const int DRVEN_GPIO_NUMBER_MOTOR_1 = 5;

	SPICommunication::SPI_DEVICE SPI_CS_DEVICE; ///stores the id of the cs pin mapping
	int DRVEN_GPIO = 0; ///ACTUALLITY USED GPIO FOR DRV_EN PIN

	//TMC5160 RAMP PARAMETERS
	int RAMP_VSTART = 1;
	int RAMP_A1 = 25000;
	int RAMP_V1 = 250000;
	int RAMP_AMAX = 5000;
	int RAMP_VMAX = 1000000;
	int RAMP_DMAX = 5000;
	int RAMP_D1 = 50000;
	int RAMP_VSTOP = 10;
	//TMC5160 REGISTER SPI ADRESSES
	const int REGDEF_GCONF      = 0x00;
	const int REGDEF_GSTAT      = 0x01;
	const int REGDEF_IFCNT      = 0x02;
	const int REGDEF_SLAVECONF  = 0x03;
	const int REGDEF_INP_OUT    = 0x04;
	const int REGDEF_X_COMPARE  = 0x05;
	const int REGDEF_GLOBAL_SCALER = 0x08; //CURRENT REGISTER
	const int REGDEF_IHOLD_IRUN = 0x10;
	const int REGDEF_TPOWERDOWN = 0x11;
	const int REGDEF_TSTEP      = 0x12;
	const int REGDEF_TPWMTHRS   = 0x13;
	const int REGDEF_TCOOLTHRS  = 0x14;
	const int REGDEF_THIGH      = 0x15;
	const int REGDEF_RAMPMODE   = 0x20;
	const int REGDEF_XACTUAL    = 0x21;
	const int REGDEF_VACTUAL    = 0x22;
	const int REGDEF_VSTART     = 0x23;
	const int REGDEF_A1         = 0x24;
	const int REGDEF_V1         = 0x25;
	const int REGDEF_AMAX       = 0x26;
	const int REGDEF_VMAX       = 0x27;
	const int REGDEF_DMAX       = 0x28;
	const int REGDEF_D1         = 0x2A;
	const int REGDEF_VSTOP      = 0x2B;
	const int REGDEF_TZEROWAIT  = 0x2C;
	const int REGDEF_XTARGET    = 0x2D;
	const int REGDEF_VDCMIN     = 0x33;
	const int REGDEF_SWMODE     = 0x34;
	const int REGDEF_RAMPSTAT   = 0x35;
	const int REGDEF_XLATCH     = 0x36;
	const int REGDEF_ENCMODE    = 0x38;
	const int REGDEF_XENC       = 0x39;
	const int REGDEF_ENC_CONST  = 0x3A;
	const int REGDEF_ENC_STATUS = 0x3B;
	const int REGDEF_ENC_LATCH  = 0x3C;
	const int REGDEF_MSLUT0     = 0x60;
	const int REGDEF_MSLUT1     = 0x61;
	const int REGDEF_MSLUT2     = 0x62;
	const int REGDEF_MSLUT3     = 0x63;
	const int REGDEF_MSLUT4     = 0x64;
	const int REGDEF_MSLUT5     = 0x65;
	const int REGDEF_MSLUT6     = 0x66;
	const int REGDEF_MSLUT7     = 0x67;
	const int REGDEF_MSLUTSEL   = 0x68;
	const int REGDEF_MSLUTSTART = 0x69;
	const int REGDEF_MSCNT      = 0x6A;
	const int REGDEF_MSCURACT   = 0x6B;
	const int REGDEF_CHOPCONF   = 0x6C;
	const int REGDEF_COOLCONF   = 0x6D;
	const int REGDEF_DCCTRL     = 0x6E;
	const int REGDEF_DRVSTATUS  = 0x6F;
	const int REGDEF_PWMCONF    = 0x70;
	const int REGDEF_PWMSTATUS  = 0x71;
	const int REGDEF_ENCM_CTRL  = 0x72;
	const int REGDEF_LOST_STEPS = 0x73;
};


#endif //UNTITLED1_TMC5160_H
