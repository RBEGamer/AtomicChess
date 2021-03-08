#include "TMC5160.h"

TMC5160::TMC5160(MOTOR_ID _id) {
	_motor_id = _id; //SAVE MOTOR ID INSTANCE
	//INIT WIRING PI FOR GPIO OF THE DRIVE ENABLE
	wiringPiSetup();
	
	//GET SPI COMMUNICATION CLASS WIRRTEN FOR THIS PROJECT
	if(!SPICommunication::getInstance()->isInitialised()){
		if(SPICommunication::getInstance()->lastErrorCode() != 0){
			return;
		}
		return;
	}
	//REGIER CS PINS
	
	if (_id == MOTOR_ID::MOTOR_0) {
		SPI_CS_DEVICE = SPICommunication::SPI_DEVICE::MOTOR_0;
		SPICommunication::getInstance()->register_cs_gpio(SPI_CS_DEVICE, CS_GPIO_NUMBER_MOTOR_0);
		DRVEN_GPIO = DRVEN_GPIO_NUMBER_MOTOR_0;
	}
	else  if (_id == MOTOR_ID::MOTOR_1) {
		SPI_CS_DEVICE = SPICommunication::SPI_DEVICE::MOTOR_1;
		SPICommunication::getInstance()->register_cs_gpio(SPI_CS_DEVICE, CS_GPIO_NUMBER_MOTOR_1);	
		DRVEN_GPIO = DRVEN_GPIO_NUMBER_MOTOR_1;
	}
	
//	std::cout << "CS GOPO IS " << CS_GPIO  << std::endl;
	std::cout << "DRV EN GOIO " << DRVEN_GPIO << std::endl;
	
	
	
	//SETUP DRVENPIN
	pinMode(DRVEN_GPIO, OUTPUT);
	disable_motor();//DISBALE DRIVER OUTPUT STATE
	_motor_id = _id;

	default_settings();
	
}

TMC5160::~TMC5160() {
	stop_motor();
	disable_motor();
}



void TMC5160::enable_motor()
{
	digitalWrite(DRVEN_GPIO, LOW);
}


void TMC5160::disable_motor()
{
	digitalWrite(DRVEN_GPIO, HIGH);
}
void TMC5160::set_VSTART(int _value) {
    write(REGDEF_VSTART, _value);
    RAMP_VSTART = _value;
}

void TMC5160::set_A1(int _value) {
    write(REGDEF_A1, _value);
    RAMP_A1 = _value;
}

void TMC5160::set_V1(int _value) {
    write(REGDEF_V1, _value);
    RAMP_V1 = _value;
}

void TMC5160::set_AMAX(int _value) {
	write(REGDEF_AMAX, _value);
    RAMP_AMAX = _value;
}

void TMC5160::set_VMAX(int _value) {
    write(REGDEF_VMAX, _value);
    RAMP_VMAX = _value;
}

void TMC5160::set_DMAX(int _value) {
    write(REGDEF_DMAX, _value);
    RAMP_DMAX = _value;
}

void TMC5160::set_D1(int _value) {
    write(REGDEF_D1, _value);
    RAMP_D1 = _value;
}

void TMC5160::set_VSTOP(int _value) {
    write(REGDEF_VSTOP, _value);
    RAMP_VSTOP = _value;
}

void TMC5160::default_settings()
{
	/// MULTISTEP_FILT = 1, EN_PWM_MODE = 1 enables stealthChop
	   write(REGDEF_GCONF, 0x0000000C);
	/// TOFF = 3, HSTRT = 4, HEND = 1, TBL = 2, CHM = 0 (spreadCycle)
	   write(REGDEF_CHOPCONF, 0x000100C3);
	/// IHOLD = 2, IRUN = 15 (max current), IHOLDDELAY = 8
	    write(REGDEF_IHOLD_IRUN, 0x00080F02);
	/// TPOWERDOWN = 10: Delay before powerdown in standstill
	   write(REGDEF_TPOWERDOWN, 0x0000000A);
	/// TPWMTHRS = 500
	   write(REGDEF_A1, 0x000001F4);
	///WRITE THE DEFAULT RAMP PARAMETERS
	reset_ramp_defaults(); 
	/// Position mode
	write(REGDEF_RAMPMODE, 0);
	/// Set current positin to 0
	write(REGDEF_XACTUAL, 0);
	/// Set XTARGET to 0, which holds the motor at the current position
	write(REGDEF_XTARGET, 0);
	
	steps_per_mm(DEFAULT_STEPS_PER_MM); ///WRITE STEPS PER MM
}

void TMC5160::write_ramp_params() {
	set_VSTART(RAMP_VSTART);
	set_A1(RAMP_A1);
	set_V1(RAMP_V1);
	set_AMAX(RAMP_AMAX);
	set_VMAX(RAMP_VMAX);
	set_DMAX(RAMP_DMAX);
	set_D1(RAMP_D1);
	set_VSTOP(RAMP_VSTOP);
}
void TMC5160::reset_ramp_defaults() {
	RAMP_VSTART = 1;
	RAMP_A1 = 25000;
	RAMP_V1 = 250000;
	RAMP_AMAX = 5000;
	RAMP_VMAX = 1000000;
	RAMP_DMAX = 5000;
	RAMP_D1 = 50000;
	RAMP_VSTOP = 10;
	write_ramp_params();
}




int TMC5160::read(int _address)
{
	const size_t DATA_LEN = 5;
	uint8_t write_buffer[] = { 0, 0, 0, 0, 0 };
	uint8_t read_buffer[] = { 0, 0, 0, 0, 0 };
	int value = 0;
	int res = 0;
	write_buffer[0] = _address & 0x7F;
	read_buffer[0] = _address & 0x7F;
	//digitalWrite(CS_GPIO, LOW);
	res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE ,write_buffer, DATA_LEN);
	//res = wiringPiSPIDataRW(0, write_buffer, DATA_LEN);
	//digitalWrite(CS_GPIO, HIGH);
	//It will look like [address, 0, 0, 0, 0]
	 //   digitalWrite(CS_GPIO, LOW);
	 res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE, read_buffer, DATA_LEN);
	//res = wiringPiSPIDataRW(0, read_buffer, DATA_LEN);
	//digitalWrite(CS_GPIO, HIGH);
	//# Parse data returned from SPI transfer/read
	//read_buffer =write_buffer;
	value = read_buffer[1];
	value = value << 8;
	value |= read_buffer[2];
	value = value << 8;
	value |= read_buffer[3];
	value = value << 8;
	value |= read_buffer[4];

	// value = read_buffer;
	 return value;
}



int TMC5160::write(int _address, int _data)
{
	const size_t DATA_LEN = 5;
	uint8_t write_buffer[] = { 0, 0, 0, 0, 0 };


	write_buffer[0] = _address | 0x80;

	write_buffer[1] = 0xFF & (_data >> 24);
	write_buffer[2] = 0xFF & (_data >> 16);
	write_buffer[3] = 0xFF & (_data >> 8);
	write_buffer[4] = 0xFF & _data;
	
	//WRITE DATA OVER SPI; SET CS PIN LOW BEFORE WRITING
	//digitalWrite(CS_GPIO, LOW);
	//int res = wiringPiSPIDataRW(0, write_buffer, DATA_LEN);
	int res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE ,write_buffer, DATA_LEN);
	//SET CS PIN TO HIGH TO DEACTIVATE CHIP
	//digitalWrite(CS_GPIO, HIGH);
	return res;

}


int TMC5160::two_complement(int _value, int _bits /* = 32 */)
{
	signed int signed_value = 0;
	if ((_value & (1 << (_bits - 1))) != 0) {
		signed_value = _value - (1 << _bits);
	}
	else {
		signed_value = _value;
	}
	return signed_value;
}



void TMC5160::go_to(int _position) {
	
	write(REGDEF_RAMPMODE, 0);
	write(REGDEF_XTARGET, _position);
}

int TMC5160::get_position()
{
	return two_complement(read(REGDEF_XACTUAL));
}

void TMC5160::hold_mode() {
	write(REGDEF_RAMPMODE, 3);
}

void TMC5160::position_mode() {
	write(REGDEF_RAMPMODE, 0);
}

int TMC5160::get_latched_position()
{
	return two_complement(read(REGDEF_XLATCH));
}

int TMC5160::get_velocity() {
	int current_velocity = read(REGDEF_VACTUAL);
	//USING ONLY THE 24 BIT BECAUSE THE OTHER BITS ARE THE ADRESS
	current_velocity = two_complement(current_velocity, 24);
	return current_velocity;
}

void TMC5160::move_velocity(TMC5160::VELOCITY_DIRECTION _dir, int _v_max, int _a_max) {
	enable_motor();
	write(REGDEF_VMAX, _v_max);
	write(REGDEF_AMAX, _a_max);

	bool error = false;
	int velocity_mode = 0;
	if (_dir == VELOCITY_DIRECTION::POSITIVE) {
		velocity_mode = 1;
		error = false;
	}
	else if (_dir == TMC5160::VELOCITY_DIRECTION::NEGATIVE) {
		velocity_mode = 2;
		error = false;
	}
	else {
		error = true;
	}

	if (!error) {
		write(REGDEF_RAMPMODE, velocity_mode);
	}

}

void TMC5160::stop_motor() {
	move_velocity(TMC5160::VELOCITY_DIRECTION::POSITIVE, 0, RAMP_AMAX);
	while (get_velocity() != 0) {
		std::this_thread::sleep_for(std::chrono::microseconds(2));
#ifdef USE_STD_LOG
		std::cout << "stop_motor_wait_loop " << get_velocity() << std::endl; 
#endif

	}
	hold_mode();
	set_VMAX(RAMP_VMAX);
	position_mode();
}

int TMC5160::steps_per_mm() {
	return spm;
}

void TMC5160::steps_per_mm(int _steps) {
	spm = _steps;
}

int TMC5160::steps_to_mm(int _steps)
{
	return _steps / steps_per_mm();
}
	
void TMC5160::move_to_postion_mm_relative(int _rpos, bool _blocking) {
	
	enable_motor();
	int rel_steps = _rpos*steps_per_mm();
	
	int target_steps = get_position() + rel_steps;
	int currpos = get_position();
	go_to(target_steps);
	std::this_thread::sleep_for(std::chrono::microseconds(1));
	if (_blocking)
	{
		while (currpos != target_steps )
		{
			currpos = get_position();
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
		disable_motor();
	}
}


void TMC5160::move_to_postion_mm_absolute(int _apos, bool _blocking) {
	int rel_steps = _apos - steps_to_mm(get_position());
	move_to_postion_mm_relative(rel_steps, _blocking);
}


TMC5160::RAMP_STATUS TMC5160::get_ramp_stauts()
{
	TMC5160::RAMP_STATUS tmp;
	read(REGDEF_RAMPSTAT);
	
	int readres = read(REGDEF_RAMPSTAT);
	tmp.status_signal = (bool)(readres & (1 << 13));
	tmp.seconds_move = (bool)(readres & (1 << 12));
	tmp.zero_wait_active = (bool)(readres & (1 << 11));
	tmp.vzero = (bool)(readres & (1 << 10));
	tmp.position_reached = (bool)(readres & (1 << 9));
	tmp.velocity_reached = (bool)(readres & (1 << 8));
	tmp.event_pos_reached = (bool)(readres & (1 << 7));
	tmp.stop_signal = (bool)(readres & (1 << 6));
	tmp.event_stop_r = (bool)(readres & (1 << 5));
	tmp.event_stop_l = (bool)(readres & (1 << 4));
	tmp.status_latch_r = (bool)(readres & (1 << 3));
	tmp.status_latch_l = (bool)(readres & (1 << 2));
	tmp.status_stop_r = (bool)(readres & (1 << 1));
	tmp.status_stop_l = (bool)(readres & (1 << 0));
	//tmp = (TMC5160::RAMP_STATUS)readres;
	return tmp;
}



bool TMC5160::is_target_position_reached()
{ 
	return get_ramp_stauts().position_reached;
}

void TMC5160::wait_for_target_position_reached()
{
	while (!is_target_position_reached())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1)); 
	}
}

void TMC5160::enable_switch(TMC5160::REF_SWITCH _direction, bool _enable, bool _automatic_stop, bool _invert_button_level)
{
	read(REGDEF_SWMODE);
	int settings = read(REGDEF_SWMODE);
	bool err = false;
	BIT_SET(settings, 11); //SET TO HARDSTOP MODE (NOT RECOMMENDET IN DATASHEET)
	
	//FOR THE SPECIFIC INPUT REF_L or REF_R
	if(_direction == TMC5160::REF_SWITCH::REF_L)
	{
		if (_enable) {
			BIT_SET(settings, 5);       //enable latch
		}
		else {
			BIT_CLEAR(settings, 5);      //enable latch
		}
		if(_automatic_stop) {
			BIT_SET(settings, 0);
		}else{
			BIT_CLEAR(settings, 0);
		}
		
		
		if (_invert_button_level)
		{
			BIT_SET(settings, 2);
		}else{
			BIT_CLEAR(settings, 2);
		}
	}
	else if(_direction == TMC5160::REF_SWITCH::REF_R)
	{
		if (_enable) {
			BIT_SET(settings, 7);        //enable latch
		}
		else {
			BIT_CLEAR(settings, 7);       //enable latch
		}
		//ENABLE HARDSTOP BY DRIVER
		if(_automatic_stop) {
			BIT_SET(settings, 1);
		}else{
			BIT_CLEAR(settings, 1);
		}
		if (_invert_button_level)
		{
			BIT_SET(settings, 3);
		}else{
			BIT_CLEAR(settings, 3);
		}
	}else
	{
		err = true;
	}
	
	if (!err)
	{
		write(REGDEF_SWMODE, (int)settings);
	}
}



void TMC5160::set_postion_offset(int _offset)
{
	position_offset = _offset;
}


//FOR HOMING HTE REF_L input is used for the switch in Coordiante system Zero
void TMC5160::atc_home_sync()
{	
	enable_motor(); //ENABLE MOTOR
	enable_switch(TMC5160::REF_SWITCH::REF_L, true, true, true); //ENABLE LIMIT SWICHT
	int amx = RAMP_AMAX;
	int vmax = RAMP_VMAX;
	move_velocity(TMC5160::VELOCITY_DIRECTION::NEGATIVE, HOME_SPEED_VELOCITY, 1000);  //MOVE NEGATIVE TO LIMIT SWITCH
	
	
	//WAIT TO REACH THE ENDSTOP
	while(!get_ramp_stauts().status_stop_l) {
		std::this_thread::sleep_for(std::chrono::microseconds(1));		
	}
	//STOP MOTOR
	hold_mode();
	//SAVE LATCHED POSTION
	int offset = get_position() - get_latched_position();
	write(REGDEF_XACTUAL, offset);
	//SAVE OFFSET
	int currpos = get_position();
	set_postion_offset(currpos);
	//RESET RAMP
	write(REGDEF_RAMPSTAT, 4);
	//GOTO THE NEW ZERO POSTION
	set_AMAX(amx);
	set_VMAX(vmax);
	go_to(0);
	//DISABLE HARD ENDSTOP 
	enable_switch(TMC5160::REF_SWITCH::REF_L, true, false, true);
	disable_motor(); //DISBLE MOTOR
}





//ASYNC IMPLEMENTATION OF THE atc_home_sync FUNCTION, RETURNS TRUE IF HOMED
bool TMC5160::atc_home_async()
{
	if (atc_home_process_status == 0)
	{
		enable_motor();        //ENABLE MOTOR
		enable_switch(TMC5160::REF_SWITCH::REF_L, true, true, true);        //ENABLE LIMIT SWICHT
		move_velocity(TMC5160::VELOCITY_DIRECTION::NEGATIVE, HOME_SPEED_VELOCITY, 1000);         //MOVE NEGATIVE TO LIMIT SWITCH
		atc_home_process_status = 1;
		return false;
	}
	else if (atc_home_process_status == 1)
	{
		if(get_ramp_stauts().status_stop_l)
		{
			hold_mode();
			stop_motor();
			atc_home_process_status = 2;
			
		}
		
		return false;
	}
	else if (atc_home_process_status == 2)
	{
		hold_mode();
		//SAVE LATCHED POSTION
		int offset = get_position() - get_latched_position();
		write(REGDEF_XACTUAL, offset);
		int currpos = get_position();
		set_postion_offset(currpos);
		write(REGDEF_RAMPSTAT, 4);
		go_to(0);
		stop_motor();    //STOP MOTOR
		//DISABLE HARD ENDSTOP 
		enable_switch(TMC5160::REF_SWITCH::REF_L, true, false, true);
		disable_motor();    //DISBLE MOTOR
		atc_home_process_status = 3;
		return true;
	}
	else
	{
		return false;
	}
}

void TMC5160::atc_home_async_reset()
{
	atc_home_process_status = 0;
}


void TMC5160::atc_set_speed_preset(TRAVEL_SPEED_PRESET _preset)
{
	switch (_preset)
	{
	case TMC5160::TRAVEL_SPEED_PRESET::MOVE:
		set_VMAX(250000);
		set_AMAX(50000);
		break;
	case TMC5160::TRAVEL_SPEED_PRESET::TRAVEL:
		set_VMAX(800000);
		set_AMAX(80000);
		break;
	default:
		stop_motor();
		break;
	}
}

void TMC5160::move_velocity(VELOCITY_DIRECTION _dir, TRAVEL_SPEED_PRESET _preset)
{ 
	switch (_preset)
	{
	case TMC5160::TRAVEL_SPEED_PRESET::MOVE:
		move_velocity(_dir, 50000, 80000);
		break;
	case TMC5160::TRAVEL_SPEED_PRESET::TRAVEL:
		move_velocity(_dir, 50000, 80000);

		break;
	default:
		stop_motor();
		break;
	}
}