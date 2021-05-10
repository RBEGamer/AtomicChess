#pragma once
#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__


#include <iostream>
#include <chrono>
#include <thread>



#include "SPICommunication.h"
#include "ChessPiece.h"


#define NFC_READ_ACK_DELAY 50 ///HOW LONG TO WAIT FOR NFC DATA IN MS
class IOController
{
	
	
public:
	
	enum COIL
	{
		COIL_A = 0,
		COIL_B = 1,
		COIL_NFC = 2
	};
	
	enum LED
	{
		STAUS_LED_A = 0,
		STATUS_LED_B = 1
	};
	
	///BY ADDING NEW ENTRIES PLEASE EDIT THE OFFSET RESERVED ENTRIES IN IOController::COMMANDS
	enum TURN_STATE_LIGHT
	{
		TSL_OFF = 0,
		TSL_IDLE = 1,
		TSL_PLAYER_WHITE_TURN = 2,
		TSL_PLAYER_BLACK_TURN = 3,
		TSL_PRECCESSING = 4
	};
		
		
	IOController();
	~IOController();
	
	ChessPiece::FIGURE ScanNFC(int _retry_count = 1);
	
	void setCoilState(IOController::COIL _coil, bool _state);
	void setStatusLed(IOController::LED _led, bool _state);
	void invertCoilPolarity(IOController::COIL _coil, bool _invert_field);
	bool isInitialized();
	
	void setTurnStateLight(IOController::TURN_STATE_LIGHT _state);
private:
	///THIS ENUM CONTAINS ALL POSSIBLE COMMANDS TO THE IO BOARD
	enum COMMANDS
	{
		NONE = 0,
		SET_STATUS_LED = 1,
		DISABLE_STATUS_LED = 2,
		STATUS_LED = 3,
		RESERVED = 4,
		SET_COIL_A = 5,
		DISABLE_COIL_A = 6,
		SET_COIL_B = 7,
		DISABLE_COIL_B = 8,
		NFC_READ_RQUEST = 9,
		NFC_READ_ANSWER = 10,
		NFC_READ_CANCEL = 0, ///NOTE THE 0 -> 0 is always aborting
		
		///RESERVED FOR THE TURN STATE LIGHT
		///BASE ADRESS + VALUE OF TURN_STATE_LIGHT ENUM = FINAL REGISTER
		TURN_STATE_LIGHT_BASE_ADDRESS = 11, 
		RES_2 = 12,
		RES_3 = 13,
		RES_4 = 14,
		RES_5 = 15,
		
		COIL_INVERT_ENABLE_A = 16,
		COIL_INVERT_ENABLE_B = 17,
		
		COIL_INVERT_DISBALE_A = 18,
		COIL_INVERT_DISBALE_B = 19
			
	};
	bool board_init_success = false;
	const int SPI_CS_GPIO = 3; //WIRING PI NUMBERING
	SPICommunication::SPI_DEVICE SPI_CS_DEVICE;
};
#endif


