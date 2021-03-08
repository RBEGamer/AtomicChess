#include "IOController.h"


IOController::IOController()
{
	
	if (!SPICommunication::getInstance()->isInitialised()) {
		if (SPICommunication::getInstance()->lastErrorCode() != 0) {
			board_init_success = false;
			return;
			
		}
		board_init_success = false;
		return;
	}
	
	SPI_CS_DEVICE = SPICommunication::SPI_DEVICE::IO_CONTROLLER;
	SPICommunication::getInstance()->register_cs_gpio(SPI_CS_DEVICE, SPI_CS_GPIO);

	//CHECKS THE STATE OF THE BOARD
	uint8_t buffer[] = {IOController::COMMANDS::STATUS_LED};
	int res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE, buffer, 1);
	res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE, buffer, 1);
	//CHECK THE RESULT
	if(res == 0 && buffer[0] == IOController::COMMANDS::STATUS_LED) {
		//TODO hmm ...
	}
	board_init_success = true;
}

void IOController::setCoilState(IOController::COIL _coil, bool _state)
{
	
	unsigned char cmd = -1;
	if (_coil == IOController::COIL::COIL_A)
	{
		if (_state)
		{
			cmd = IOController::COMMANDS::SET_COIL_A;
		}
		else
		{
			cmd = IOController::COMMANDS::DISABLE_COIL_A;
		}
	}
	else if (_coil == IOController::COIL::COIL_B)
	{
		if (_state)
		{
			cmd = IOController::COMMANDS::SET_COIL_B;
		}
		else
		{
			cmd = IOController::COMMANDS::DISABLE_COIL_B;
		}
	}
	
	//DO NOTHING IF ANYTHING ELESE IS CHOOSEN THAN ACOIL_A AND COIL_B
	if(cmd < 0) {
		return;
	}
	
	
	uint8_t buffer_w[] = { cmd };
	uint8_t buffer_r[] = { 0 };
	
	int res = SPICommunication::getInstance()->spi_write_ack(SPI_CS_DEVICE, buffer_w, 1);      //WRITE CMD
}

void IOController::setStatusLed(IOController::LED _led, bool _state)
{
	unsigned char cmd = 0;
	if (_led == IOController::LED::STAUS_LED_A)
	{
		if (_state)
		{
			cmd = IOController::COMMANDS::SET_STATUS_LED;
		}
		else
		{
			cmd = IOController::COMMANDS::DISABLE_STATUS_LED;
		}
	}
	else
	{
		if (_state)
		{
			cmd = IOController::COMMANDS::SET_STATUS_LED;
		}
		else
		{
			cmd = IOController::COMMANDS::DISABLE_STATUS_LED;
		}
	}
	
	uint8_t buffer_w[] = { cmd };
	uint8_t buffer_r[] = { 0 };
	
	int res = SPICommunication::getInstance()->spi_write_ack(SPI_CS_DEVICE, buffer_w, 1);      //WRITE CMD
}

ChessPiece::FIGURE IOController::ScanNFC(int _retry_count)
{
	//POPULATE CMD ARRAYS
	uint8_t buffer_w[] = { IOController::COMMANDS::NFC_READ_RQUEST };
	uint8_t buffer_r[] = { 0 };
	
	//SEND NFC READ REQUEST
	int res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE, buffer_w, 1);   //WRITE CMD
	res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE, buffer_r, 1);    //READ BACK
	//WAIT FOR ANSWER
	volatile int retry = 0;
	bool figure_valid = false;
	ChessPiece::FIGURE tmp_figure;
	while (!figure_valid && retry < _retry_count)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(NFC_READ_ACK_DELAY));
		res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE, buffer_r, 1);
		//PARSE RESULT TO A FIGURE AND CHECK IF ITS VALID
		tmp_figure = ChessPiece::NDEF2Figure(buffer_r[0]);
		if (tmp_figure.type != ChessPiece::TYPE::TYPE_INVALID) {
			figure_valid = true;
			
		}
		retry++;
	}
	//SEND NFC READ/FINISH COMMAND
	buffer_w[0] = IOController::COMMANDS::NFC_READ_CANCEL;
	res = SPICommunication::getInstance()->spi_write(SPI_CS_DEVICE, buffer_w, 1);
	
	//PARSE RESULT TO A FIGURE
	
	return tmp_figure;
}

bool  IOController::isInitialized()
{
	return board_init_success;
}



IOController::~IOController()
{
	
}

void IOController::invertCoilPolarity(IOController::COIL _coil, bool _invert_field)
{
	unsigned char cmd = -1;
	if (_coil == IOController::COIL::COIL_A)
	{
		if (_invert_field)
		{
			cmd = IOController::COMMANDS::COIL_INVERT_ENABLE_A;
		}
		else
		{
			cmd = IOController::COMMANDS::COIL_INVERT_DISBALE_A;
		}
	}
	else if (_coil == IOController::COIL::COIL_B)
	{
		if (_invert_field)
		{
			cmd = IOController::COMMANDS::COIL_INVERT_ENABLE_B;
		}
		else
		{
			cmd = IOController::COMMANDS::COIL_INVERT_DISBALE_B;
		}
	}
	
	//DO NOTHING IF ANYTHING ELESE IS CHOOSEN THAN ACOIL_A AND COIL_B
	if (cmd < 0) {
		return;
	}
	
	//FINALLY WRITE CMD TO SPI BUS
	uint8_t buffer_w[] = { cmd };
	uint8_t buffer_r[] = { 0 };
	
	int res = SPICommunication::getInstance()->spi_write_ack(SPI_CS_DEVICE, buffer_w, 1);     //WRITE CMD
	
	
}





void IOController::setTurnStateLight(IOController::TURN_STATE_LIGHT _state)
{
	
	//CALCULATE THE REGISTER ADRESS
	
		unsigned char cmd = _state + IOController::COMMANDS::TURN_STATE_LIGHT_BASE_ADDRESS;
	
	
		uint8_t buffer_w[] = { cmd };
		uint8_t buffer_r[] = { 0 };
	
		int res = SPICommunication::getInstance()->spi_write_ack(SPI_CS_DEVICE, buffer_w, 1);    //WRITE CMD
}