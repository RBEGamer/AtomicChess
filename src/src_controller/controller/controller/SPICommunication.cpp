//
// Created by Marcel Ochsendorf on 18.10.20.
//

#include "SPICommunication.h"
SPICommunication* SPICommunication::instance;
std::mutex SPICommunication::acces_lock_mutex;
int assigned_cs_gpios_mapping[] = { -1, -1, -1, -1, -1 };

 SPICommunication* SPICommunication::getInstance()
{
    //{
		std::lock_guard<std::mutex> lck(SPICommunication::acces_lock_mutex);
        if(SPICommunication::instance == nullptr)
        {
	        SPICommunication::instance = new SPICommunication();
        }
	return SPICommunication::instance;
   // }
}

SPICommunication::SPICommunication()
{
    //SETUP WIRING PI
    //ITS POSSIBLE TP CALL THE SETUP ROUTINE MULTIBLE TIMES
    //WIRINGPI CHECK FOR THIS
    wiringPiSetup();

    //TRY TO GET OPEN THE SPI BUF (FILE)
	int fd = wiringPiSPISetup(0, SPI_SPEED);
	if (fd < 0) {
		std::cout << "ERROR ----SPI FD IS" << fd << std::endl;
        last_error = 1;
        return;
	}
	
    is_initialised = true;
}





int SPICommunication::lastErrorCode(){
    int tmp = last_error;
#ifdef USE_STD_LOG
	std::cout << "lastErrorCode IS"<< tmp << std::endl; 
#endif
    last_error = 0;
	
    return tmp;
}


int SPICommunication::spi_write(SPICommunication::SPI_DEVICE _device, uint8_t* _data, int _len){
	int res = 0;
	//disable_devices();
	
	enable_device(_device);
	wiringPiSPIDataRW(0, _data, _len);
	disable_device(_device);
	//deaktivate alle cs pins bis auf device
	//sende
	//deaktiviere alle
	return res;
}

//SAME AS  spi_write BUT CHECKS RESULT ACK WITH MAGIC BYTE; SEE #define MAGIC_ACK_BYTE
int SPICommunication::spi_write_ack(SPICommunication::SPI_DEVICE _device, uint8_t* _data, int _len)
{
	
	uint8_t* buffer_r{ new uint8_t[_len] { 0 }};
	uint8_t* buffer_w{ new uint8_t[_len] { 0 }};
	//POPULATE BUFFERS FOR READ WRITE
	for (size_t i = 0; i < _len; i++)
	{
		buffer_w[i] = _data[i];
	}
	for (size_t i = 0; i < _len; i++)
	{
		buffer_r[i] = 0;
	}
	
	
	volatile int res = -1;
	volatile int c = 0; //RETRY COUNTER
	while (true)
	{
		//POPULATE COMMAND BUFFER
		for (size_t i = 0; i < _len; i++)
		{
			buffer_w[i] = _data[i];
		}
		
		for (size_t i = 0; i < _len; i++)
		{
			buffer_r[i] = 0;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(SPI_RW_DELAY));
		res = SPICommunication::getInstance()->spi_write(_device, buffer_w, _len); //WRITE
		res = SPICommunication::getInstance()->spi_write(_device, buffer_r, _len);//READ RESULT
		//PARSE RESULT TO A FIGURE AND CHECK IF ITS VALID
		if(buffer_r[0] == MAGIC_ACK_BYTE)
		{
			break;
		}
		c++;
		if (c > SPI_RW_ACK_RETRY)
		{
			break;
		}
	}
	
	return res;
}

bool SPICommunication::isInitialised(){
    return is_initialised;
}


void SPICommunication::disable_devices(){

disable_device(SPICommunication::SPI_DEVICE::MOTOR_0);
disable_device(SPICommunication::SPI_DEVICE::MOTOR_1);
disable_device(SPICommunication::SPI_DEVICE::IO_CONTROLLER);
disable_device(SPICommunication::SPI_DEVICE::RESERVED_1);
disable_device(SPICommunication::SPI_DEVICE::RESERVED_2);

}

void SPICommunication::disable_device(SPICommunication::SPI_DEVICE _device){
	if (assigned_cs_gpios_mapping[_device] < 0)
	{
#ifdef USE_STD_LOG
		std::cout << "enable_device(SPICommunication::SPI_DEVICE _device) -> PLEASE CALL register_cs_gpio first" << std::endl; 
#endif
		return;
	}
	
	digitalWrite(assigned_cs_gpios_mapping[_device], HIGH);
}

void SPICommunication::enable_device(SPICommunication::SPI_DEVICE _device){
	if (assigned_cs_gpios_mapping[_device] < 0)
	{
#ifdef USE_STD_LOG
		std::cout << "enable_device(SPICommunication::SPI_DEVICE _device) -> PLEASE CALL register_cs_gpio first" << std::endl; 
#endif
		return;
	}
	digitalWrite(assigned_cs_gpios_mapping[_device], LOW);
}


bool SPICommunication::register_cs_gpio(SPICommunication::SPI_DEVICE _device, int _gpio_number)
{
	if (_gpio_number < 0) {
#ifdef USE_STD_LOG
		std::cout << "register_cs_gpio(SPICommunication::SPI_DEVICE) GPIO NUMBERING IS POSTIVE" << std::endl; 
#endif
		return false;
		
	}
	
	//UNREGISTER OLD MAPPING IF PIN CHANGED
	if (assigned_cs_gpios_mapping[_device] >= 0)
	{
		pinMode(assigned_cs_gpios_mapping[_device], INPUT);
	}
	
	assigned_cs_gpios_mapping[_device] = _gpio_number; ///MAP PIN NUMBER TO CS ENUM NAME
	pinMode(_gpio_number, OUTPUT); //SET PIN TO OUTPUT
	disable_device(_device);//SET TO HIGH = DISBALE DEVICE
	
#ifdef USE_STD_LOG
	std::cout << "register_cs_gpio(SPICommunication::SPI_DEVICE) REGISTERED CS GPIO " << _gpio_number  << " AT POSITION " << _device << std::endl; 
#endif
	return true; 
}