//
// Created by Marcel Ochsendorf on 18.10.20.
//

#ifndef SPICOMMUNICATION_H
#define SPICOMMUNICATION_H


#include "SHARED/WiringPi-master/wiringPi/wiringPi.h"
#include "SHARED/WiringPi-master/wiringPi/wiringPiSPI.h"
#include <thread>
#include <mutex>



#define USE_STD_LOG

#ifdef USE_STD_LOG
#include <iostream>
#endif

#define MAGIC_ACK_BYTE 254 //ACK BYTE FROM IO CONTROLLER THAT COMMAND WAS EXECUTED
#define SPI_RW_DELAY 100 //WAIT BEWEEN EACH RW TRY
#define SPI_RW_ACK_RETRY 5 //MAX TIMES RW TRY
class SPICommunication {


public:

    static SPICommunication* getInstance(); 
    
	///AFTER ADDING A NEW DEVICE ENTRY, ALSO ADD AN ENTRY INTO assigned_cs_gpios_mapping in SPICOmmunication.h
    enum SPI_DEVICE{
        MOTOR_0 = 0,
        MOTOR_1 = 1,
        IO_CONTROLLER = 2,
        RESERVED_1 = 4,
        RESERVED_2 = 5
    };

   

    
   
    int lastErrorCode();
    bool isInitialised();


    int spi_write(SPICommunication::SPI_DEVICE _device ,uint8_t* _data, int _len); //WRITES DATA TO THE SPI BUS => READ BYTES IN _data
	int spi_write_ack(SPICommunication::SPI_DEVICE _device, uint8_t* _data, int _len); //SAME AS  spi_write BUT CHECKS RESULT ACK WITH MAGIC BYTE; SEE #define MAGIC_ACK_BYTE
    bool register_cs_gpio(SPICommunication::SPI_DEVICE _device, int _gpio_number);

    
private:
    //CONSTRUCOTRS FOR SINGLETONS
    SPICommunication();
    SPICommunication ( const SPICommunication& );
    ~SPICommunication();
    //ERR/DEBUG FUNTIONS
    int last_error = 0;
    bool is_initialised = false;
    static SPICommunication* instance; //SINGLETON INSTANCE
    
    //SPI CS METHODS
    void disable_devices();
    void disable_device(SPICommunication::SPI_DEVICE _device);
    void enable_device(SPICommunication::SPI_DEVICE _device);
    //SPI SETTINGS
    const int SPI_SPEED = 1000000;
	const int SPI_MODE = 3;
	const uint8_t SPI_BPW = 8;
    //THREAD STUFF
    static std::mutex acces_lock_mutex;
	
	
};


#endif //SPICOMMUNICATION_H
