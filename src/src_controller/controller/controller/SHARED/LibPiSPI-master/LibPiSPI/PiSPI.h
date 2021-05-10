#pragma once
#ifndef __PiSPI_H_
#define __PiSPI_H_

// Includes
#include <mutex>
#include <cstdint>
#include <linux/spi/spidev.h>

//Namespace STD to avoid std::
using namespace std;



//PiSPI class
//Class for low level SPI communicationn
class PiSPI
{
	//Constructor / Destructor
	//Constructor - Sets up the SPI Port and opens it. Singleton due to HW.
public:
	PiSPI(uint8_t channel, int speed, int mode, uint8_t bitsperword = 8);
	~PiSPI();

	//Methods
public:
	bool SetMode(int mode);
	int GetMode();

	bool SetBitsPerWord(uint8_t bpw);
	uint8_t GetBitsPerWord();

	bool SetSpeed(int speed);
	int GetSpeed();

	bool Write(uint8_t reg, uint8_t* pData, size_t length);
	bool Write(uint8_t* pData, size_t length);
	bool Read(uint8_t reg, uint8_t* pData, size_t length);
	bool Read(uint8_t* pData, size_t length);
	bool SyncReadWrite(uint8_t* pData, size_t length);

	//Fields
public:
	static std::mutex mutexSPI[2];

private:
	uint8_t  _u8Channel;
	int _iSpeed;
	uint8_t _iBitsPerWord;
	int _iMode;
	int _iFD;

};

#endif //__PiSPI_H_

/*
Copyright (c) 2018 Tobias Eiseler

This file is part of LibPiSPI.

Foobar is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/